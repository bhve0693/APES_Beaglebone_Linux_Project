
/**********************************************************************************************
​ * ​ ​ Redistribution,​ ​ modification​ ​ or​ ​ use​ ​ of​ ​ this​ ​ software​ ​ in​ ​ source​ ​ or​ ​ binary
​ * ​ ​ forms​ ​ is​ ​ permitted​ ​ as​ ​ long​ ​ as​ ​ the​ ​ files​ ​ maintain​ ​ this​ ​ copyright.​ ​ Users​ ​ are
​ * ​ ​ permitted​ ​ to​ ​ modify​ ​ this​ ​ and​ ​ use​ ​ it​ ​ to​ ​ learn​ ​ about​ ​ the​ ​ field​ ​ of​ ​ embedded
​ * ​ ​ software.​ ​ Alex​ ​ Fosdick​ ​ and​ ​ the​ ​ University​ ​ of​ ​ Colorado​ ​ are​ ​ not​ ​ liable​ ​ for
​ * ​ ​ any​ ​ misuse​ ​ of​ ​ this​ ​ material.
​ *
***********************************************************************************************/
/**
​ * ​ ​ @file​ ​ i2c_light.h
​ * ​ ​ @brief​ ​ An​ ​ abstraction​ ​ for​ using th I2C Driver
​ * ​ ​ This​ ​ header​ ​ file​ ​ provides​ ​ an​ ​ abstraction​ ​ of​ ​a I2C Driver Application
 *   implementation
 *
​ * ​ ​ @author​ ​  Divya Sampath Kumar, Bhallaji Venkatesan
​ * ​ ​ @date​ ​    11/1/2017
​ *
​ */
#ifndef __I2C_LIGHT_H_INCLUDED
#define __I2C_LIGHT_H_INCLUDED

#include <stdio.h>

#define CMD_REG 0x80
#define CNTRL_REG 0x00
#define ID_REG 0x0A
#define TIMING_REG 0x01
#define INTR_REG 0x06
#define ADC0_REG_LOW 0x0C
#define ADC0_REG_HIGH 0x0D
#define ADC1_REG_LOW 0x0E
#define ADC1_REG_HIGH 0x0F
#define THRESH_LOW_REG 0x02
#define THRESH_HIGH_REG 0x04


#define INT_137 0x00
#define INT_101 0x01
#define INT_402 0x02
#define INTR_ENABLE 0x01

#define ON_VALUE 0x03
#define DEV_LIGHT_ADDR 0x39


/*
​ * ​ ​ @brief​    : Enum for indicating different Light Sensor Registers
​
 *   CMD       : Command Register
 *   CNTRL     : Control Register
 *   TIMING    : Timing Register
 *   INTR      : Interrupt Register
 *   ID        : Identification Register
 *   ADC0      : ADC Channel 0 Register
 *   ADC1      : ADC Channel 1 Register
 *   T_LOW     : Threshold Low Register
 *   T_HIGH    : Threshold High Register
​ */
enum RegOptions {CMD,CNTRL,TIMING,INTR,ID,ADC0,ADC1,T_LOW,T_HIGH}reg;




/**
​ * ​ ​ @brief​ : This function sets the control register to 0x03 to power on the sensor
​ *
​ * ​ ​ Returns status Success/fail to indicate light sensor on status
 *	 @param : fd  - accepts I2C Device file descriptor
​ */
enum Status light_sensor_switchon(uint8_t fd);



/**
​ * ​ ​ @brief​ : The function reads ADC0 for measured value(Visible+IR)
​ *
​ * ​ ​ Returns value of uint16_t data type
​ * ​ ​ @param : fd  - accepts I2C Device file descriptor
​ */
uint16_t read_adc0(uint8_t fd);

/**
​ * ​ ​ @brief​ : The function reads ADC1 for measured valuee(IR)
​ *
​ * ​ ​ Returns value of uint16_t data type
​ * ​ ​ @param : fd  - accepts I2C Device file descriptor
​ */
uint16_t read_adc1(uint8_t fd);

