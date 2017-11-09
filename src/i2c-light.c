/*
* FileName        : i2c-light.c
* Description     :	A software that includes several APIs to access and modify light sensor registers-such as
					read_light_registers(),write_light_register() and functions to do some special configurations
* File Author Name:	Divya Sampath Kumar, Bhallaji Venkatesan
* Tools used	  :	gcc,gdb
* Reference       : Datasheet
*/

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

enum brightness light_indication = DARK;

enum Status read_id_reg(uint8_t fd,uint8_t reg,uint8_t *val)
{
	enum Status stat;
	stat = i2c_write(fd,&reg);
	if(!stat)
	{
		stat = i2c_read(fd,val);
	}
	return stat;
}

enum Status read_timing_reg(uint8_t fd,uint8_t reg,uint8_t *val)
{
	enum Status stat;
	stat = i2c_write(fd,&reg);
	if(!stat)
	{
		stat = i2c_read(fd,val);
	}
	return stat;
}

enum Status read_intr_reg(uint8_t fd,uint8_t reg,uint8_t *val)
{
	enum Status stat;
	stat = i2c_write(fd,&reg);
	if(!stat)
	{
		stat = i2c_read(fd,val);
	}
	return stat;
}

enum Status read_cntl_reg(uint8_t fd,uint8_t reg,uint8_t *val)
{
	enum Status stat;
	stat = i2c_write(fd,&reg);
	if(!stat)
	{
		stat = i2c_read(fd,val);
	}
	return stat;
}


enum Status read_adc_reg(uint8_t fd,uint8_t reg,uint8_t *val)
{
	enum Status stat;
	stat = i2c_write(fd,&reg);
	if(!stat)
	{
		stat = i2c_read(fd,val);
	}
	return stat;
}

enum Status read_threshlow_reg(uint8_t fd,uint8_t reg, uint16_t *val)
{
	enum Status stat;

	stat = i2c_write(fd,&reg);
	uint8_t *buffer = (uint8_t*)malloc(2*sizeof(uint8_t));
	if(!stat)
	{
		stat = i2c_read_word(fd,buffer);
		*val = (uint16_t)(buffer[0]<<8) | (uint16_t)(buffer[1]);
	}
	return stat;	
}

enum Status read_threshhigh_reg(uint8_t fd,uint8_t reg, uint16_t *val)
{
	enum Status stat;

	stat = i2c_write(fd,&reg);
	uint8_t *buffer = (uint8_t*)malloc(2*sizeof(uint8_t));
	if(!stat)
	{
		stat = i2c_read_word(fd,buffer);
		*val = (uint16_t)(buffer[0]<<8) | (uint16_t)(buffer[1]);
	}
	return stat;	
}


enum Status write_contrl_reg(uint8_t fd,uint8_t reg,uint8_t val)
{
	enum Status stat;
	uint8_t *buffer = (uint8_t*)(malloc(sizeof(uint8_t)*2));
	buffer[0] = reg;
	buffer[1] = val;
	stat = i2c_write_light(fd,buffer);
	return stat;
}

enum Status write_intr_reg(uint8_t fd,uint8_t reg,uint8_t val)
{
	enum Status stat;
	uint8_t *buffer = (uint8_t*)(malloc(sizeof(uint8_t)*2));
	buffer[0] = reg;
	buffer[1] = val;
	stat = i2c_write_light(fd,buffer);
	return stat;
}

enum Status write_timing_reg(uint8_t fd,uint8_t reg,uint8_t val)
{
	enum Status stat;
	uint8_t *buffer = (uint8_t*)(malloc(sizeof(uint8_t)*2));
	buffer[0] = reg;
	buffer[1] = val;
	stat = i2c_write_light(fd,buffer);
	return stat;
}

enum Status write_threshlow_reg(uint8_t fd,uint8_t reg,uint16_t val)
{
	enum Status stat;
	uint8_t *buffer = (uint8_t*)(malloc(sizeof(uint8_t)*2));
	buffer[0] = reg;
	buffer[1] = (uint8_t)((val & 0xFF00)>>8);
	stat = i2c_write_light(fd,buffer);
	buffer[0] = reg+1;
	buffer[1] = (uint8_t)(val & 0x00FF);
	stat = i2c_write_light(fd,buffer);
	return stat;
}

