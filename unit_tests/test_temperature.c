/*
* FileName        : test-temperature.c
* Description     : A software that performs unint test using unity framework for temperature sensor driver
* File Author Name: Bhallaji Venkatesan,Divya Sampath Kumar
* Tools used      : gcc,gdb
* Reference       : HW-2 Professor's Alex Fosdick's Unity framework link
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "fw_i2c.h"
#include "i2c_temp.h"
#include "unity.h"



void test_init_temp_pass(void)
{
    uint16_t val;
    int fd = open("/dev/i2c-2",O_RDWR);
    stat = i2c_temp_init(fd,DEV_ADDR);
    TEST_ASSERT_EQUAL_INT32(0,(uint32_t)stat);
    close(fd);
}

void test_read_temp_register_treg_addr(void)
{
    uint16_t val;
    int fd = open("/dev/i2c-2",O_RDWR);
    stat = i2c_temp_init(fd,DEV_ADDR);
    stat =  read_temp_register(fd,REQ_TEMPREG_READ,&val);
    TEST_ASSERT_EQUAL_INT32(0,(uint32_t)stat);
    close(fd); 
}

void test_read_temp_register_configreg_addr(void)
{
    uint16_t val;
    int fd = open("/dev/i2c-2",O_RDWR);
    stat = i2c_temp_init(fd,DEV_ADDR);
    stat =  read_temp_register(fd,REQ_TEMPREG_CONFIG_READ,&val);
    TEST_ASSERT_EQUAL_INT32(0,(uint32_t)stat);
    close(fd); 
}

void test_read_temp_register_datalow_addr(void)
{
    uint16_t val;
    int fd = open("/dev/i2c-2",O_RDWR);
    stat = i2c_temp_init(fd,DEV_ADDR);
    stat =  read_temp_register(fd,REQ_TEMPREG_DATA_LOW_READ,&val);
    TEST_ASSERT_EQUAL_INT32(0,(uint32_t)stat);
    close(fd); 
}

void test_read_temp_register_datahigh_addr(void)
{
    uint16_t val;
    int fd = open("/dev/i2c-2",O_RDWR);
    stat = i2c_temp_init(fd,DEV_ADDR);
    stat =  read_temp_register(fd,REQ_TEMPREG_DATA_HIGH_READ,&val);
    TEST_ASSERT_EQUAL_INT32(0,(uint32_t)stat);
    close(fd); 
}

void test_read_temp_register_invalid_addr(void)
{
    uint16_t val;
    int fd = open("/dev/i2c-2",O_RDWR);
    stat = i2c_temp_init(fd,DEV_ADDR);
    stat =  read_temp_register(fd,90,&val);
    TEST_ASSERT_EQUAL_INT32(1,(uint32_t)stat);
    close(fd); 
}


void test_write_temp_register_ptr_addr_fail(void)
{
    uint16_t val = 0x0008;
    int fd = open("/dev/i2c-2",O_RDWR);
    stat = i2c_temp_init(fd,DEV_ADDR);
    stat =  write_temp_register(fd,REQ_TEMPREG_PTRREG_WRITE,val);
    TEST_ASSERT_EQUAL_INT32(1,(uint32_t)stat);
    close(fd); 
}

void test_write_temp_register_ptr_addr_success(void)
{
    uint16_t val = 0x0002;
    int fd = open("/dev/i2c-2",O_RDWR);
    stat = i2c_temp_init(fd,DEV_ADDR);
    stat =  write_temp_register(fd,REQ_TEMPREG_PTRREG_WRITE,val);
    TEST_ASSERT_EQUAL_INT32(0,(uint32_t)stat);
    close(fd); 
}

void test_write_temp_register_configreg_addr(void)
{
    uint16_t val = 0x6020;
    int fd = open("/dev/i2c-2",O_RDWR);
    stat = i2c_temp_init(fd,DEV_ADDR);
    stat =  write_temp_register(fd,REQ_TEMPREG_CONFIG_WRITE,val);
    TEST_ASSERT_EQUAL_INT32(0,(uint32_t)stat);
    close(fd); 
}

void test_write_temp_register_datalow_addr(void)
{
    uint16_t val = 0x8800;
    int fd = open("/dev/i2c-2",O_RDWR);
    stat = i2c_temp_init(fd,DEV_ADDR);
    stat =  write_temp_register(fd,REQ_TEMPREG_DATA_LOW_WRITE,val);
    TEST_ASSERT_EQUAL_INT32(0,(uint32_t)stat);
    close(fd); 
}

void test_write_temp_register_datahigh_addr(void)
{
    uint16_t val = 0xF000;
    int fd = open("/dev/i2c-2",O_RDWR);
    stat = i2c_temp_init(fd,DEV_ADDR);
    stat =  write_temp_register(fd,REQ_TEMPREG_DATA_HIGH_WRITE,val);
    TEST_ASSERT_EQUAL_INT32(0,(uint32_t)stat);
    close(fd); 
}

void test_write_temp_register_invalid_addr(void)
{
    uint16_t val = 0x9900;
    int fd = open("/dev/i2c-2",O_RDWR);
    stat = i2c_temp_init(fd,DEV_ADDR);
    stat =  read_temp_register(fd,90,&val);
    TEST_ASSERT_EQUAL_INT32(1,(uint32_t)stat);
    close(fd); 
}

void test_shutdown_mode(void)
{
    uint16_t val = 1;
    int fd = open("/dev/i2c-2",O_RDWR);
    stat = i2c_temp_init(fd,DEV_ADDR);
    stat =  shutdown_temp_mode(fd,val);
    TEST_ASSERT_EQUAL_INT32(0,(uint32_t)stat);
    close(fd); 
}

void test_continuous_conversion_mode_valid(void)
{
    uint16_t val = 1;
    int fd = open("/dev/i2c-2",O_RDWR);
    stat = i2c_temp_init(fd,DEV_ADDR);
    stat =  shutdown_temp_mode(fd,val);
    TEST_ASSERT_EQUAL_INT32(0,(uint32_t)stat);
    close(fd); 
}

void test_continuous_conversion_mode_invalid(void)
{
    uint16_t val = 8;
    int fd = open("/dev/i2c-2",O_RDWR);
    stat = i2c_temp_init(fd,DEV_ADDR);
    stat =  shutdown_temp_mode(fd,val);
    TEST_ASSERT_EQUAL_INT32(1,(uint32_t)stat);
    close(fd); 
}

void test_temp_read(void)
{
    uint16_t unit_choice = 2;
    float val = 0.0;
    int fd = open("/dev/i2c-2",O_RDWR);
    stat = i2c_temp_init(fd,DEV_ADDR);
    val =  temp_read(fd,val);
    TEST_ASSERT_EQUAL_INT32(0,(uint32_t)stat);
    close(fd); 
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_init_temp_pass);
    RUN_TEST(test_read_temp_register_treg_addr);
    RUN_TEST(test_read_temp_register_configreg_addr);
    RUN_TEST(test_read_temp_register_datalow_addr);
    RUN_TEST(test_read_temp_register_datahigh_addr);
    RUN_TEST(test_read_temp_register_invalid_addr);

    RUN_TEST(test_write_temp_register_ptr_addr_fail);
    RUN_TEST(test_write_temp_register_ptr_addr_success);
    RUN_TEST(test_write_temp_register_configreg_addr);
    RUN_TEST(test_write_temp_register_datalow_addr);
    RUN_TEST(test_write_temp_register_datahigh_addr);
    RUN_TEST(test_write_temp_register_invalid_addr);


    RUN_TEST(test_shutdown_mode);
    RUN_TEST(test_continuous_conversion_mode_valid);
    RUN_TEST(test_continuous_conversion_mode_valid);
    RUN_TEST(test_temp_read);

    return UNITY_END();
}