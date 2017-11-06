/*
* FileName        :    main.c
* Description     :    This file contains concurrent SW implementation of a Beaglebone Linux
*                      system handling multiple sensor interfacing and data logging in a multi-threaded
*                      synchronized way.
*                        
* File Author Name:    Bhallaji Venkatesan, Divya Sampath Kumar
* Tools used      :    gcc, gedit, Sublime Text
* References      :    
*
*
*/

#include <stdio.h>
#include <mqueue.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>
#include "message.h"
#include "fw_i2c.h"
#include "i2c_temp.h"
#include "i2c_light.h"
#define MSG_QUEUE "/msg_queue"
#define HB_TEMP_QUEUE "/hb_temp_queue"
#define HB_LIGHT_QUEUE "/hb_light_queue"
#define HB_TEMP_REQ_QUEUE "/temp_req_queue"

#define LIGHT_HB_CHECK_CNT 1
#define TEMP_HB_CHECK_CNT  2
#define LOG_HB_CHECK_CNT   3

#define HB_LOG_QUEUE "/hb_log_queue"
#define MAX_LOG_SIZE     9999

char logbuff[MAX_LOG_SIZE];
pthread_t tempsensor_thread;
pthread_t lightsensor_thread;
pthread_t synclogger_thread;
struct mq_attr mq_attr_queue;
struct mq_attr mq_attr_hb_queue;
struct mq_attr mq_attr_log_queue;
struct mq_attr mq_attr_temp_req_queue;

static volatile sig_atomic_t blocking = 1;
static volatile sig_atomic_t light_flag = 0;
static volatile sig_atomic_t temp_flag = 0;
static volatile sig_atomic_t request_flag_temp = 0;
static volatile sig_atomic_t req_processed = 0;
static volatile sig_atomic_t write_complete = 0;
static volatile sig_atomic_t req_cnt = 0;
static volatile sig_atomic_t shutdown_disable_complete = 0;
static volatile sig_atomic_t shutdown_enable_complete = 0;
static volatile sig_atomic_t conv_rate_set = 0;

pthread_cond_t sig_logger;
pthread_mutex_t logger_mutex;
pthread_cond_t proceed_init;
pthread_mutex_t proceed_init_mutex;
pthread_mutex_t msg_temp_mutex;
pthread_mutex_t msg_light_mutex;
pthread_cond_t sig_req_process;
pthread_mutex_t req_process_mutex;

pthread_mutex_t i2c_init_mutex;
pthread_mutex_t i2c_rw_mutex;
//pthread_mutex_t msg_light_mutex;


uint8_t fd_temp;
uint8_t fd_light;
uint8_t hb_temp_cnt;
uint8_t hb_light_cnt;
uint8_t hb_synclog_cnt;
static uint8_t temp_hb_light;
static mqd_t msg_queue;
static mqd_t hb_temp_queue;
static mqd_t hb_light_queue;
static mqd_t hb_log_queue;
static mqd_t temp_req_queue;

FILE *fp;
char *i2c_temp_fname = "/dev/i2c-2";
char *i2c_light_fname = "/dev/i2c-2";
//static unsigned int counter;
static volatile sig_atomic_t counter =0;
logpacket msg_tempsensor,msg_lightsensor, msg_synclogger;

void exit_handler(int sig) {
    if (sig != SIGINT) {
        printf("Received invalid signum = %d in exit_handler()\n", sig);
        exit(0);
    }
    
    printf("Received SIGINT. Exiting Application\n");

    pthread_cancel(tempsensor_thread);
    pthread_cancel(lightsensor_thread);
    pthread_cancel(synclogger_thread);
    
    mq_close(msg_queue);
    mq_unlink(MSG_QUEUE);
    mq_close(hb_light_queue);
    mq_unlink(HB_LIGHT_QUEUE);
    mq_close(hb_temp_queue);
    mq_unlink(HB_TEMP_QUEUE);
    mq_close(hb_log_queue);
    mq_unlink(HB_LOG_QUEUE);
    mq_close(temp_req_queue);
    mq_unlink(HB_TEMP_REQ_QUEUE);
   // if(fd_temp !=NULL)
    //fclose(fd_temp);
    if(fp!=NULL)
    fclose(fp);
    exit(0);
}

enum Status api_temp_log(logpacket msg)
{
    uint8_t status;
    
    status=mq_send(hb_log_queue, (const logpacket*)&msg, sizeof(msg),1);
    if(status == -1)
    {
        printf("\ntemp was unable to send log message\n");
        return FAIL;
        //ERR_Log();
    }
    pthread_cond_signal(&sig_logger);


    return SUCCESS;

}