enum Status write_threshhigh_reg(uint8_t fd,uint8_t reg,uint16_t val)
{
	enum Status stat;
	uint8_t *buffer = (uint8_t*)(malloc(sizeof(uint8_t)*2));
	buffer[0] = reg;
	buffer[1] = (uint8_t)((val & 0xFF00)>>8);
	stat = i2c_write_light(fd,buffer);
	buffer[0] = reg+1;
	buffer[1] = (uint8_t)(val & 0x00FF);
	stat = i2c_write_light(fd,buffer);
	return stat;
}

enum Status read_light_registers(uint8_t fd,uint8_t reg_option,uint16_t *val)
{
	enum Status stat;
	uint8_t high_byte,low_byte,addr,value;
	switch(reg_option)
	{
		case ID: 
			value = (uint8_t)(*val);
			stat = read_id_reg(fd,CMD_REG|ID_REG,&value);
			*val = (uint16_t)value;
			break;

		case TIMING: 
			value = (uint8_t)(*val);
			stat = read_timing_reg(fd,CMD_REG|TIMING_REG,&value);
			*val = (uint16_t)value;
			break;

		case INTR: 
			value = (uint8_t)(*val);
			stat = read_intr_reg(fd,CMD_REG|INTR_REG,&value);
			*val = (uint16_t)value;
			break;

		case CNTRL:
			value = (uint8_t)(*val);
			stat = read_cntl_reg(fd, CMD_REG|CNTRL_REG,&value);
			*val = (uint16_t)value;
			break;

		case T_LOW:
			stat = read_threshlow_reg(fd, CMD_REG|THRESH_LOW_REG,val);
			break;

		case T_HIGH:
			stat = read_threshhigh_reg(fd, CMD_REG|THRESH_HIGH_REG,val);
			break;


		case ADC0: 
			high_byte = (uint8_t)(*val);
			stat = read_adc_reg(fd,CMD_REG|ADC0_REG_LOW,&high_byte);
			low_byte = (uint8_t)(*val);
			stat = read_adc_reg(fd,CMD_REG|ADC0_REG_LOW,&low_byte);
			*val = ((uint16_t)(high_byte)<<8) | ((uint16_t)(low_byte));
			break;

		case ADC1: 
			high_byte = (uint8_t)(*val);
			stat = read_adc_reg(fd,CMD_REG|ADC1_REG_LOW,&high_byte);
			low_byte = (uint8_t)(*val);
			stat = read_adc_reg(fd,CMD_REG|ADC1_REG_HIGH,&low_byte);
			*val = ((uint16_t)(high_byte)<<8) | ((uint16_t)(low_byte));
			break;
			


		default:stat = FAIL;
				printf("\nERR:Incorrect register chosen to read\n");	
	}

	return stat;
}


enum Status write_light_registers(uint8_t fd,uint8_t reg_option,uint16_t val)
{
	enum Status stat;
	uint8_t value;
	switch(reg_option)
	{
		case CNTRL:
			if(val == 0x01 || val ==0x02 || val>0x03)
			{
				printf("ERR:Invalid CTRL Write value\n");
				stat = FAIL;
			}
			else 
			{
				value = (uint8_t) val;
				stat = write_contrl_reg(fd,CMD_REG|CNTRL_REG,value);
			}
			break;

		case TIMING:
			if(val > 0x31 || (val == 0x0004))
			{
				printf("ERR:Invalid Timing Write value\n");
				stat = FAIL;
			}
			else
			{
				value = (uint8_t) val;
				stat = write_timing_reg(fd,CMD_REG|TIMING_REG,value);
			}
			break;

		case INTR:
			if(val > 0x63)
			{
				printf("ERR:Invalid INTR Write value\n");
				stat = FAIL;
			}
			else
			{
				value = (uint8_t) val;
				stat = write_intr_reg(fd,CMD_REG|INTR_REG,value);
			}
			break;

		case T_LOW:
			stat = write_threshlow_reg(fd,CMD_REG|THRESH_LOW_REG,val);
			break;

		case T_HIGH:
			stat = write_threshhigh_reg(fd,CMD_REG|THRESH_HIGH_REG,val);
			break;
		
		default:stat = FAIL;
				printf("\nERR:Incorrect register chosen to write\n");	
	}
	return stat;
}



