
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

/**
​ * ​ ​ @brief​ : This function sets the control register to 0x03 to power on the sensor
​ *
​ * ​ ​ Returns status Success/fail to indicate light sensor on status
 *	 @param : fd  - accepts I2C Device file descriptor
​ */
enum Status light_sensor_switchon(uint8_t fd);

/**
​ * ​ ​ @brief​ : The function reads ADC0 for measured valuee(Visible+IR)
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
double light_read(void);

#endif