enum Status api_temp_req_hdlr()
{   
    logpacket msg_request;
    logpacket api_req_msg;
    msg_request.req_type = -1;
    uint8_t status;
    char *temp_buff_float = (char*)malloc(sizeof(float));
    char *temp_buff_uint16 = (char*)malloc(sizeof(uint16_t));
    uint16_t temp_value_uint16;
    float temp_value_float;
    uint16_t temp_write_value;
    status = mq_receive(temp_req_queue,(logpacket*)&msg_request, sizeof(msg_request), NULL);
    if(status >0)
    {
        printf("\nReceived Request from source\n");
    }
    else
    {
        return FAIL;
        //ERR_Log();
    }
    printf("\n Message Req Type %d\n",msg_request.req_type);
    switch(msg_request.req_type)
    {
        case REQ_TEMP_CELSIUS:
            temp_value_float = temp_read(fd_temp,REQ_TEMP_CELSIUS);
            sprintf(temp_buff_float,"%f",temp_value_float);
            gettimeofday(&msg_tempsensor.time_stamp, NULL);
            strcpy(msg_tempsensor.logmsg,temp_buff_float);
            printf("\ntempbuff in logpacket %s\n",msg_tempsensor.logmsg);
            msg_tempsensor.req_type = REQ_TEMP_CELSIUS;
            if(msg_tempsensor.logmsg != NULL)
            {
                if(api_temp_log(msg_tempsensor))
                {
                    printf("\ntemp was unable to log data request\n");
                    //ERR_Log();
                    return FAIL;
                }
                pthread_cond_signal(&sig_req_process);
                status=mq_send(temp_req_queue, (const logpacket*)&msg_tempsensor, sizeof(msg_tempsensor),1);
                if(status == -1)
                {
                    printf("\ntemp was unable to send log message\n");
                    return FAIL;
                    //ERR_Log();
                }
                req_processed = 1;                
            }

            break;
        case REQ_TEMP_KELVIN:
            temp_value_float = temp_read(fd_temp,REQ_TEMP_KELVIN);
            sprintf(temp_buff_float,"%f",temp_value_float);
            gettimeofday(&msg_tempsensor.time_stamp, NULL);
            strcpy(msg_tempsensor.logmsg,temp_buff_float);
            printf("\ntempbuff in logpacket %s\n",msg_tempsensor.logmsg);
            msg_tempsensor.req_type = REQ_TEMP_KELVIN;
            if(msg_tempsensor.logmsg != NULL)
            {
                if(api_temp_log(msg_tempsensor))
                {
                    printf("\ntemp was unable to log data request\n");
                    //ERR_Log();
                    return FAIL;
                }
                pthread_cond_signal(&sig_req_process);
                status=mq_send(temp_req_queue, (const logpacket*)&msg_tempsensor, sizeof(msg_tempsensor),1);
                if(status == -1)
                {
                    printf("\ntemp was unable to send log message\n");
                    return FAIL;
                    //ERR_Log();
                }
                req_processed = 1;
            }
            break;            
        case REQ_TEMP_FAHRENHEIT:
            temp_value_float = temp_read(fd_temp,REQ_TEMP_FAHRENHEIT);
            sprintf(temp_buff_float,"%f",temp_value_float);
            gettimeofday(&msg_tempsensor.time_stamp, NULL);
            strcpy(msg_tempsensor.logmsg,temp_buff_float);
            printf("\ntempbuff in logpacket %s\n",msg_tempsensor.logmsg);
            msg_tempsensor.req_type = REQ_TEMP_FAHRENHEIT;
            if(msg_tempsensor.logmsg != NULL)
            {
                if(api_temp_log(msg_tempsensor))
                {
                    printf("\ntemp was unable to log data request\n");
                    //ERR_Log();
                    return FAIL;
                }
                pthread_cond_signal(&sig_req_process);
                status=mq_send(temp_req_queue, (const logpacket*)&msg_tempsensor, sizeof(msg_tempsensor),1);
                if(status == -1)
                {
                    printf("\ntemp was unable to send log message\n");
                    return FAIL;
                    //ERR_Log();
                }
                req_processed = 1;
            }
            break;
        case REQ_TEMPREG_CONFIG_READ:
            printf("\nTEMP: CONFIG REGISTER REQUEST RECVD\n");
            status = read_temp_register(fd_temp,REQ_TEMPREG_CONFIG_READ,&temp_value_uint16);
            sprintf(temp_buff_uint16,"%4x",temp_value_uint16);
            gettimeofday(&api_req_msg.time_stamp, NULL);
            printf("\ntempbuff in logpacket %s\n",api_req_msg.logmsg);
            strcpy(api_req_msg.logmsg,temp_buff_uint16);
            api_req_msg.req_type = REQ_TEMPREG_CONFIG_READ;
            if(api_req_msg.logmsg != NULL)
            {
                if(api_temp_log(api_req_msg))
                {
                    printf("\ntemp was unable to log data request\n");
                    //ERR_Log();
                    return FAIL;
                }
                pthread_cond_signal(&sig_req_process);
                status=mq_send(temp_req_queue, (const logpacket*)&api_req_msg, sizeof(api_req_msg),1);
                if(status == -1)
                {
                    printf("\ntemp was unable to send log message\n");
                    return FAIL;
                    //ERR_Log();
                }
                req_processed = 1;
            }
            break;
        case REQ_TEMPREG_READ:
            printf("\nTEMP :TEMP REGISTER REQUEST RECVD\n");
            status = read_temp_register(fd_temp,REQ_TEMPREG_READ,&temp_value_uint16);
            sprintf(temp_buff_uint16,"%4x",temp_value_uint16);
            gettimeofday(&api_req_msg.time_stamp, NULL);
            printf("\ntempbuff in logpacket %s\n",api_req_msg.logmsg);
            strcpy(api_req_msg.logmsg,temp_buff_uint16);
            api_req_msg.req_type = REQ_TEMPREG_READ;
            if(api_req_msg.logmsg != NULL)
            {
                if(api_temp_log(api_req_msg))
                {
                    printf("\ntemp was unable to log data request\n");
                    //ERR_Log();
                    return FAIL;
                }
                pthread_cond_signal(&sig_req_process);
                status=mq_send(temp_req_queue, (const logpacket*)&api_req_msg, sizeof(api_req_msg),1);
                if(status == -1)
                {
                    printf("\ntemp was unable to send log message\n");
                    return FAIL;                        
                    //ERR_Log();
                }
                req_processed = 1;
                }
                break;

        case REQ_TEMPREG_DATA_LOW_READ:
            printf("\nTEMP CONFIG REQUEST RECVD\n");
            status = read_temp_register(fd_temp,REQ_TEMPREG_DATA_LOW_READ,&temp_value_uint16);
            sprintf(temp_buff_uint16,"%4x",temp_value_uint16);
            gettimeofday(&api_req_msg.time_stamp, NULL);
            printf("\ntempbuff in logpacket %s\n",api_req_msg.logmsg);
            strcpy(api_req_msg.logmsg,temp_buff_uint16);
            api_req_msg.req_type = REQ_TEMPREG_DATA_LOW_READ;
            if(api_req_msg.logmsg != NULL)
            {
                if(api_temp_log(api_req_msg))
                {
                    printf("\ntemp was unable to log data request\n");
                    //ERR_Log();
                    return FAIL;
                }
                pthread_cond_signal(&sig_req_process);
                status=mq_send(temp_req_queue, (const logpacket*)&api_req_msg, sizeof(api_req_msg),1);
                if(status == -1)
                {
                    printf("\ntemp was unable to send log message\n");
                    return FAIL;
                    //ERR_Log();
                }
                req_processed = 1;
            }
            break;
        case REQ_TEMPREG_DATA_HIGH_READ: 
            printf("\nTEMP CONFIG REQUEST RECVD\n");
            status = read_temp_register(fd_temp,REQ_TEMPREG_DATA_HIGH_READ,&temp_value_uint16);
            sprintf(temp_buff_uint16,"%4x",temp_value_uint16);
            gettimeofday(&api_req_msg.time_stamp, NULL);
            printf("\ntempbuff in logpacket %s\n",api_req_msg.logmsg);
            strcpy(api_req_msg.logmsg,temp_buff_uint16);
            api_req_msg.req_type = REQ_TEMPREG_DATA_HIGH_READ;
            if(api_req_msg.logmsg != NULL)req_processed = 1;
            {
                if(api_temp_log(api_req_msg))
                {
                    printf("\ntemp was unable to log data request\n");
                    //ERR_Log();
                    return FAIL;
                }
                pthread_cond_signal(&sig_req_process);
                status=mq_send(temp_req_queue, (const logpacket*)&api_req_msg, sizeof(api_req_msg),1);
                if(status == -1)
                {
                    printf("\ntemp was unable to send log message\n");
                    return FAIL;
                    //ERR_Log();
                }
                req_processed = 1;
            }
            break;
        case REQ_TEMPREG_PTRREG_WRITE:
        	printf("\nTEMP PTR WRITE REQUEST RCVD\n");
        	temp_write_value = (uint16_t)atoi(msg_request.logmsg);  
        	status = write_temp_register(fd_temp,REQ_TEMPREG_PTRREG_WRITE,temp_write_value);
        	if(status)
        	{
        		strcpy(msg_request.logmsg,"INVALID PTR WRITE VALUE");
        	}
        	if(api_temp_log(msg_request))
            {
                printf("\ntemp was unable to log data request\n");
                //ERR_Log();
                return FAIL;
            }
                pthread_cond_signal(&sig_req_process);
        	//IF WRITE IS SUCCESSFUL
        	if(!status)
        	write_complete = 1;
        	req_processed = 1;
        	break;
        case REQ_TEMPREG_CONFIG_WRITE:
        	printf("\nTEMP CONFIG WRITE REQUEST RCVD\n");
        	temp_write_value = (uint16_t)atoi(msg_request.logmsg);  
        	status = write_temp_register(fd_temp,REQ_TEMPREG_CONFIG_WRITE,temp_write_value);
        	if(status)
        	{
        		strcpy(msg_request.logmsg,"INVALID CONFIG WRITE VALUE");
        	}
        	if(api_temp_log(msg_request))
            {
                printf("\ntemp was unable to log data request\n");
                //ERR_Log();
                return FAIL;
            }
                pthread_cond_signal(&sig_req_process);
        	//IF WRITE IS SUCCESSFUL
        	if(!status)
        	write_complete = 1;
        	req_processed = 1;
        	break;

        case REQ_TEMPREG_DATA_LOW_WRITE:
        	printf("\nTEMP TLOW WRITE REQUEST RCVD\n");
        	temp_write_value = (uint16_t)atoi(msg_request.logmsg);  
        	status = write_temp_register(fd_temp,REQ_TEMPREG_DATA_LOW_WRITE,temp_write_value);
        	if(status)
        	{
        		strcpy(msg_request.logmsg,"INVALID TLOW WRITE VALUE");
        	}
        	if(api_temp_log(msg_request))
            {
                printf("\ntemp was unable to log data request\n");
                //ERR_Log();
                return FAIL;
            }
                pthread_cond_signal(&sig_req_process);
        	//IF WRITE IS SUCCESSFUL
        	if(!status)
        	write_complete = 1;
        	req_processed = 1;
        	break;
        case REQ_TEMPREG_DATA_HIGH_WRITE:
        	printf("\nTEMP TLOW WRITE REQUEST RCVD\n");
        	temp_write_value = (uint16_t)atoi(msg_request.logmsg);  
        	status = write_temp_register(fd_temp,REQ_TEMPREG_DATA_HIGH_WRITE,temp_write_value);
        	if(status)
        	{
        		strcpy(msg_request.logmsg,"INVALID THIGH WRITE VALUE");
        	}
        	if(api_temp_log(msg_request))
            {
                printf("\ntemp was unable to log data request\n");
                //ERR_Log();
                return FAIL;
            }
            pthread_cond_signal(&sig_req_process);
        	//IF WRITE IS SUCCESSFUL
        	if(!status)
        	write_complete = 1;
        	req_processed = 1;
        	break;  
        case REQ_SHUTDOWN_ENABLE:
        	printf("\nTEMP Sensor Shutdown Enable REQUEST RCVD\n");
        	status= shutdown_temp_mode(fd_temp,1);
        	if(!status)
        	{
        		shutdown_enable_complete = 1;
        	}
        	if(api_temp_log(msg_request))
            {
                printf("\ntemp was unable to log data request\n");
                //ERR_Log();
                return FAIL;
            }
            pthread_cond_signal(&sig_req_process);
        	req_processed = 1;
        	break;
        	
        case REQ_SHUTDOWN_DISABLE:
        	printf("\nTEMP Sensor Shutdown Disable REQUEST RCVD\n");
        	status= shutdown_temp_mode(fd_temp,0);
        	if(!status)
        	{
        		shutdown_disable_complete = 1;
        	}
        	if(api_temp_log(msg_request))
            {
                printf("\ntemp was unable to log data request\n");
                //ERR_Log();
                return FAIL;
            }
            pthread_cond_signal(&sig_req_process);
        	req_processed = 1;
        	break;
        case REQ_TEMP_CONV_RATE_0_25_HZ:
        	printf("\nTEMP Sensor Conversion Rate Set as 0.25HZ REQUEST RCVD\n");
        	status = continuous_conversion_mode(fd_temp,0);
        	if(!status)
        	{
        		conv_rate_set = 1;
        	}
        	if(api_temp_log(msg_request))
            {
                printf("\ntemp was unable to log data request\n");
                //ERR_Log();
                return FAIL;
            }
            pthread_cond_signal(&sig_req_process);
        	req_processed = 1;
        	break;
        case REQ_TEMP_CONV_RATE_1_HZ:
        	printf("\nTEMP Sensor Conversion Rate Set as 1HZ REQUEST RCVD\n");
        	status = continuous_conversion_mode(fd_temp,1);
        	if(!status)
        	{
        		conv_rate_set = 1;
        	}
        	if(api_temp_log(msg_request))
            {
                printf("\ntemp was unable to log data request\n");
                //ERR_Log();
                return FAIL;
            }
            pthread_cond_signal(&sig_req_process);
        	req_processed = 1;
        	break;        
        case REQ_TEMP_CONV_RATE_4_HZ:
        	printf("\nTEMP Sensor Conversion Rate Set as 4HZ REQUEST RCVD\n");
        	status = continuous_conversion_mode(fd_temp,2);
        	if(!status)
        	{
        		conv_rate_set = 1;
        	}
        	if(api_temp_log(msg_request))
            {
                printf("\ntemp was unable to log data request\n");
                //ERR_Log();
                return FAIL;
            }
            pthread_cond_signal(&sig_req_process);
        	req_processed = 1;
        	break;
        case REQ_TEMP_CONV_RATE_8_HZ:
        	printf("\nTEMP Sensor Conversion Rate Set as 8HZ REQUEST RCVD\n");
        	status = continuous_conversion_mode(fd_temp,3);
        	if(!status)
        	{
        		conv_rate_set = 1;
        	}
        	if(api_temp_log(msg_request))
            {
                printf("\ntemp was unable to log data request\n");
                //ERR_Log();
                return FAIL;
            }
            pthread_cond_signal(&sig_req_process);
        	req_processed = 1;
        	break;        	

    }
                    

