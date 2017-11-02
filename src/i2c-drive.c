/*Refer https://www.kernel.org/doc/Documentation/i2c/dev-interface*/
/*https://elinux.org/Interfacing_with_I2C_Devices*/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include "../inc/fw_i2c.h"

int fd;
//To check which i2c adapter needs to be accessed, run i2cdetect -l
void i2c_init(uint8_t addr,char *file)
{
	fd = open(file,O_RDWR);
	if(fd<0)
	{
		//check error number and print appropriate message
		printf("Error opening file:%s\n",strerror(errno));
		exit(1);
	}
	//Once device is opened. we must specify with what device address we want to communicate
	if(ioctl(fd,I2C_SLAVE,addr)<0)
	{
		printf("ioctl error:%s\n",strerror(errno));
		exit(1);
	}
}

enum Status i2c_write(uint8_t *val)
{
	if(write(fd,&val,1)!=1)
	{
		printf("Error writing values into file:%s",strerror(errno));
		return FAIL;
	}
	close(fd);
	return SUCCESS;
}

enum Status i2c_read(uint8_t *val)
{
	if(read(fd,val,1)!=1)
	{
		printf("Error reading value from file:%s",strerror(errno));
		close(fd);
		return FAIL;
	}
	close(fd);
	return SUCCESS;
}



