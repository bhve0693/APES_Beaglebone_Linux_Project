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
#include "fw_i2c.h"
#include "i2c_temp.h"


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
*/
float temp_read()
{
	int fd=open("/dev/i2c-2",O_RDWR);
	uint8_t buf = 0x00;
	uint8_t readval;
	enum Status stat;
	if(fd<0)
	{
		printf("Error opening file:%s\n",strerror(errno));
		exit(1);
	}
	stat = i2c_temp_init(fd,DEV_ADDR);
	if(!stat)
	{
		printf("\nSuccessful ioctl() operation\n");
	}
	else 
	{
		printf("\nFailed ioctl() operation:%s\n",strerror(errno));
		exit(1);
	}
	stat = i2c_write(fd,&buf);
	if(!stat)
	{
		printf("\nSuccessfully selected temperature register read operation\n");
	}
	else 
	{
		printf("\nFailed to select temperature register:%s\n",strerror(errno));
		exit(1);
	}
	
	float temp_celsius = read_tempsense(fd,0.0625);
	return temp_celsius;
	close(fd);	
}

float read_tempsense(uint8_t fd,float resolution)
{
	printf("\nentered read_tempsense\n");
	uint8_t* buffer = (uint8_t*)malloc(sizeof(uint8_t)*2);
	uint16_t high_byte;
	uint16_t low_byte;
	uint16_t temperature_value;
	float temp_celsius,temp_fahrenheit; 
	enum Status ret;
	uint8_t number_of_bytes;
	ret = i2c_read(fd,buffer);
	if(ret)	
		printf("\nError reading temperature sensor value into register\n");
	else
	{
		high_byte = buffer[0];
		low_byte = buffer[1];
	}
	temperature_value=((high_byte<<8)|low_byte)>>4;
	temp_celsius = temperature_value*resolution;
	//temp_fahrenheit = (1.8*temp_celsius)+32;
	//printf("\n%fC,%fF\n",temp_celsius,temp_fahrenheit); 
	free(buffer);
	return temp_celsius;

}

/*int main(int argc, char argv[])
{
	temp_read();
}*/