    return SUCCESS;
}


enum Status init_tempsensor_task()
{
    uint8_t status;
    logpacket msg;
    fd_temp = open(i2c_temp_fname,O_RDWR);
    if(fd_temp<0)
    {
        printf("\nFailed to Open\n ");
        return FAIL;
        //ERR_Log();
    }
    /*if(pthread_mutex_lock(&i2c_init_mutex) !=0)
    {
        printf("\nERR: Unable to Lock\n");
    }*/
    status = i2c_temp_init(fd_temp,0x48);
    /*if(pthread_mutex_unlock(&i2c_init_mutex))
    {
        printf("\nERR: Unable to unlock\n");
    }*/
    if(status)
    {
        printf("\nERR:Failed to Init I2C\n ");
        return FAIL;
        //ERR_Log();   
    }
    msg.sourceid = SRC_TEMPERATURE;
    msg.level = LOG_INIT;
    char *init_msg = "\nTEMP :Initialization Done";
    strcpy(msg.logmsg,init_msg);
    api_temp_log(msg);
    pthread_cond_signal(&proceed_init);
    return SUCCESS;
}

void *app_tempsensor_task(void *args) // Temperature Sensor Thread/Task
{
    uint32_t usecs;
    uint8_t status;
    uint8_t temp_count;
    logpacket msg_request;
    usecs = 10000;
    printf("\nIn Temperature Sensor Thread execution\n");
    //Creating Log message in logpacket
    status = init_tempsensor_task();
    if(status)
    {
        printf("\nTemp Sensor Init Failed\n");
        //ERR_Log();
        //Log Error in Log Queue
    }
    
    float temp_value = 0;
    //float temp_value = 35.02;
    char *temp_buff = (char*)malloc(sizeof(float));
    if(!temp_buff)
    {
    	printf("\nERR:Malloc Error");
        //ERR_Log();
    }
    while(1)
    {
        status=mq_send(msg_queue, (const char*)&counter, sizeof(counter),1);
        if(status == -1)
        {
            exit_handler(SIGINT);
            //ERR_Log();
        }
        /*if(pthread_mutex_lock(&i2c_rw_mutex) !=0)
        {
            printf("\nERR: Unable to Lock\n");
        }*/
        temp_value = temp_read(fd_light,REQ_TEMP_CELSIUS);
        //temp_value++;
        if(pthread_mutex_unlock(&i2c_rw_mutex) !=0)
        {
            printf("\nERR: Unable to unlock\n");
        }
        sprintf(temp_buff,"%f",temp_value);
        gettimeofday(&msg_tempsensor.time_stamp, NULL);
        strcpy(msg_tempsensor.logmsg,temp_buff);
        printf("\ntempbuff in logpacket %s\n",msg_tempsensor.logmsg);
        msg_tempsensor.sourceid = SRC_TEMPERATURE;
        if(msg_tempsensor.logmsg != NULL)
        {
            if(api_temp_log(msg_tempsensor))
            {
                printf("\ntemp was unable to send log message\n");
                //ERR_log();
            }

        }
        usleep(usecs);
        if(request_flag_temp)
        {
            printf("\nRequest Flag Set\n");
            if(api_temp_req_hdlr())
            {
                printf("\nERR: TEMP Task Unable to Handle Request\n");
                //ERR_log();
            }
            request_flag_temp = 0;
        }

        if(temp_flag)
        {
            status = mq_receive(hb_temp_queue,(char*)&temp_count, sizeof(counter), NULL);
            if(status >0)
            {
                printf("\nReceive Heartbeat temp request: %d\n", temp_count);
                hb_temp_cnt+=1;
                if(hb_temp_cnt %200 == 0)
                {
                    hb_temp_cnt = 0;   
                }
                status=mq_send(hb_temp_queue, (const char*)&hb_temp_cnt, sizeof(counter),1);

            }
            temp_flag = 0;
        } 
    }

	//Do Sensor data acquisition
	//Send heartbeat notif when requested
	//Log essentials/ errors
	//Log data into log queues 

}
 
