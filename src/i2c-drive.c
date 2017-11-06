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
#include "fw_i2c.h"
#include "i2c_light.h"
//#include "fw_i2c.h"


//To check which i2c adapter needs to be accessed, run i2cdetect -l
enum Status i2c_temp_init(uint8_t fd,uint8_t addr)
{

	if(addr == 0x48 || addr == 0x49 || addr == 0x4A || addr==0x4B)
	{
		if(ioctl(fd,I2C_SLAVE,addr)<0)
		{
			return FAIL;
		}
		return SUCCESS;
	}
	else return FAIL;
}

enum Status i2c_light_init(uint8_t fd,uint8_t addr)
{
	if(addr == 0x29 || addr == 0x39 || addr == 0x49)
	{
		if(ioctl(fd,I2C_SLAVE,addr)<0)
		{
			stat =  FAIL;
		}
		stat = SUCCESS;
		stat = write_light_registers(fd,T_LOW,0x0000);
		if (stat)
		{
			printf("\nERR:Could not write value into T_LOW Register\n");
		}
		stat = write_light_registers(fd,T_HIGH,0xFFFF);
		if(stat)
		{
			printf("\nERR:Could not write value into T_HIGH Register\n");
		}
	}
	else stat = FAIL;
	return stat;
}

enum Status i2c_write(uint8_t fd,uint8_t *val)
{
	if(write(fd,val,1)!=1)
	{
		return FAIL;
	}
	return SUCCESS;
}

enum Status i2c_write_word(uint8_t fd,uint8_t *val)
{
	if(write(fd,val,3)!=3)
	{
		return FAIL;
	}
	return SUCCESS;
}

enum Status i2c_write_light(uint8_t fd,uint8_t *val)
{
	if(write(fd,val,2)!=2)
	{
		return FAIL;
	}
	return SUCCESS;
}

enum Status i2c_read_word(uint8_t fd,uint8_t *val)
{
	if(read(fd,val,2)!=2)
	{
		return FAIL;
	}
	return SUCCESS;
}

enum Status i2c_read(uint8_t fd,uint8_t *val)
{
	if(read(fd,val,1)!=1)
	{
		return FAIL;
	}
	return SUCCESS;
}


