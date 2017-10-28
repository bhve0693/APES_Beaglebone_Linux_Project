/*
* FileName        :    main.c
* Description     :    This file contains concurrent SW implementation of a Beaglebone Linux
*					   system handling multiple sensor interfacing and data logging in a multi-threaded
*					   synchronized way.
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
#include "message.h"


#define MSG_QUEUE "/msg_queue"

pthread_t tempsensor_thread;
pthread_t lightsensor_thread;
pthread_t synclogger_thread;
struct mq_attr mq_attr_queue;
static mqd_t mqd;

logpacket msg_tempsensor,msg_lightsensor, msg_synclogger;

void *app_tempsensor_task(void *args) // Temperature Sensor Thread/Task
{
	//Do Sensor data acquisition
	//Send heartbeat notif when requested
	//Log essentials/ errors
	//Log data into log queues 

}

void *app_lightsensor_task(void *args) //Light Sensor Thread/Task
{
	//Do Sensor data acquisition
	//Send heartbeat notif when requested
	//Log essentials/ errors
	//Log data into log queues 

}

void *app_sync_logger(void *args) // Synchronization Logger Thread/Task
{
	//Handle Log Requests
	//Send heartbeat notif when requested
	//Log essentials/ errors
	//Perform Logging into custom log file

}

int main(int argc, char **argv)
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);
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
    if(pthread_create(&synclogger_thread, &attr, (void*)&app_lightsensor_task, NULL))
    {
    	printf("\nERR: Failure to create thread\n");
    	//Log Error 
    }

    //Synchronization with the main thread
    pthread_join(tempsensor_thread, NULL);
 	pthread_join(lightsensor_thread, NULL);
	pthread_join(synclogger_thread, NULL);

	return 0;

}