enum Status init_lightsensor_task()
{
    uint8_t status;
    logpacket msg;
    //pthread_mutex_lock()
    pthread_cond_wait(&proceed_init,&proceed_init_mutex);
    printf("\nInit Light Task\n");
    //if(!fd_temp)
    //{
        fd_light= open(i2c_light_fname,O_RDWR);
    //}
    if(fd_light<0)
    {
        printf("\nFailed to Open\n ");
        return FAIL;
        //ERR_Log();
    }
   /* if(pthread_mutex_lock(&i2c_init_mutex) !=0)
    {
        printf("\nERR: Unable to Lock\n");
    }*/
    status = i2c_light_init(fd_light,DEV_LIGHT_ADDR);
    /*if(pthread_mutex_unlock(&i2c_init_mutex) !=0)
    {
        printf("\nERR: Unable to Lock\n");
    }*/
    if(status)
    {
        printf("\nERR:Failed to Init I2C\n ");
        return FAIL;
        //ERR_Log();   
    }
    msg.sourceid = SRC_LIGHT;
    msg.level = LOG_INIT;
    char *init_msg = "\nLIGHT :Initialization Done";
    strcpy(msg.logmsg,init_msg);
    api_temp_log(msg);

    return SUCCESS;
}

void *app_lightsensor_task(void *args) //Light Sensor Thread/Task
{
	uint32_t usecs;
    int status;
    int recvcounter;
    uint8_t temp_count;
    usecs = 45000;
    printf("\nIn Light Sensor Thread execution\n");
    printf("\nRecvcounter is %d\n",recvcounter);
    init_lightsensor_task();
    double temp_value;
    char *temp_buff = (char*)malloc(sizeof(float));
    if(!temp_buff)
    {
        printf("\nERR:Malloc Error");
    }


    while(1)
    {
        
       /* status = mq_receive(msg_queue,(char*)&recvcounter, sizeof(recvcounter), NULL);
        if(status >0)
        {
            printf("\nRecevied Message in Lightsensor Thread : %d\n",recvcounter);
            counter+=1;
            if(counter % 200 == 0)
            {
                counter = 0;
            }
        }*/
        /*if(pthread_mutex_lock(&i2c_rw_mutex) !=0)
        {
            printf("\nERR: Unable to Lock\n");
        }*/
        temp_value = light_read(fd_temp);
        /*if(pthread_mutex_unlock(&i2c_rw_mutex) !=0)
        {
            printf("\nERR: Unable to unlock\n");
        }*/
        sprintf(temp_buff,"%lf",temp_value);
        //msg_tempsensor.msg_size = strlen(temp_buff);
        gettimeofday(&msg_lightsensor.time_stamp, NULL);
        strcpy(msg_lightsensor.logmsg, temp_buff);
        msg_lightsensor.sourceid = SRC_LIGHT;
        if(msg_lightsensor.logmsg != NULL)
        {
            status=mq_send(hb_log_queue, (const logpacket*)&msg_lightsensor, sizeof(msg_lightsensor),1);
            if(status == -1)
            {
                printf("\ntemp was unable to send log message\n");
            }
            //pthread_cond_signal(&sig_logger);
        }
        //temp_value++;
        usleep(usecs);
        if(light_flag)
        {
            status = mq_receive(hb_light_queue,(char*)&temp_count, sizeof(counter), NULL);
            if(status >0)
            {
                printf("\nReceive Heartbeat request: %d\n", temp_count);
                hb_light_cnt+=1;
                if(hb_light_cnt %200 == 0)
                {
                    hb_light_cnt = 0;
                }
                status=mq_send(hb_light_queue, (const char*)&hb_light_cnt, sizeof(counter),1);

            }
            light_flag = 0;
        }   
    }

    //Do Sensor data acquisition
	//Send heartbeat notif when requested
	//Log essentials/ errors
	//Log data into log queues 

}

