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
//#include "../inc/fw_i2c.h"
#include "fw_i2c.h"


//To check which i2c adapter needs to be accessed, run i2cdetect -l
enum Status i2c_temp_init(uint8_t fd,uint8_t addr)
{
	if(ioctl(fd,I2C_SLAVE,addr)<0)
	{
		return FAIL;
	}
	return SUCCESS;
}

enum Status i2c_write(uint8_t fd,uint8_t *val)
{
	if(write(fd,val,1)!=1)
	{
		return FAIL;
	}
	return SUCCESS;
}

enum Status i2c_read(uint8_t fd,uint8_t *val)
{
	if(read(fd,val,2)!=2)
	{
		return FAIL;
	}
	return SUCCESS;
}