/**
​ * ​ ​ @brief​ : The function calculates lux value equivalent
​ *
​ * ​ ​ Returns value of double data type
​ * ​ ​ @param : final_lumen0-value returned from read_adc0()
 * ​ ​ @param : final_lumen1-value returned from read_adc1()

​ */
double calc_lux_val(uint16_t final_lumen0,uint16_t final_lumen1);

/**
​ * ​ ​ @brief​ : The API returns the final calculated lux value
​ *
​ * ​ ​ Returns value of double data type
​ */
double light_read(uint8_t fd);


/**
​ * ​ ​ @brief​ : The function reads the ID Register
​ *
​ * ​ ​ Returns status indicating SUCCESS/FAIL Read ID operation
 *    
​ * ​ ​ @param : fd-File Descriptor
 * ​ ​ @param : reg-ID Register Address
 *   @param : *val-pointer to value read from register

​ */
enum Status read_id_reg(uint8_t fd,uint8_t reg,uint8_t *val);


/**
​ * ​ ​ @brief​ : The function reads the timing Register
​ *
​ * ​ ​ Returns status indicating SUCCESS/FAIL Read Timing operation
 *    
​ * ​ ​ @param : fd-File Descriptor
 * ​ ​ @param : reg-Timing Register Address
 *   @param : *val-pointer to value read from register

​ */
enum Status read_timing_reg(uint8_t fd,uint8_t reg,uint8_t *val);



/**
​ * ​ ​ @brief​ : The function reads the Interrupt Register
​ *
​ * ​ ​ Returns status indicating SUCCESS/FAIL Read Interrupt operation
 *    
​ * ​ ​ @param : fd-File Descriptor
 * ​ ​ @param : reg-Interrupt Register Address
 *   @param : *val-pointer to value read from register

​ */
enum Status read_intr_reg(uint8_t fd,uint8_t reg,uint8_t *val);


/**
​ * ​ ​ @brief​ : The function reads the Control Register
​ *
​ * ​ ​ Returns status indicating SUCCESS/FAIL Read Control operation
 *    
​ * ​ ​ @param : fd-File Descriptor
 * ​ ​ @param : reg-Control Register Address
 *   @param : *val-pointer to value read from register

​ */
enum Status read_cntl_reg(uint8_t fd,uint8_t reg,uint8_t *val);


/**
​ * ​ ​ @brief​ : The function reads the ADC Register
​ *
​ * ​ ​ Returns status indicating SUCCESS/FAIL Read ADC operation
 *    
​ * ​ ​ @param : fd-File Descriptor
 * ​ ​ @param : reg-ADC(0/1) Register Address
 *   @param : *val-pointer to value read from register

​ */
enum Status read_adc_reg(uint8_t fd,uint8_t reg,uint8_t *val);


/**
​ * ​ ​ @brief​ : The function reads the Threshold Low Register
​ *
​ * ​ ​ Returns status indicating SUCCESS/FAIL Read Threshold Low  operation
 *    
​ * ​ ​ @param : fd-File Descriptor
 * ​ ​ @param : reg-Threshold Low  Register Address
 *   @param : *val-pointer to value read from register

​ */
enum Status read_threshlow_reg(uint8_t fd,uint8_t reg, uint16_t *val);


/**
​ * ​ ​ @brief​ : The function reads the Threshold High Register
​ *
​ * ​ ​ Returns status indicating SUCCESS/FAIL Read Threshold High  operation
 *    
​ * ​ ​ @param : fd-File Descriptor
 * ​ ​ @param : reg-Threshold High  Register Address
 *   @param : *val-pointer to value read from register

​ */
enum Status read_threshhigh_reg(uint8_t fd,uint8_t reg, uint16_t *val);