void *app_sync_logger(void *args) // Synchronization Logger Thread/Task
{
	if(!args)
	{
		printf("\nERR:Invalid Log Filename!\n");
		pthread_exit(NULL);
	}
	char *fname = (char*)args;
	
	//fp = fopen(fname,w+);
	if(!(fp= fopen(fname,"w+")))
	{
		printf("\nERR:Invalid Log Filename fp!\n");
		pthread_exit(NULL);
	} 
    uint32_t usecs;
    usecs = 20000;
	//Wait on temperature Thread to log
    while(1)
    {
	pthread_cond_wait(&sig_logger, &logger_mutex);

    int status =0;
    logpacket temp; 
    temp.req_type = -1;
    //temp.logmsg = NULL;
    status = mq_receive(hb_log_queue,(logpacket*)&temp, sizeof(temp), NULL);
        printf("\nLog Status %d\n",status);
        if(temp.logmsg !=NULL)
        {
            if(status >0)
            {
                printf("\nLog Queue message received\n");
               // printf("\n%d\n",strlen((char*)temp.logmsg));
                if(temp.sourceid == SRC_LIGHT)
                {
                    if(temp.level == LOG_INIT)
                    {
                        strcpy(logbuff,temp.logmsg);
                    }
                    else
                    {
                        sprintf(logbuff,"\n[ %ld sec, %ld usecs] LUX :", temp.time_stamp.tv_sec,temp.time_stamp.tv_usec);
                        strncat(logbuff, temp.logmsg, strlen(temp.logmsg));
                    }
                    
                   // sprintf(logbuff,"%s","\n[1sec,44usec] LUX :");
                }
                else if(temp.sourceid == SRC_TEMPERATURE)
                {
                    if(temp.level == LOG_INIT)
                    {
                        strcpy(logbuff,temp.logmsg);
                    }
                    else
                    {
                        sprintf(logbuff,"\n[ %ld sec, %ld usec] TEMP :",temp.time_stamp.tv_sec,temp.time_stamp.tv_usec);
                        strncat(logbuff, temp.logmsg, strlen(temp.logmsg));
                    }    

                    //sprintf(logbuff,"%s","\n[1sec,44usec] TEMP :");
                }
                if(temp.req_type == REQ_POWEROFF)
                {
                    sprintf(logbuff,"\nReceived Data Request to Temperature Task! Exiting App!");
                    strncat(logbuff, temp.logmsg, strlen(temp.logmsg));
                    fwrite(logbuff,1, strlen(logbuff)*sizeof(char),fp);
                    //exit_handler(SIGINT);
                }
                if(temp.req_type == REQ_TEMP_CELSIUS)
                {
                    sprintf(logbuff,"\nReceived Temperature Response in deg.C Value :");
                    strncat(logbuff, temp.logmsg, strlen(temp.logmsg));
                    fwrite(logbuff,1, strlen(logbuff)*sizeof(char),fp);
                    exit_handler(SIGINT);
                }

                if(temp.req_type == REQ_TEMP_KELVIN)
                {
                    sprintf(logbuff,"\nReceived Temperature Response in Kelvin Value :");
                    strncat(logbuff, temp.logmsg, strlen(temp.logmsg));
                    fwrite(logbuff,1, strlen(logbuff)*sizeof(char),fp);
                    exit_handler(SIGINT);
                }
                 if(temp.req_type == REQ_TEMP_FAHRENHEIT)
                {
                    sprintf(logbuff,"\nReceived Temperature Response in deg.F Value :");
                    strncat(logbuff, temp.logmsg, strlen(temp.logmsg));
                    fwrite(logbuff,1, strlen(logbuff)*sizeof(char),fp);
                    exit_handler(SIGINT);
                }               

                if(temp.req_type == REQ_TEMPREG_CONFIG_READ)
                {
                    sprintf(logbuff,"\nReceived Config Register Value :");
                    strncat(logbuff, temp.logmsg, strlen(temp.logmsg));
                    fwrite(logbuff,1, strlen(logbuff)*sizeof(char),fp);
                    exit_handler(SIGINT);
                }
                if(temp.req_type == REQ_TEMPREG_READ)
                {
                    sprintf(logbuff,"\nReceived TEMP Register Value :");
                    strncat(logbuff, temp.logmsg, strlen(temp.logmsg));
                    fwrite(logbuff,1, strlen(logbuff)*sizeof(char),fp);
                    exit_handler(SIGINT);
                }
                if(temp.req_type == REQ_TEMPREG_DATA_LOW_READ)
                {
                    sprintf(logbuff,"\nReceived TEMP T-LOW Register Value :");
                    strncat(logbuff, temp.logmsg, strlen(temp.logmsg));
                    fwrite(logbuff,1, strlen(logbuff)*sizeof(char),fp);
                    exit_handler(SIGINT);
                }
                if(temp.req_type == REQ_TEMPREG_DATA_HIGH_READ)
                {
                    sprintf(logbuff,"\nReceived TEMP T-HIGH Register Value :");
                    strncat(logbuff, temp.logmsg, strlen(temp.logmsg));
                    fwrite(logbuff,1, strlen(logbuff)*sizeof(char),fp);
                    exit_handler(SIGINT);
                }
                if(temp.req_type == REQ_TEMPREG_PTRREG_WRITE)
                {
                	sprintf(logbuff,"\nReceived WRITE Request to Temp Task with Value :");
                    strncat(logbuff, temp.logmsg, strlen(temp.logmsg));
                    fwrite(logbuff,1, strlen(logbuff)*sizeof(char),fp);
                    exit_handler(SIGINT);

                }
                if(temp.req_type == REQ_TEMPREG_CONFIG_WRITE)
                {
                	sprintf(logbuff,"\nReceived CONFIG WRITE Request to Temp Task with Value :");
                    strncat(logbuff, temp.logmsg, strlen(temp.logmsg));
                    fwrite(logbuff,1, strlen(logbuff)*sizeof(char),fp);
                    exit_handler(SIGINT);

                }
                if(temp.req_type == REQ_TEMPREG_DATA_LOW_WRITE)
                {
                	sprintf(logbuff,"\nReceived TLOW WRITE Request to Temp Task with Value :");
                    strncat(logbuff, temp.logmsg, strlen(temp.logmsg));
                    fwrite(logbuff,1, strlen(logbuff)*sizeof(char),fp);
                    exit_handler(SIGINT);

                }
                if(temp.req_type == REQ_TEMPREG_DATA_HIGH_WRITE)
                {
                	sprintf(logbuff,"\nReceived THIGH WRITE Request to Temp Task with Value :");
                    strncat(logbuff, temp.logmsg, strlen(temp.logmsg));
                    fwrite(logbuff,1, strlen(logbuff)*sizeof(char),fp);
                    exit_handler(SIGINT);
                }
                if(temp.req_type == REQ_SHUTDOWN_ENABLE)
                {
                	sprintf(logbuff,"\nReceived SHUTDOWN Enable Request to Temp Task");
                   // strncat(logbuff, temp.logmsg, strlen(temp.logmsg));
                    fwrite(logbuff,1, strlen(logbuff)*sizeof(char),fp);
                    exit_handler(SIGINT);
                }                                
                if(temp.req_type == REQ_SHUTDOWN_DISABLE)
                {
                	sprintf(logbuff,"\nReceived SHUTDOWN Disable Request to Temp Task");
                   // strncat(logbuff, temp.logmsg, strlen(temp.logmsg));
                    fwrite(logbuff,1, strlen(logbuff)*sizeof(char),fp);
                    exit_handler(SIGINT);
                }
				if(temp.req_type == REQ_TEMP_CONV_RATE_0_25_HZ)
                {
                	sprintf(logbuff,"\nReceived CONV RATE SET Req as 0.25hz");
                   // strncat(logbuff, temp.logmsg, strlen(temp.logmsg));
                    fwrite(logbuff,1, strlen(logbuff)*sizeof(char),fp);
                    exit_handler(SIGINT);
                } 
                if(temp.req_type == REQ_TEMP_CONV_RATE_1_HZ)
                {
                	sprintf(logbuff,"\nReceived CONV RATE SET Req as 1hz");
                   // strncat(logbuff, temp.logmsg, strlen(temp.logmsg));
                    fwrite(logbuff,1, strlen(logbuff)*sizeof(char),fp);
                    exit_handler(SIGINT);
                }               
                if(temp.req_type == REQ_TEMP_CONV_RATE_4_HZ)
                {
                	sprintf(logbuff,"\nReceived CONV RATE SET Req as 4hz");
                   // strncat(logbuff, temp.logmsg, strlen(temp.logmsg));
                    fwrite(logbuff,1, strlen(logbuff)*sizeof(char),fp);
                    exit_handler(SIGINT);
                }
                if(temp.req_type == REQ_TEMP_CONV_RATE_8_HZ)
                {
                	sprintf(logbuff,"\nReceived CONV RATE SET Req as 8hz");
                   // strncat(logbuff, temp.logmsg, strlen(temp.logmsg));
                    fwrite(logbuff,1, strlen(logbuff)*sizeof(char),fp);
                    exit_handler(SIGINT);
                }                  
                //strncat(logbuff, temp.logmsg, strlen(temp.logmsg));
                fwrite(logbuff,1, strlen(logbuff)*sizeof(char),fp);
                //fwrite((uint8_t*)temp.logmsg,1, strlen((uint8_t*)temp.logmsg)*sizeof(uint8_t),fp);
            }    
        }
        usleep(usecs);
        /*status = mq_receive(hb_log_queue,(logpacket*)&temp, sizeof(temp), NULL);
        printf("\nLog Status %d\n",status);
        if(temp.logmsg !=NULL)
        {
            if(status >0)
            {
                printf("\nLog Queue message received\n");
                printf("\n%d\n",strlen((char*)temp.logmsg));
                if(temp.sourceid == SRC_LIGHT)
                {
                    sprintf(logbuff,"\n[ %ld sec, %ld usecs] LUX :", temp.time_stamp.tv_sec,temp.time_stamp.tv_usec);
                    //sprintf(logbuff,"%s","\n[1sec,44usec] LUX :");
                }
                else if(temp.sourceid == SRC_TEMPERATURE)
                {
                    sprintf(logbuff,"\n[ %ld sec, %ld usec] TEMP :",temp.time_stamp.tv_sec,temp.time_stamp.tv_usec);
                    //sprintf(logbuff,"%s","\n[1sec,44usec] TEMP :");
                }
                strncat(logbuff, (char *)temp.logmsg, strlen((char*)temp.logmsg));
                fwrite(logbuff,1, strlen(logbuff)*sizeof(char),fp);
                //fwrite((uint8_t*)temp.logmsg,1, strlen((uint8_t*)temp.logmsg)*sizeof(uint8_t),fp);
            }    
        }*/
    }    
	printf("\nExiting logger Thread\n");
	fclose(fp);
	pthread_exit(NULL);
	//Handle Log Requests
	//Send heartbeat notif when requested
	//Log essentials/ errors
	//Perform Logging into custom log file

}


