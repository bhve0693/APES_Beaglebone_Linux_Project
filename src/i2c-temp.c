/*Refer https://www.kernel.org/doc/Documentation/i2c/dev-interface*/
/*https://elinux.org/Interfacing_with_I2C_Devices*/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h> 
#include "../inc/fw_i2c.h"

#define PTR_REG 0x00
#define TEMP_REG 0x00
#define CONFIG_REG 0x01
#define TLOW_REG 0x02
#define THIGH_REG 0x03

#define DEV_ADDR 0x48

int file;

//To check which i2c adapter needs to be accessed, run i2cdetect -l
/*uint8_t select_op(int opt)
{
	uint8_t op_config;
	switch(opt)
	{
		case 0:  opconfig = PTR_REG|TEMP_REG;
				 break;
		case 1:  opconfig = PTR_REG|CONFIG_REG;
				 break;
		case 2:  opconfig = PTR_REG|TLOW_REG;
				 break;
		case 3:  opconfig = PTR_REG|THIGH_REG;
				 break;
		default: printf("\nInvalid operation selection\n");
	}
	return op_config;
}

float config_temp()
{
	return resolution value=0.0625;
}

void read_tempsense(float resolution)
{
	uint8_t* buffer = (uint8_t*)malloc(sizeof(uint8_t)*2);
	uint16_t high_byte;
	uint16_t low_byte;
	uint16_t temperature_value;
	float temp_celsius,temp_fahrenheit; 
	enum Status ret;
	while(1)
	{
		ret = i2c_read(buffer);
		if(!ret)
			printf("\nError reading temperature sensor value into register\n");
		else
		{
			high_byte = buffer[0];
			low_byte = buffer[1];
			temperature_value=((high_byte<<8)|low_byte)>>4;
			temp_celsius = temperature_value*resolution;
			temp_fahrenheit = (1.8*temp_celsius)+32;
		}

	}

}*/

int main(int argc, char argv[])
{
	//fill in the value returned on running i2cdetect -l
	char *filename = "/dev/i2c-2";
	i2c_init(DEV_ADDR,filename);
	//float resolution_value;
	//uint8_t operation = select_op(0);
	/*if(operation)
	{
		i2c_write(&operation);
	}
	if(!operation)
	{
		read_tempsense(resolution_value);
	}*/
	uint8_t buf = 0x44;
	uint8_t readval;
	if(!(i2c_write(&buf)))
	{
		perror("Error writing byte 1\n");
	}
	if(!(i2c_read(&readval)))
	{
		perror("Error reading byte 1\n");
	}

}

