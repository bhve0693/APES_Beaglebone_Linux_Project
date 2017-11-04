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


#define MSG_QUEUE "/msg_queue"
#define HB_TEMP_QUEUE "/hb_temp_queue"
#define HB_LIGHT_QUEUE "/hb_light_queue"

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

static volatile sig_atomic_t light_flag = 0;
static volatile sig_atomic_t temp_flag = 0;
static volatile sig_atomic_t req_cnt = 0;

pthread_cond_t sig_logger;
pthread_mutex_t logger_mutex;
pthread_mutex_t msg_temp_mutex;
pthread_mutex_t msg_light_mutex;

uint8_t hb_temp_cnt;
uint8_t hb_light_cnt;
uint8_t hb_synclog_cnt;
static uint8_t temp_hb_light;
static mqd_t msg_queue;
static mqd_t hb_temp_queue;
static mqd_t hb_light_queue;
static mqd_t hb_log_queue;

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
    exit(0);
}


void *app_tempsensor_task(void *args) // Temperature Sensor Thread/Task
{
    uint32_t usecs;
    uint8_t status;
    uint8_t temp_count;
    usecs = 1000000;
    printf("\nIn Temperature Sensor Thread execution\n");
    
    //Creating Log message in logpacket
    //float temp_value = temp_read();
    float temp_value = 32.02;
    char *temp_buff = (char*)malloc(sizeof(float));
    if(!temp_buff)
    {
    	printf("\nERR:Malloc Error");
    }
    sprintf(temp_buff,"%f",temp_value);
    //msg_tempsensor.msg_size = strlen(temp_buff);
    msg_tempsensor.logmsg = NULL;
    msg_tempsensor.logmsg = (uint8_t*)temp_buff;
    msg_tempsensor.sourceid = SRC_TEMPERATURE;
    if(msg_tempsensor.logmsg != NULL)
    {
        status=mq_send(hb_log_queue, (const logpacket*)&msg_tempsensor, sizeof(msg_tempsensor),1);
        if(status == -1)
        {
            printf("\ntemp was unable to send log message\n");
        }
        pthread_cond_signal(&sig_logger);
    }

    while(1)
    {
        status=mq_send(msg_queue, (const char*)&counter, sizeof(counter),1);
        if(status == -1)
        {
            exit_handler(SIGINT);
        }
        usleep(usecs);
        if(temp_flag)
        {
            status = mq_receive(hb_temp_queue,(char*)&temp_count, sizeof(counter), NULL);
            if(status >0)
            {
                printf("\nReceive Heartbeat temp request: %d\n", temp_count);
                hb_temp_cnt+=1;
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

void *app_lightsensor_task(void *args) //Light Sensor Thread/Task
{
	uint32_t usecs;
    int status;
    int recvcounter;
    uint8_t temp_count;
    usecs = 10000;
    printf("\nIn Light Sensor Thread execution\n");
    printf("\nRecvcounter is %d\n",recvcounter);
    while(1)
    {
        
        status = mq_receive(msg_queue,(char*)&recvcounter, sizeof(recvcounter), NULL);
        if(status >0)
        {
            printf("\nRecevied Message in Lightsensor Thread : %d\n",recvcounter);
            counter+=1;
        }
        usleep(usecs);
        if(light_flag)
        {
            status = mq_receive(hb_light_queue,(char*)&temp_count, sizeof(counter), NULL);
            if(status >0)
            {
                printf("\nReceive Heartbeat request: %d\n", temp_count);
                hb_light_cnt+=1;
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
	FILE *fp;
	//fp = fopen(fname,w+);
	if(!(fp= fopen(fname,"w+")))
	{
		printf("\nERR:Invalid Log Filename fp!\n");
		pthread_exit(NULL);
	} 

	//Wait on temperature Thread to log
	pthread_cond_wait(&sig_logger, &logger_mutex);

    int status =0;
    logpacket temp; 
    temp.logmsg = NULL;
    status = mq_receive(hb_log_queue,(logpacket*)&temp, sizeof(temp), NULL);
        printf("\nLog Status %d\n",status);
        if(temp.logmsg !=NULL)
        {
            if(status >0)
            {
                printf("\nLog Queue message received\n");
                printf("\n%d\n",strlen((char*)temp.logmsg));
                if(temp.sourceid == SRC_LIGHT)
                {
                    sprintf(logbuff,"%s","\n[1sec,44usec] LUX :");
                }
                else if(temp.sourceid == SRC_TEMPERATURE)
                {
                    sprintf(logbuff,"%s","\n[1sec,44usec] TEMP :");
                }
                strncat(logbuff, (char *)temp.logmsg, strlen((char*)temp.logmsg));
                fwrite(logbuff,1, strlen(logbuff)*sizeof(char),fp);
                //fwrite((uint8_t*)temp.logmsg,1, strlen((uint8_t*)temp.logmsg)*sizeof(uint8_t),fp);
            }    
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
    //strncpy(logfname,argv[1],sizeof(argv[1]));
    printf("%s",logfname);
   // exit(0);

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


    msg_queue = mq_open(MSG_QUEUE,O_CREAT|O_RDWR|O_NONBLOCK, 0666, &mq_attr_queue);

    // To improve efficiency of space. Stop using counter as the size of each message
    hb_temp_queue = mq_open(HB_TEMP_QUEUE,O_CREAT|O_RDWR|O_NONBLOCK, 0666, &mq_attr_queue);
    hb_light_queue = mq_open(HB_LIGHT_QUEUE,O_CREAT|O_RDWR|O_NONBLOCK, 0666, &mq_attr_queue);
    hb_log_queue = mq_open(HB_LOG_QUEUE,O_CREAT|O_RDWR|O_NONBLOCK, 0666, &mq_attr_log_queue);

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

    if(pthread_cond_init(&sig_logger,NULL))
    {
    	printf("\nERR: Failure to init condition\n");
    	
    }
    
    if(pthread_mutex_init(&logger_mutex,NULL))
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

    while(1)
    {
        if(!monitor_hb_notif())
        {
            break;
        }

    }
    //Synchronization with the main thread
    //pthread_join(tempsensor_thread, NULL);
 	//pthread_join(lightsensor_thread, NULL);
	pthread_join(synclogger_thread, NULL);
    exit_handler(SIGINT);
	return 0;

}