uint8_t monitor_hb_notif()
{
    uint32_t usecs_send;
    int status;
    int recvcounter;
    
    usecs_send = 4000000;
    uint32_t usecs_total = 4000000;
    uint8_t temp_hb_light = hb_light_cnt;
    uint8_t temp_hb_temp = hb_temp_cnt;
    if(req_cnt == LIGHT_HB_CHECK_CNT)
    {
        status=mq_send(hb_light_queue, (const char*)&hb_light_cnt, sizeof(counter),1);
        printf("\nStatus is %d",status);
        light_flag = 1;
        if(status == -1)
        {
            printf("\nUnable to send Heartbeat Notification\n");
            exit_handler(SIGINT);
        }
        else
        {
            printf("\nSuccessfully sent Heartbeat Notification to Light sensor\n");   
        }

    }

    if(req_cnt == TEMP_HB_CHECK_CNT)
    {
        status=mq_send(hb_temp_queue, (const char*)&hb_temp_cnt, sizeof(counter),1);
        temp_flag = 1;
        printf("\nStatus is %d",status);
        if(status == -1)
        {
            printf("\nUnable to send Heartbeat Notification\n");
            exit_handler(SIGINT);
        }
        else
        {
            printf("\nSuccessfully sent Heartbeat Notification to Temp sensor\n");   
        }
    }

    usleep(usecs_send);
    if(req_cnt == LIGHT_HB_CHECK_CNT)
    {   
        status = mq_receive(hb_light_queue,(char*)&hb_light_cnt, sizeof(counter), NULL);
        printf("\nLight Status %d\n",status);
        if(status >0 && (temp_hb_light !=hb_light_cnt))
        {
            printf("\nHeartbeat Received from the Light Sensor Task\n");
        }
        else
        { 
            printf("\nUnable to fetch Heartbeat\n");
        }
    }

    if(req_cnt == TEMP_HB_CHECK_CNT)
    {   
        status = mq_receive(hb_temp_queue,(char*)&hb_temp_cnt, sizeof(counter), NULL);
        printf("\nLight Status %d\n",status);
        if(status >0 && (temp_hb_temp !=hb_temp_cnt))
        {
            printf("\nHeartbeat Received from the Temp Sensor Task\n");
        }
        else
        { 
            printf("\nUnable to fetch Heartbeat\n");
        }
        req_cnt = 0;
    }

    usleep(usecs_total);
    req_cnt +=1;
    return 1;
}
enum Status api_read_temp_register(uint16_t *readval)
{
    logpacket request_pck;
    logpacket req_rcv_pckt;
    uint8_t read_val;
    uint8_t status;

    request_pck.req_type = REQ_TEMPREG_READ;
    status=mq_send(temp_req_queue, (const logpacket*)&request_pck, sizeof(request_pck),1);
    printf("\nStatus Value of temp req queue %d\n",status);
    printf("\n Sending Request Type %d\n",request_pck.req_type);
    if(status == -1)
    {
        printf("\nMain was unable to send request message\n");
    }
    request_flag_temp =1;
    while(!req_processed);
    printf("\n  Request Processed by Temp Task\n");
    status = mq_receive(temp_req_queue,(logpacket *)&req_rcv_pckt, sizeof(logpacket), NULL);
    if(status >0)
    {   
        printf("\n Receivig Request Type %d\n",req_rcv_pckt.req_type);
        printf("\nRecevied Value from Temp Sensor %s\n",req_rcv_pckt.logmsg);
        read_val = (uint16_t)atoi(req_rcv_pckt.logmsg);
    }
    else
    {
        printf("\nUnable to Receive Request from source\n");
    }
    readval = &read_val;
    return SUCCESS;


}

enum Status api_read_temp_value(uint16_t *readval,request_t unit)
{
    logpacket request_pck;
    logpacket req_rcv_pckt;
    uint8_t read_val;
    uint8_t status;

    request_pck.req_type = unit;
    status=mq_send(temp_req_queue, (const logpacket*)&request_pck, sizeof(request_pck),1);
    printf("\nStatus Value of temp req queue %d\n",status);
    printf("\n Sending Request Type %d\n",request_pck.req_type);
    if(status == -1)
    {
        printf("\nMain was unable to send request message\n");
    }
    request_flag_temp =1;
    while(!req_processed);
    printf("\n  Request Processed by Temp Task\n");
    status = mq_receive(temp_req_queue,(logpacket *)&req_rcv_pckt, sizeof(logpacket), NULL);
    if(status >0)
    {   
        printf("\n Receivig Request Type %d\n",req_rcv_pckt.req_type);
        printf("\nRecevied Value from Temp Sensor %s\n",req_rcv_pckt.logmsg);
        read_val = (uint16_t)atoi(req_rcv_pckt.logmsg);
    }
    else
    {
        printf("\nUnable to Receive Request from source\n");
    }
    readval = &read_val;
    return SUCCESS;


}

enum Status api_read_temp_config_register(uint16_t *readval)
{
    logpacket request_pck;
    logpacket req_rcv_pckt;
    uint8_t read_val;
    uint8_t status;
    request_pck.req_type = REQ_TEMPREG_CONFIG_READ;
    status=mq_send(temp_req_queue, (const logpacket*)&request_pck, sizeof(request_pck),1);
    printf("\nStatus Value of temp req queue %d\n",status);
    printf("\n Sending Request Type %d\n",request_pck.req_type);
    if(status == -1)
    {
        printf("\nMain was unable to send request message\n");
    }
    request_flag_temp =1;
    while(!req_processed);
    printf("\n  Request Processed by Temp Task\n");
    status = mq_receive(temp_req_queue,(logpacket *)&req_rcv_pckt, sizeof(logpacket), NULL);
    if(status >0)
    {   
        printf("\n Receivig Request Type %d\n",req_rcv_pckt.req_type);
        printf("\nRecevied Value from Temp Sensor %s\n",req_rcv_pckt.logmsg);
        read_val = (uint16_t)atoi(req_rcv_pckt.logmsg);
    }
    else
    {
        printf("\nUnable to Receive Request from source\n");
    }
    readval = &read_val;
    return SUCCESS;


}

enum Status api_read_temp_tlow_register(uint16_t *readval)
{
    logpacket request_pck;
    logpacket req_rcv_pckt;
    uint16_t read_val;
    uint8_t status;
    request_pck.req_type = REQ_TEMPREG_DATA_LOW_READ;
    status=mq_send(temp_req_queue, (const logpacket*)&request_pck, sizeof(request_pck),1);
    printf("\nStatus Value of temp req queue %d\n",status);
    printf("\n Sending Request Type %d\n",request_pck.req_type);
    if(status == -1)
    {
        printf("\nMain was unable to send request message\n");
    }
    request_flag_temp =1;
    while(!req_processed);
    printf("\n  Request Processed by Temp Task\n");
    status = mq_receive(temp_req_queue,(logpacket *)&req_rcv_pckt, sizeof(logpacket), NULL);
    if(status >0)
    {   
        printf("\n Receivig Request Type %d\n",req_rcv_pckt.req_type);
        printf("\nRecevied Value from Temp Sensor %s\n",req_rcv_pckt.logmsg);
        read_val = (uint16_t)atoi(req_rcv_pckt.logmsg);
    }
    else
    {
        printf("\nUnable to Receive Request from source\n");
    }
    
