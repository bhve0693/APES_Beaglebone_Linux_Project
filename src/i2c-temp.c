#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include <signal.h>
#include "fw_i2c.h"
#include "i2c_temp.h"



#define PTR_REG 0x00
#define TEMP_REG 0x00
#define CONFIG_REG 0x01
#define TLOW_REG 0x02
#define THIGH_REG 0x03
#define READ 0x01
#define WRITE 0x00
#define SHUTDOWN_ENABLE 0x0100
#define ENABLE_25 0x00
#define ENABLE_1 0x40
#define ENABLE_4 0x80
#define ENABLE_8 0xC0
#define DEV_ADDR 0x48


enum Status read_temp_reg(uint8_t fd,uint8_t reg,uint16_t *val)
{
	enum Status stat;
	uint8_t *buffer = (uint8_t*)malloc(sizeof(uint8_t)*2);
	stat = i2c_write(fd,&reg);
	if(!stat)
	{
		stat = i2c_read_word(fd,buffer);
		*val = (uint16_t)(buffer[0]<<8) | (uint16_t)(buffer[1]);
	}
	free(buffer);
	return stat;

}

enum Status read_config_reg(uint8_t fd,uint8_t reg,uint16_t *val)
{
	enum Status stat;
	uint8_t *buffer = (uint8_t*)malloc(2*sizeof(uint8_t));
	stat = i2c_write(fd,&reg);
	if(!stat)
	{
		stat = i2c_read_word(fd,buffer);
		*val = (uint16_t)(buffer[0]<<8) | (uint16_t)(buffer[1]);
	}
	free(buffer);
	
	return stat;
	
}

enum Status read_tlow_reg(uint8_t fd,uint8_t reg,uint16_t *val)
{
	enum Status stat;
	uint8_t *buffer = (uint8_t*)malloc(sizeof(uint8_t)*2);
	stat = i2c_write(fd,&reg);
	if(!stat)
	{
		stat = i2c_read_word(fd,buffer);
		*val = (uint16_t)(buffer[0]<<8) | (uint16_t)(buffer[1]);
	}
	return stat;
	free(buffer);
}


enum Status read_thigh_reg(uint8_t fd,uint8_t reg,uint16_t *val)
{
	enum Status stat;
	uint8_t *buffer = (uint8_t*)malloc(sizeof(uint8_t)*2);
	stat = i2c_write(fd,&reg);
	if(!stat)
	{
		stat = i2c_read_word(fd,buffer);
		*val = (uint16_t)(buffer[0]<<8) | (uint16_t)(buffer[1]);
	}
	return stat;
	free(buffer);
}


enum Status write_ptr_reg(uint8_t fd,uint8_t reg)
{
	enum Status stat;
	stat = i2c_write(fd,&reg);
	return stat;
}

enum Status write_config_reg(uint8_t fd,uint8_t reg,uint16_t value)
{
	enum Status stat;
	uint8_t *buffer = (uint8_t*)malloc(3*sizeof(uint8_t));
	buffer[0] = reg;
	buffer[1] = (uint8_t)((value & 0xFF00)>>8);
	buffer[2] = (uint8_t)(value & 0x00FF);
	stat = i2c_write_word(fd,buffer);
	free(buffer);
	return stat;
}

enum Status write_tlow_reg(uint8_t fd,uint8_t reg,uint16_t value)
{
	enum Status stat;
	uint8_t *buffer = (uint8_t*)malloc(3*sizeof(uint8_t));
	buffer[0] = reg;
	buffer[1] = (uint8_t)((value & 0xFF00)>>8);
	buffer[2] = (uint8_t)(value & 0x00FF);
	stat = i2c_write_word(fd,buffer);
	free(buffer);
	return stat;
	
}

enum Status write_thigh_reg(uint8_t fd,uint8_t reg,uint16_t value)
{
	enum Status stat;
	uint8_t *buffer = (uint8_t*)malloc(3*sizeof(uint8_t));
	buffer[0] = reg;
	buffer[1] = (uint8_t)((value & 0xFF00)>>8);
	buffer[2] = (uint8_t)(value & 0x00FF);
	stat = i2c_write_word(fd,buffer);
	free(buffer);
	return stat;	
}


enum Status read_temp_register(uint8_t fd,request_t reg_option,uint16_t *val)
{
	enum Status stat;
	uint8_t value,addr;
	switch(reg_option)
	{
		case REQ_TEMPREG_READ: addr = PTR_REG|TEMP_REG;
				stat = read_temp_reg(fd,addr,val);
				break;

		case REQ_TEMPREG_CONFIG_READ: addr = PTR_REG|CONFIG_REG;
				stat = read_config_reg(fd,addr,val);
				break;

		case REQ_TEMPREG_DATA_LOW_READ: addr = PTR_REG|TLOW_REG;
				stat = read_tlow_reg(fd,addr,val);
				break;

		case REQ_TEMPREG_DATA_HIGH_READ: addr = PTR_REG|THIGH_REG;
				stat = read_thigh_reg(fd,addr,val);
				break;

		default:stat = FAIL;
				printf("\nERR:Incorrect register chosen to read\n");	
	}

