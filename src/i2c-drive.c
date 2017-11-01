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
#include "fw_i2c.h"

#define PTR_REG_READ_ONLY 0x00
#define PTR_REG_CONFIG 0x01
#define PTR_REG_TLOW 0x02
#define PTR_REG_THIGH 0x03

int file;

//To check which i2c adapter needs to be accessed, run i2cdetect -l
void i2c_init()
{
	int adapter_number = 0; //fill in the value returned on running i2cdetect -l
	file = open("/dev/i2c-0",O_RDWR);
	if(file<0)
	{
		//check error number and print appropriate message
		printf("Error opening file:%s\n",strerror(errno));
		exit(1);
	}
	//Once device is opened. we must specify with what device address we want to communicate

	int addr = 0x48;
	if(ioctl(file,I2C_SLAVE,addr)<0)
	{
		printf("ioctl error:%s\n",strerror(errno));
		exit(1);
	}
}

enum Status i2c_write()
{
	int val = 0x45;
	if(write(file,&val,1)!=1)
		{
			printf("Error writing values into file:%s",strerror(errno));
			return FAIL;
		}
	return SUCCESS;
}

enum Status i2c_read()
{
	int val;
	if(read(file,&val,1)!=1)
	{
		printf("Error reading value from file:%s",strerror(errno));
		return FAIL;
	}
	return SUCCESS;
}


int main(int argc, char argv[])
{
	int read_val;
	enum Status ret;
	i2c_init();
	ret = i2c_write();
	if(ret)
	{
		printf("Written succesfully..\n");
	}
	read_val = i2c_read();
	printf("Value read from file is:%d\n",read_val);
}