    readval = &read_val;
    return SUCCESS;


}

enum Status api_read_temp_thigh_register(uint16_t *readval)
{
    logpacket request_pck;
    logpacket req_rcv_pckt;
    uint16_t read_val;
    uint8_t status;
    request_pck.req_type = REQ_TEMPREG_DATA_HIGH_READ;
    status=mq_send(temp_req_queue, (const logpacket*)&request_pck, sizeof(request_pck),1);
    printf("\nStatus Value of temp req queue %d\n",status);
    printf("\n Sending Request Type %d\n",request_pck.req_type);
    if(status == -1)
    {
        printf("\nMain was unable to send request message\n");
    }
    request_flag_temp =1;
    while(!req_processed);
    printf("\n  Request Processed by Temp Task\n");
    status = mq_receive(temp_req_queue,(logpacket *)&req_rcv_pckt, sizeof(logpacket), NULL);
    if(status >0)
    {   
        printf("\n Receivig Request Type %d\n",req_rcv_pckt.req_type);
        printf("\nRecevied Value from Temp Sensor %s\n",req_rcv_pckt.logmsg);
        read_val = (uint16_t)atoi(req_rcv_pckt.logmsg);
    }
    else
    {
        printf("\nUnable to Receive Request from source\n");
    }
    readval = &read_val;
    return SUCCESS;


}

enum Status api_write_temp_ptr_register(uint16_t writeval)
{
	logpacket request_pck;
    logpacket req_rcv_pckt;
    uint8_t status;
    request_pck.req_type = REQ_TEMPREG_PTRREG_WRITE;
    char *temp_buff = (char*)malloc(sizeof(uint16_t));
    sprintf(temp_buff,"%4x",writeval);
    strcpy(request_pck.logmsg,temp_buff);
    status=mq_send(temp_req_queue, (const logpacket*)&request_pck, sizeof(request_pck),1);
    printf("\nStatus Value of temp req queue %d\n",status);
    printf("\n Sending Request Type %d\n",request_pck.req_type);
    if(status == -1)
    {
        printf("\nMain was unable to send request message\n");
    }
    request_flag_temp =1;
    while(!req_processed);
    if(write_complete)
    {
    	write_complete = 0; 
    	return SUCCESS;
    }
    else
    {
    	return FAIL;
    }

}
enum Status api_write_temp_config_register(uint16_t writeval)
{
	logpacket request_pck;
    logpacket req_rcv_pckt;
    uint8_t status;
    request_pck.req_type = REQ_TEMPREG_CONFIG_WRITE;
    char *temp_buff = (char*)malloc(sizeof(uint16_t));
    sprintf(temp_buff,"%4x",writeval);
    strcpy(request_pck.logmsg,temp_buff);
    status=mq_send(temp_req_queue, (const logpacket*)&request_pck, sizeof(request_pck),1);
    printf("\nStatus Value of temp req queue %d\n",status);
    printf("\n Sending Request Type %d\n",request_pck.req_type);
    if(status == -1)
    {
        printf("\nMain was unable to send request message\n");
    }
    request_flag_temp =1;
    while(!req_processed);
    if(write_complete)
    {
    	write_complete = 0; 
    	return SUCCESS;
    }
    else
    {
    	return FAIL;
    }

}

enum Status api_write_temp_tlow_register(uint16_t writeval)
{
	logpacket request_pck;
    logpacket req_rcv_pckt;
    uint8_t status;
    request_pck.req_type = REQ_TEMPREG_DATA_LOW_WRITE;
    char *temp_buff = (char*)malloc(sizeof(uint16_t));
    sprintf(temp_buff,"%4x",writeval);
    strcpy(request_pck.logmsg,temp_buff);
    status=mq_send(temp_req_queue, (const logpacket*)&request_pck, sizeof(request_pck),1);
    printf("\nStatus Value of temp req queue %d\n",status);
    printf("\n Sending Request Type %d\n",request_pck.req_type);
    if(status == -1)
    {
        printf("\nMain was unable to send request message\n");
    }
    request_flag_temp =1;
    while(!req_processed);
    if(write_complete)
    {
    	write_complete = 0; 
    	return SUCCESS;
    }
    else
    {
    	return FAIL;
    }

	
}

enum Status api_write_temp_thigh_register(uint16_t writeval)
{
	logpacket request_pck;
    uint8_t status;
    request_pck.req_type = REQ_TEMPREG_DATA_HIGH_WRITE;
    char *temp_buff = (char*)malloc(sizeof(uint16_t));
    sprintf(temp_buff,"%4x",writeval);
    strcpy(request_pck.logmsg,temp_buff);
    status=mq_send(temp_req_queue, (const logpacket*)&request_pck, sizeof(request_pck),1);
    printf("\nStatus Value of temp req queue %d\n",status);
    printf("\n Sending Request Type %d\n",request_pck.req_type);
    if(status == -1)
    {
        printf("\nMain was unable to send request message\n");
    }
    request_flag_temp =1;
    while(!req_processed);
    if(write_complete)
    {
    	write_complete = 0; 
    	return SUCCESS;
    }
    else
    {
    	return FAIL;
    }
	
}



enum Status api_write_tempreg(request_t reg_request,uint16_t writeval)
{
	enum Status state;
	switch(reg_request)
	{
        case REQ_TEMPREG_PTRREG_WRITE:
            state = api_write_temp_ptr_register(writeval);
            break;
        case REQ_TEMPREG_CONFIG_WRITE: 
            state = api_write_temp_config_register(writeval);
            break;
        case REQ_TEMPREG_DATA_LOW_WRITE: 
            state = api_write_temp_tlow_register(writeval);
            break;
        case REQ_TEMPREG_DATA_HIGH_WRITE: 
            state = api_write_temp_thigh_register(writeval);
            break;
        default:
        	state = FAIL;
        	break;      		
	}
	return state;

}
enum Status api_read_tempreg(request_t reg_request,uint16_t *readval)
{
	enum Status state;
    switch(reg_request)
    {
        case REQ_TEMPREG_READ:
            state = api_read_temp_register(readval);
            break;
        case REQ_TEMPREG_CONFIG_READ: 
            state = api_read_temp_config_register(readval);
            break;
        case REQ_TEMPREG_DATA_LOW_READ: 
            state = api_read_temp_tlow_register(readval);
            break;
        case REQ_TEMPREG_DATA_HIGH_READ: 
            state = api_read_temp_thigh_register(readval);
            break;
        default:
        	state = FAIL;
        	break;                                   
    }
    return state;

}
enum Status api_temp_setconv(request_t conv_rate)
{
	logpacket request_pck;
	uint8_t status;
	request_pck.req_type = conv_rate;
	status=mq_send(temp_req_queue, (const logpacket*)&request_pck, sizeof(request_pck),1);
	if(status == -1)
	{
        printf("\nMain was unable to send request message\n");
    }
    while(!req_processed);
    if(conv_rate_set)
    {
    	conv_rate_set = 0;
    	return SUCCESS;
    }
    else
    {
    	return FAIL;
    }

}
enum Status api_temp_rqt_shutdown(uint8_t option)
{
	if(option == 1)
	{
		logpacket request_pck;
	    uint8_t status;
	   // api_write_tempreg(REQ_TEMPREG_CONFIG_WRITE,0x61a0);
	    request_pck.req_type = REQ_SHUTDOWN_ENABLE;
	    status=mq_send(temp_req_queue, (const logpacket*)&request_pck, sizeof(request_pck),1);
	    printf("\nStatus Value of temp req queue %d\n",status);
	    printf("\n Sending Request Type %d\n",request_pck.req_type);
	    if(status == -1)
	    {
	        printf("\nMain was unable to send request message\n");
	    }
	    request_flag_temp =1;
	    while(!req_processed);
	    if(shutdown_enable_complete)
	    {
	    	shutdown_enable_complete = 0; 
	    	return SUCCESS;
	    }
	    else
	    {
	    	return FAIL;
	    }

	}
	else if(option == 0){
		logpacket request_pck;
	    uint8_t status;
	    request_pck.req_type = REQ_SHUTDOWN_DISABLE;
	    status=mq_send(temp_req_queue, (const logpacket*)&request_pck, sizeof(request_pck),1);
	    printf("\nStatus Value of temp req queue %d\n",status);
	    printf("\n Sending Request Type %d\n",request_pck.req_type);
	    if(status == -1)
	    {
	        printf("\nMain was unable to send request message\n");
	    }
	    request_flag_temp =1;
	    while(!req_processed);
	    if(shutdown_disable_complete)
	    {
	    	shutdown_disable_complete = 0; 
	    	return SUCCESS;
	    }
	    else
	    {
	    	return FAIL;
	    }

	}
}