	return stat;
}


enum Status continuous_conversion_mode(uint8_t fd,uint8_t option)
{
	uint16_t config_val;

	stat = read_temp_register(fd,2,&config_val);
	if(!option)
	{
		config_val &= ~ENABLE_4;
		config_val |= ENABLE_25;
		printf("\nConversion rate of 0.25Hz enabled:%4x\n",config_val);

	}
	else if(option==1) 
	{
		config_val &= ~ENABLE_4;
		config_val |= ENABLE_1;
		printf("\nConversion rate of 1Hz enabled:%4x\n",config_val);
	}
	else if(option==2) 
	{
		config_val &= ~ENABLE_4;
		config_val |= ENABLE_4;
		printf("\nConversion rate of 4Hz enabled:%4x\n",config_val);
	}
	else if(option==3) 
	{
		config_val &= ~ENABLE_4;
		config_val |= ENABLE_8;
		printf("\nConversion rate of 8Hz enabled:%4x\n",config_val);
	}
	else
	{
		printf("\nInvalid option....setting rate to 4Hz default:%4x\n",config_val);
	}
}

enum Status shutdown_temp_mode(uint8_t fd,uint8_t option)
{
	uint16_t config_val;
	stat = read_temp_register(fd,2,&config_val);
	if(option==1)
	{
		config_val |= SHUTDOWN_ENABLE;
		printf("\nShutdown enabled:%4x\n",config_val);

	}
	else 
	{
		config_val &= ~SHUTDOWN_ENABLE;
		printf("\nShutdown disabled:%4x\n",config_val);
	}
}


enum Status write_temp_register(uint8_t fd,request_t reg_option,uint16_t val)
{
	enum Status stat;
	switch(reg_option)
	{
		case REQ_TEMPREG_PTRREG_WRITE:	if(val>3)
				{	
					printf("ERR:Pointer register is 8 bits long with higher 6 bits set to 0\n");
					stat = FAIL;
				}
				else stat = write_ptr_reg(fd,val);
				break;

		case REQ_TEMPREG_CONFIG_WRITE:	stat = write_config_reg(fd,PTR_REG|CONFIG_REG,val);
				break;

		case REQ_TEMPREG_DATA_LOW_WRITE: stat = write_tlow_reg(fd,PTR_REG|TLOW_REG,val);
				break;
		
		case REQ_TEMPREG_DATA_HIGH_WRITE: stat = write_thigh_reg(fd,PTR_REG|THIGH_REG,val);
				break;
		
		default:stat = FAIL;
				printf("\nERR:Incorrect register chosen to write\n");	
	}
	return stat;
}



float temp_read(uint8_t fd,uint8_t unit_choice)
{
	uint8_t operation;
	uint8_t readval;
	enum Status stat;
	uint8_t *buf = (uint8_t*)malloc(sizeof(uint8_t)*2);

	stat = i2c_write(fd,buf);
	if(stat)
	{
		printf("\nFailed to select temperature register:%s\n",strerror(errno));
		exit(1);
	}

	float temp_celsius = read_tempsense(fd,0.0625,unit_choice);
	free(buf);
	return temp_celsius;
	
}

float celsius(float temp_celsius)
{
	return temp_celsius;
}

float fahrenheit(float temp_fahrenheit)
{
	return temp_fahrenheit;
}

float kelvin(float temp_kelvin)
{
	return temp_kelvin;
}


float read_tempsense(uint8_t fd,float resolution,uint8_t unit_choice)
{

	uint8_t* buffer = (uint8_t*)malloc(sizeof(uint8_t)*2);
	uint16_t high_byte;
	uint16_t low_byte;
	uint16_t temperature_value;
	float temp_celsius,temp_fahrenheit,temp_kelvin; 
	enum Status ret;
	uint8_t number_of_bytes;
	ret = i2c_read_word(fd,buffer);
	if(ret)	
		printf("\nError reading temperature sensor value into register\n");

	else
	{
		high_byte = buffer[0];
		low_byte = buffer[1];
		temperature_value=((high_byte<<8)|low_byte)>>4;
		if(high_byte & 0x80)
		{
			printf("\nNegative temperature value detected..\n");
			temperature_value=((high_byte<<8)|low_byte)>>4;
			temperature_value = (~temperature_value)+1;
		}
	}

	
	temp_celsius = temperature_value*resolution;
	temp_fahrenheit = (1.8*temp_celsius)+32;
	temp_kelvin = temp_celsius+273.15;
	printf("\n%fC,%fF.%fK\n",temp_celsius,temp_fahrenheit,temp_kelvin); 
	free(buffer);
	if(unit_choice== 0)
	{
		return celsius(temp_celsius);
	}
	else if(unit_choice==1)
	{
		return fahrenheit(temp_fahrenheit);
	}
	else if(unit_choice==2)
	{
		return kelvin(temp_kelvin);
	}
}

