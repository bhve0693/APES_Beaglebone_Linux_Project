#include <stdlib.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "fw_i2c.h"
#include "i2c_light.h"


#define CMD_REG 0x80
#define CNTRL_REG 0x00
#define ADC0_REG_LOW 0x0C
#define ADC0_REG_HIGH 0x0D
#define ADC1_REG_LOW 0x0E
#define ADC1_REG_HIGH 0x0F
#define DEV_ADDR 0x39

enum Status light_sensor_switchon(uint8_t fd)
{
	enum Status stat;
	uint8_t reg_addr=CMD_REG|CNTRL_REG;
	uint8_t readval;
	uint8_t on_val = 0x03;
	stat = i2c_write(fd,&reg_addr);
	if(stat)
	{
		printf("\nFailed control register access operation:%s\n",strerror(errno));
		return FAIL;
		exit(1);
	}
	stat = i2c_read(fd,&readval);
	if(stat)
	{
		printf("\nFailed to read value from control register:%s\n",strerror(errno));
		return FAIL;
		exit(1);
	}
	if(readval==0x00)
	{
		printf("Light sensor is powered off....write value 0x03 to power on\n");
		stat = i2c_write(fd,&on_val);
		if(stat)
		{
			printf("\nFailed control register access operation:%s\n",strerror(errno));
			return FAIL;
			exit(1);
		}
	}
	stat = i2c_read(fd,&readval);
	if(stat)
	{
		printf("\nFailed to read value from control register:%s\n",strerror(errno));
		return FAIL;
		exit(1);
	}
	return SUCCESS;
}

uint16_t read_adc0(uint8_t fd)
{
	enum Status stat;
	uint8_t reg_addr=CMD_REG|ADC0_REG_LOW;
	uint8_t readval_L,readval_H;
	uint16_t final_lumen;
	stat = i2c_write(fd,&reg_addr);
	if(stat)
	{
		printf("\nFailed ADC low register access operation:%s\n",strerror(errno));
		return FAIL;
		exit(1);
	}
	stat = i2c_read(fd,&readval_L);
	
	if(!stat)
	{
		printf("\nSuccessfully read light value from ADC0 low register operation. Value is %d.\n",readval_L);
	}
	else
	{
		printf("\nFailed to read value from control register:%s\n",strerror(errno));
		return FAIL;
		exit(1);
	}

	reg_addr=CMD_REG|ADC0_REG_HIGH;
	stat = i2c_write(fd,&reg_addr);
	if(stat)
	{
		printf("\nFailed ADC low register access operation:%s\n",strerror(errno));
		return FAIL;
		exit(1);
	}
	stat = i2c_read(fd,&readval_H);
	if(!stat)
	{
		printf("\nSuccessfully read light value from ADC0 high register operation. Value is %d.\n",readval_H);
	}
	else 
	{
		printf("\nFailed to read value from control register:%s\n",strerror(errno));
		return FAIL;
		exit(1);
	}
	final_lumen = (readval_H<<8)|readval_L;
	printf("Final light value:%d\n",final_lumen);
	return final_lumen;
}

uint16_t read_adc1(uint8_t fd)
{
	enum Status stat;
	uint8_t reg_addr=CMD_REG|ADC1_REG_LOW;
	uint8_t readval_L,readval_H;
	uint16_t final_lumen;
	stat = i2c_write(fd,&reg_addr);
	if(stat)
	{
		printf("\nFailed ADC low register access operation:%s\n",strerror(errno));
		return FAIL;
		exit(1);
	}
	stat = i2c_read(fd,&readval_L);
	
	if(!stat)
	{
		printf("\nSuccessfully read light value from ADC1 low register operation. Value is %d.\n",readval_L);
	}
	else
	{
		printf("\nFailed to read value from control register:%s\n",strerror(errno));
		return FAIL;
		exit(1);
	}

	reg_addr=CMD_REG|ADC1_REG_HIGH;
	stat = i2c_write(fd,&reg_addr);
	if(stat)
	{
		printf("\nFailed ADC low register access operation:%s\n",strerror(errno));
		return FAIL;
		exit(1);
	}
	stat = i2c_read(fd,&readval_H);
	if(!stat)
	{
		printf("\nSuccessfully read light value from ADC1 high register operation. Value is %d.\n",readval_H);
	}
	else 
	{
		printf("\nFailed to read value from control register:%s\n",strerror(errno));
		return FAIL;
		exit(1);
	}
	final_lumen = (readval_H<<8)|readval_L;
	printf("Final light value:%d\n",final_lumen);
	return final_lumen;

}

double calc_lux_val(uint16_t final_lumen0,uint16_t final_lumen1)
{
	double div=(double)final_lumen1/(double)final_lumen0;
	printf("\nDiv is %lf\n",div);
	double sensor_lux=0.0;
	if(div>0 && div<=0.5)
		sensor_lux = (0.0304*final_lumen0)-(0.062*final_lumen0*pow(div,1.4));
	else if(div>0.5 && div<=0.61)
		sensor_lux = (0.0224*final_lumen0)-0.031*final_lumen1;
	else if(div>0.61 && div<=0.80)
		sensor_lux = (0.0128*final_lumen0)-0.0153*final_lumen1;
	else if(div>0.8 && div<=1.3)
		sensor_lux = (0.00146*final_lumen0)-0.00112*final_lumen1;
	else sensor_lux=0.0;
	return sensor_lux;
}

double light_read()
{
	int fd;
	fd = open("/dev/i2c-2",O_RDWR);
	double light_lux;
	uint8_t readval;
	uint16_t final_lumen0,final_lumen1;
	enum Status stat;
	if(fd<0)
	{
		printf("Error opening file:%s\n",strerror(errno));
		exit(1);
	}
	stat = i2c_temp_init(fd,DEV_ADDR);
	if(stat)
	{
		printf("\nFailed ioctl() operation:%s\n",strerror(errno));
		exit(1);
	}
	stat = light_sensor_switchon(fd);
	if(stat)
	{
		printf("\nFailed light sensor ON operation:%s\n",strerror(errno));
		exit(1);
	}
	final_lumen0 = read_adc0(fd);
	final_lumen1 = read_adc1(fd);
	if(final_lumen0 !=0)
		light_lux = calc_lux_val(final_lumen0,final_lumen1);
	printf("\n Light Lux in light read %lf\n",light_lux);
	close(fd);	
	return light_lux;

}