int main(int argc, char **argv)
{
    
    if (argc < 2)
    {
      printf("USAGE:  <log filename>\n");
      exit(1);
    }
    char *logfname = malloc(50 *sizeof(char));
    if(!logfname)
    {
    	printf("\nERR:Malloc Failed!\n");
		return 1;

    }
    if(!memcpy(logfname,argv[1],strlen(argv[1])))
  	{
  		printf("\nERR:Memcpy Failed!\n");
		return 1;
  	}
    
    printf("%ld",sizeof(argv[1]));
    printf("%s",logfname);

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    int temp_counter = 659;
    int status;
  
    uint32_t usecs_send;
    usecs_send = 4000000;
    uint32_t usecs_total = 4000000;
    uint8_t temp_hb_light = hb_light_cnt;


    signal(SIGINT, exit_handler);
    counter = 0;
    mq_attr_queue.mq_maxmsg = 10;
    mq_attr_queue.mq_msgsize = sizeof(counter);

    mq_attr_log_queue.mq_maxmsg = 10;
    mq_attr_log_queue.mq_msgsize = sizeof(logpacket);
    
    mq_attr_temp_req_queue.mq_maxmsg = 10;
    mq_attr_temp_req_queue.mq_msgsize = sizeof(logpacket);

    mq_unlink(MSG_QUEUE);
    msg_queue = mq_open(MSG_QUEUE,O_CREAT|O_RDWR|O_NONBLOCK, 0666, &mq_attr_queue);

    // To improve efficiency of space. Stop using counter as the size of each message
    mq_unlink(HB_TEMP_QUEUE);
    hb_temp_queue = mq_open(HB_TEMP_QUEUE,O_CREAT|O_RDWR|O_NONBLOCK, 0666, &mq_attr_queue);
    mq_unlink(HB_LIGHT_QUEUE);
    hb_light_queue = mq_open(HB_LIGHT_QUEUE,O_CREAT|O_RDWR|O_NONBLOCK, 0666, &mq_attr_queue);
    mq_unlink(HB_LOG_QUEUE);
    hb_log_queue = mq_open(HB_LOG_QUEUE,O_CREAT|O_RDWR|O_NONBLOCK, 0666, &mq_attr_log_queue);
    mq_unlink(HB_TEMP_REQ_QUEUE);
    temp_req_queue = mq_open(HB_TEMP_REQ_QUEUE,O_CREAT|O_RDWR|O_NONBLOCK, 0666, &mq_attr_temp_req_queue);

    if(msg_queue == -1)
    {
        printf("\nUnable to open message queue! Exiting\n");
        exit(0);
    }

    if(hb_temp_queue == -1)
    {
        printf("\nUnable to open message queue! Exiting\n");
        exit(0);
    }

    if(hb_light_queue == -1)
    {
        printf("\nUnable to open message queue! Exiting\n");
        exit(0);
    }

    if(temp_req_queue == -1)
    {
        printf("\nUnable to open message queue! Exiting\n");
        exit(0);
    }

    if(pthread_cond_init(&sig_logger,NULL))
    {
    	printf("\nERR: Failure to init condition\n");
    	
    }
    
    if(pthread_mutex_init(&logger_mutex,NULL))
    {
    	printf("\nERR: Failure to init condition\n");
    	
    }

    if(pthread_cond_init(&sig_req_process,NULL))
    {
        printf("\nERR: Failure to init condition\n");
        
    }
    

    if(pthread_mutex_init(&proceed_init_mutex,NULL))
    {
        printf("\nERR: Failure to init condition\n");
        
    }

    if(pthread_cond_init(&proceed_init,NULL))
    {
        printf("\nERR: Failure to init condition\n");
        
    }


    if(pthread_mutex_init(&req_process_mutex,NULL))
    {
        printf("\nERR: Failure to init condition\n");
        
    } 

    if(pthread_mutex_init(&i2c_init_mutex,NULL))
    {
        printf("\nERR: Failure to init condition\n");
        
    } 

    if(pthread_mutex_init(&i2c_rw_mutex,NULL))
    {
        printf("\nERR: Failure to init condition\n");
        
    }     
    if(hb_log_queue == -1)
    {
        printf("\nUnable to open message queue! Exiting\n");
        exit(0);
    } 
    //To modify attributes to improve scheduling efficiency and task synchronization

    /* Creating Temp Sensor Thread */
    if(pthread_create(&tempsensor_thread, &attr, (void*)&app_tempsensor_task, NULL))
    {
    	printf("\nERR: Failure to create thread\n");
    	//Log Error 
    }

    /* Creating Light Sensor Thread */
    if(pthread_create(&lightsensor_thread, &attr, (void*)&app_lightsensor_task, NULL))
    {
    	printf("\nERR: Failure to create thread\n");
    	//Log Error 
    }
    
    /* Creating Synchronization Logger Thread */
    if(pthread_create(&synclogger_thread, &attr, (void*)&app_sync_logger, logfname))
    {
    	printf("\nERR: Failure to create thread\n");
    	//Log Error 
    }
    
    status=mq_send(msg_queue, (const char*)&temp_counter, sizeof(temp_counter),1);
    if(status == -1)
    {
        printf("\nMain was unable to send message\n");
    }
    logpacket request_pck;
    logpacket req_rcv_pckt;
    while(1)
    {
        uint8_t api_count=0;
        if(!monitor_hb_notif())
        {
            break;
        }
        //Testing Querry APIs of Temperature Task
        api_count++;
        uint16_t readval;
        uint16_t writeval = 0x68a0;
        if (api_count == 1 && !req_processed)
        {
            api_count++;
            printf("\n API COUNT %d",api_count);
        	//api_temp_rqt_shutdown(0);
        }

    }
    //Synchronization with the main thread
    pthread_join(tempsensor_thread, NULL);
 	pthread_join(lightsensor_thread, NULL);
	pthread_join(synclogger_thread, NULL);
    exit_handler(SIGINT);
	return 0;

}
