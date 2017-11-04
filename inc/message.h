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

//Log Packet
typedef struct log_packet_t{
struct timeval time_stamp;  //Timestamps use time.h
loglevel level;        //Log Levels based on criticality of logs
srcid sourceid;         //Source of Logs 
int32_t crc;           //CRC check for error checking mechanism   
//size_t msg_size;
uint8_t *logmsg;          //Message to be logged: void * due to flexibile data type requirment
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