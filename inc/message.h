/*
* FileName        :    message.h
* Description     :    This file contains the necessary SW support for message queue, logpackets and link layer functions
*                        
* File Author Name:    Divya Sampath Kumar, Bhallaji Venkatesan
* Tools used      :    gcc, gedit, Sublime Text
* References      :    
*
*
*/


#ifndef _MESSAGE_H
#define _MESSAGE_H

#include <time.h>
#include <mqueue.h>

//Level of logs for criticality demarcation in the logfile
typedef enum loglevel_t{
LOG_DATA_VALID,     
LOG_DATA_REQ,
LOG_DATA_CONV,
LOG_CRITICAL,
LOG_INIT,
LOG_ERR
}loglevel;

//Source Task 
typedef enum srcid_t{
SRC_MAIN,
SRC_TEMPERATURE,
SRC_LIGHT,
SRC_LOG,
SRC_DEFAULT
}srcid;

//Request Task 

typedef enum reqtype_t{
DEFAULT,
REQ_TEMP_CELSIUS,
REQ_TEMP_KELVIN,
REQ_TEMP_FAHRENHEIT,
REQ_LUX,
REQ_REG,
REQ_TEMPREG_READ,
REQ_TEMP_CONV_RATE_0_25_HZ,
REQ_TEMP_CONV_RATE_1_HZ,
REQ_TEMP_CONV_RATE_4_HZ,
REQ_TEMP_CONV_RATE_8_HZ,
REQ_TEMPREG_PTRREG_WRITE,
REQ_TEMPREG_DATA_LOW_READ,
REQ_TEMPREG_DATA_HIGH_READ,
REQ_TEMPREG_DATA_LOW_WRITE,
REQ_TEMPREG_DATA_HIGH_WRITE,
REQ_SHUTDOWN_ENABLE,
REQ_TEMPREG_CONFIG_READ,
REQ_TEMPREG_CONFIG_WRITE,
REQ_SHUTDOWN_DISABLE,
REQ_POWERON,
REQ_POWEROFF,
REQ_SET_INTEGRATION_TIME,
REQ_INTERRUPT_ENABLE,
REQ_INTERRUPT_DISABLE,
REQ_IDREG_READ,
REQ_LIGHT_TLOW_READ,
REQ_LIGHT_THIGH_READ,
REQ_LIGHT_CRTL_READ,
REQ_LIGHT_ADC0_READ,
REQ_LIGHT_ADC1_READ,
REQ_LIGHT_INTRREG_READ,
REQ_LIGHT_TIMINGREG_READ,
REQ_LIGHT_TLOW_WRITE,
REQ_LIGHT_THIGH_WRITE,
REQ_LIGHT_CRTL_WRITE,
REQ_LIGHT_INTRREG_WRITE,
REQ_LIGHT_TIMINGREG_WRITE
}request_t;

//Log Packet
typedef struct log_packet_t{
struct timeval time_stamp;  //Timestamps use time.h
loglevel level;        //Log Levels based on criticality of logs
srcid sourceid;         //Source of Logs 
int32_t crc;           //CRC check for error checking mechanism 
request_t req_type;  
char logmsg[256];
}logpacket;


/* Link Layer Functions */
int8_t link_req_hb_notif(mqd_t msgdt); // Requests Notifications from all the tasks in order of 
									  // 1.app_tempsensor_task
					                  // 2.app_lightsensor_task
					                  // 3.app_sync_logger
					 
int8_t link_send_hb_notif(mqd_t msgdt); 

int8_t link_send_log_init_status(mqd_t msgdt,struct logpacket pack);

int8_t link_send_log_data_request(mqd_t msgdt, struct logpacket pack);

int8_t link_send_log_err(mqd_t msgdt, struct logpacket pack);

int8_t link_read_data(mqd_t msgdt,void* data, srcid source, srcid dest); // Will be called by query functions. Will share the message queues with the sensor task.

int8_t link_send_logs(struct logpacket pack);

#endif