/**
​ * ​ ​ @brief​ : The function writes into control Register
​ *
​ * ​ ​ Returns status indicating SUCCESS/FAIL write control operation
 *    
​ * ​ ​ @param : fd-File Descriptor
 * ​ ​ @param : reg-Control Register Address
 *   @param : *val-pointer to value write into register

​ */
enum Status write_contrl_reg(uint8_t fd,uint8_t reg,uint8_t val);

/**
​ * ​ ​ @brief​ : The function writes into interrupt Register
​ *
​ * ​ ​ Returns status indicating SUCCESS/FAIL write interrupt operation
 *    
​ * ​ ​ @param : fd-File Descriptor
 * ​ ​ @param : reg-Interrupt Register Address
 *   @param : *val-pointer to value write into register

​ */
enum Status write_intr_reg(uint8_t fd,uint8_t reg,uint8_t val);

/**
​ * ​ ​ @brief​ : The function writes into timing Register
​ *
​ * ​ ​ Returns status indicating SUCCESS/FAIL write interrupt operation
 *    
​ * ​ ​ @param : fd-File Descriptor
 * ​ ​ @param : reg-Interrupt Register Address
 *   @param : *val-pointer to value write into register

​ */
enum Status write_timing_reg(uint8_t fd,uint8_t reg,uint8_t val);

/**
​ * ​ ​ @brief​ : The function writes into threshold low Register
​ *
​ * ​ ​ Returns status indicating SUCCESS/FAIL write threshold low operation
 *    
​ * ​ ​ @param : fd-File Descriptor
 * ​ ​ @param : reg-Threshold Low Register Address
 *   @param : *val-pointer to value write into register

​ */
enum Status write_threshlow_reg(uint8_t fd,uint8_t reg,uint16_t val);

/**
​ * ​ ​ @brief​ : The function writes into threshold high Register
​ *
​ * ​ ​ Returns status indicating SUCCESS/FAIL write threshold high operation
 *    
​ * ​ ​ @param : fd-File Descriptor
 * ​ ​ @param : reg-Threshold High Register Address
 *   @param : *val-pointer to value write into register

​ */
enum Status write_threshhigh_reg(uint8_t fd,uint8_t reg,uint16_t val);

/**
​ * ​ ​ @brief​ : The function reads all the Register
​ *
​ * ​ ​ Returns status indicating SUCCESS/FAIL of read register operation
 *    
​ * ​ ​ @param : fd-File Descriptor
 * ​ ​ @param : reg-Read Register Address
 *   @param : *val-pointer to value read from register

​ */
enum Status read_light_registers(uint8_t fd,uint8_t reg_option,uint16_t *val);


/**
​ * ​ ​ @brief​ : The function writes the Register
​ *
​ * ​ ​ Returns status indicating SUCCESS/FAIL of write register operation
 *    
​ * ​ ​ @param : fd-File Descriptor
 * ​ ​ @param : reg-Write Register Address
 *   @param : *val-pointer to value write into register
​ */
enum Status write_light_registers(uint8_t fd,uint8_t reg_option,uint16_t val);

/**
​ * ​ ​ @brief​ : The function configures integration values in timing Register
​ *
​ * ​ ​ Returns status indicating SUCCESS/FAIL of configure operation
 *    
​ * ​ ​ @param : fd-File Descriptor
 * ​ ​ @param : integration_option-the value needed to be set
​ */
enum Status config_integration_timing(uint8_t fd,uint8_t integration_option);

/**
​ * ​ ​ @brief​ : The function enables and disables the interrupt
​ *
​ * ​ ​ Returns status indicating SUCCESS/FAIL of enable/disable operation
 *    
​ * ​ ​ @param : fd-File Descriptor
 * ​ ​ @param : option-the value needed to be set
​ */
enum Status control_intr_reg(uint8_t fd,uint8_t intr_option);

/**
​ * ​ ​ @brief​ : The function returns night or day as message
​ *
​ * ​ ​ Returns "night" or "day" tag
 *    
​ * ​ ​ @param : none
​ */
char* night_or_day(void);


#endif
