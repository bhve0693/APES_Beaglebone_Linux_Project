/*
* FileName        : test-light.c
* Description     : A software that performs unint test using unity framework for light sensor driver
* File Author Name: Bhallaji Venkatesan,Divya Sampath Kumar
* Tools used      : gcc,gdb
* Reference       : HW-2 Professor Alex Fosdick's Unity framework link
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "fw_i2c.h"
#include "i2c_temp.h"
#include "i2c_light.h"
#include "unity.h"



void test_init_light_pass(void)
{
    uint16_t val;
    int fd = open("/dev/i2c-2",O_RDWR);
    stat = i2c_light_init(fd,DEV_LIGHT_ADDR);
    TEST_ASSERT_EQUAL_INT32(0,(uint32_t)stat);
    close(fd);
}

void test_read_light_registers_id_addr(void)
{
    uint16_t val;
    int fd = open("/dev/i2c-2",O_RDWR);
    stat = i2c_light_init(fd,DEV_LIGHT_ADDR);
    stat =  read_light_registers(fd,ID,&val);
    TEST_ASSERT_EQUAL_INT32(0,(uint32_t)stat);
    close(fd); 
}

void test_read_light_registers_timing_addr(void)
{
    uint16_t val;
    int fd = open("/dev/i2c-2",O_RDWR);
    stat = i2c_light_init(fd,DEV_LIGHT_ADDR);
    stat =  read_light_registers(fd,TIMING,&val);
    TEST_ASSERT_EQUAL_INT32(0,(uint32_t)stat);
    close(fd); 
}

void test_read_light_registers_intr_addr(void)
{
    uint16_t val;
    int fd = open("/dev/i2c-2",O_RDWR);
    stat = i2c_light_init(fd,DEV_LIGHT_ADDR);
    stat =  read_light_registers(fd,INTR,&val);
    TEST_ASSERT_EQUAL_INT32(0,(uint32_t)stat);
    close(fd); 
}

void test_read_light_registers_cntrl_addr(void)
{
    uint16_t val;
    int fd = open("/dev/i2c-2",O_RDWR);
    stat = i2c_light_init(fd,DEV_LIGHT_ADDR);
    stat =  read_light_registers(fd,CNTRL,&val);
    TEST_ASSERT_EQUAL_INT32(0,(uint32_t)stat);
    close(fd); 
}

void test_read_light_registers_tlow_addr(void)
{
    uint16_t val;
    int fd = open("/dev/i2c-2",O_RDWR);
    stat = i2c_light_init(fd,DEV_LIGHT_ADDR);
    stat =  read_light_registers(fd,T_LOW,&val);
    TEST_ASSERT_EQUAL_INT32(0,(uint32_t)stat);
    close(fd); 
}

void test_read_light_registers_adc0_addr(void)
{
    uint16_t val;
    int fd = open("/dev/i2c-2",O_RDWR);
    stat = i2c_light_init(fd,DEV_LIGHT_ADDR);
    stat =  read_light_registers(fd,ADC0,&val);
    TEST_ASSERT_EQUAL_INT32(0,(uint32_t)stat);
    close(fd); 
}

void test_read_light_registers_adc1_addr(void)
{
    uint16_t val;
    int fd = open("/dev/i2c-2",O_RDWR);
    stat = i2c_light_init(fd,DEV_LIGHT_ADDR);
    stat =  read_light_registers(fd,ADC1,&val);
    TEST_ASSERT_EQUAL_INT32(0,(uint32_t)stat);
    close(fd); 
}


void test_read_light_registers_thigh_addr(void)
{
    uint16_t val;
    int fd = open("/dev/i2c-2",O_RDWR);
    stat = i2c_light_init(fd,DEV_LIGHT_ADDR);
    stat =  read_light_registers(fd,T_HIGH,&val);
    TEST_ASSERT_EQUAL_INT32(0,(uint32_t)stat);
    close(fd); 
}

void test_read_light_register_invalid_addr(void)
{
    uint16_t val;
    int fd = open("/dev/i2c-2",O_RDWR);
    stat = i2c_temp_init(fd,DEV_ADDR);
    stat =  read_temp_register(fd,90,&val);
    TEST_ASSERT_EQUAL_INT32(1,(uint32_t)stat);
    close(fd); 
}


void test_write_light_register_cntrl_addr_fail(void)
{
    uint16_t val = 0x0008;
    int fd = open("/dev/i2c-2",O_RDWR);
    stat = i2c_temp_init(fd,DEV_ADDR);
    stat =  write_temp_register(fd,CNTRL,val);
    TEST_ASSERT_EQUAL_INT32(1,(uint32_t)stat);
    close(fd); 
}

void test_write_light_register_cntrl_addr_success(void)
{
    uint16_t val = 0x0003;
    int fd = open("/dev/i2c-2",O_RDWR);
    stat = i2c_temp_init(fd,DEV_ADDR);
    stat =  write_temp_register(fd,CNTRL,val);
    TEST_ASSERT_EQUAL_INT32(1,(uint32_t)stat);
    close(fd); 
}


void test_light_read(void)
{
    uint16_t unit_choice = 2;
    double val = 0.0;
    int fd = open("/dev/i2c-2",O_RDWR);
    stat = i2c_temp_init(fd,DEV_ADDR);
    val =  light_read(fd);
    TEST_ASSERT_EQUAL_INT32(0,(uint32_t)stat);
    close(fd); 
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_init_light_pass);
    RUN_TEST(test_read_light_registers_id_addr);
    RUN_TEST(test_read_light_registers_adc1_addr);
    RUN_TEST(test_read_light_registers_adc0_addr);
    RUN_TEST(test_read_light_registers_tlow_addr);
    RUN_TEST(test_read_light_registers_thigh_addr);
    RUN_TEST(test_read_light_registers_timing_addr);
    RUN_TEST(test_read_light_registers_intr_addr);
    RUN_TEST(test_read_light_registers_cntrl_addr);
    RUN_TEST(test_read_light_register_invalid_addr);


    RUN_TEST(test_write_light_register_cntrl_addr_fail);
    RUN_TEST(test_write_light_register_cntrl_addr_success);
    
    RUN_TEST(test_light_read);

    return UNITY_END();
}