enum Status config_integration_timing(uint8_t fd,uint8_t integration_option)
{
	uint16_t timing_reg_val;
	uint8_t t8_val;
	enum Status stat;
	stat = read_light_registers(fd,TIMING,&timing_reg_val);
	t8_val = (uint8_t)timing_reg_val;
	t8_val &= 0x00;
	if(!integration_option)
	{

		t8_val |= INT_137;
		printf("Timing register set with Int value 13.7ms:%x\n",t8_val);
	}
	else if(integration_option==1)
	{

		t8_val |= INT_101;
		printf("Timing register set with Int value 101ms:%x\n",t8_val);
	}
	else if(integration_option==2)
	{
		t8_val |= INT_402;
		printf("Timing register set with Int value 402ms:%x\n",t8_val);
	}
	else
	{
		printf("\nERR:Option not applicable\n");
		stat = FAIL;
	}
	stat = write_light_registers(fd,TIMING,t8_val);
	return stat;
}


enum Status control_intr_reg(uint8_t fd,uint8_t intr_option)
{
	uint16_t intr_val;
	uint8_t i8_val;
	enum Status stat;
	stat = read_light_registers(fd,INTR,&intr_val);
	i8_val = (uint8_t)intr_val;
	i8_val &= 0x00;
	if(intr_option)
	{

		i8_val |= INTR_ENABLE;
		printf("Interrupt register enabled:%x\n",i8_val);
	}
	else if(!intr_option)
	{
		printf("Interrupt register disabled:%x\n",i8_val);
	}
	else
	{
		printf("\nERR:Option not applicable\n");
		stat = FAIL;
	}
	stat = write_light_registers(fd,INTR,i8_val);
	return stat;
}


enum brightness night_or_day()
{
	return light_indication;
}

enum Status light_sensor_switchon(uint8_t fd)
{
	enum Status stat;
	uint8_t readval;
	uint8_t reg_value=CMD_REG|CNTRL_REG;
	uint8_t on_val = ON_VALUE;
	stat = i2c_write(fd,&reg_value);
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
	
	if(stat)
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
	if(stat)
	{
		printf("\nFailed to read value from control register:%s\n",strerror(errno));
		return FAIL;
		exit(1);
	}
	final_lumen = (readval_H<<8)|readval_L;
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

double light_read(uint8_t fd)
{

	double light_lux;
	uint8_t readval;
	uint16_t final_lumen0,final_lumen1,tl_val,th_val,tmid_val;
	enum Status stat;
	stat = light_sensor_switchon(fd);
	if(stat)
	{
		printf("\nFailed light sensor ON operation:%s\n",strerror(errno));
		exit(1);
	}


	stat = read_light_registers(fd,T_LOW,&tl_val);
	if(stat) printf("ERR:Unable to read ID Register:%s",strerror(errno));
	else printf("\nLOW threshold is set to:%4x\n",tl_val);

	stat = read_light_registers(fd,T_HIGH,&th_val);
	if(stat) printf("ERR:Unable to read ID Register:%s",strerror(errno));
	else printf("\nHIGH threshold is set to:%4x\n",th_val);

	tmid_val = 0x4000;
	final_lumen0 = read_adc0(fd);
	if(final_lumen0>=tl_val && final_lumen0<tmid_val)
	{
		light_indication = DARK;

	}
	else if(final_lumen0>=tmid_val && final_lumen0<=th_val)
	{
		light_indication = LIGHT;	
	}
	final_lumen1 = read_adc1(fd);

	if(final_lumen0 !=0)
		light_lux = calc_lux_val(final_lumen0,final_lumen1);
	printf("\nLight intensity value:%lf\n",light_lux);
	return light_lux;

}

