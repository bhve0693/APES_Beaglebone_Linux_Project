
/**********************************************************************************************
​ * ​ ​ Redistribution,​ ​ modification​ ​ or​ ​ use​ ​ of​ ​ this​ ​ software​ ​ in​ ​ source​ ​ or​ ​ binary
​ * ​ ​ forms​ ​ is​ ​ permitted​ ​ as​ ​ long​ ​ as​ ​ the​ ​ files​ ​ maintain​ ​ this​ ​ copyright.​ ​ Users​ ​ are
​ * ​ ​ permitted​ ​ to​ ​ modify​ ​ this​ ​ and​ ​ use​ ​ it​ ​ to​ ​ learn​ ​ about​ ​ the​ ​ field​ ​ of​ ​ embedded
​ * ​ ​ software.​ ​ Alex​ ​ Fosdick​ ​ and​ ​ the​ ​ University​ ​ of​ ​ Colorado​ ​ are​ ​ not​ ​ liable​ ​ for
​ * ​ ​ any​ ​ misuse​ ​ of​ ​ this​ ​ material.
​ *
***********************************************************************************************/
/**
​ * ​ ​ @file​ ​ fw_i2c.h
​ * ​ ​ @brief​ ​ An​ ​ abstraction​ ​ for​ using th I2C Driver
​ * ​ ​ This​ ​ header​ ​ file​ ​ provides​ ​ an​ ​ abstraction​ ​ of​ ​a I2C Driver Application
 *   implementation
 *
​ * ​ ​ @author​ ​  Divya Sampath Kumar, Bhallaji Venkatesan
​ * ​ ​ @date​ ​    11/1/2017
​ *
​ */
#ifndef __I2C_TEMP_H_INCLUDED
#define __I2C_TEMP_H_INCLUDED

#include <stdio.h>

/**
​ * ​ ​ @brief​ : Returns the temperature read from temperature register
​ *
​ * ​ ​ Returns value of float data type
​ */
float temp_read(uint8_t fd,uint8_t choice);

/**
​ * ​ ​ @brief​ : Calculates the temperature in Celsius and Fahrenheit 
​ *
​ * ​ ​ Returns value of float data type
​ * ​ ​ @param : fd  - accepts I2C Device file descriptor
 *	 @param : addr -Resolution value
​ */
float read_tempsense(uint8_t fd,float resolution,uint8_t unit_choice);


/**
​ * ​ ​ @brief​ : Reads the temperature register
​ *
​ * ​ ​ Returns value of Status type-Fail/Success
​ * ​ ​ @param : fd  - accepts I2C Device file descriptor
 *	 @param : reg - Register address to read from
  *	 @param : val - Value read from file
​ */
enum Status read_temp_reg(uint8_t fd,uint8_t reg,uint16_t *val);

/**
​ * ​ ​ @brief​ : Reads the configuration register
​ *
​ * ​ ​ Returns value of Status type-Fail/Success
​ * ​ ​ @param : fd  - accepts I2C Device file descriptor
 *	 @param : reg - Register address to read from
  *	 @param : val - Value read from file
​ */
enum Status read_config_reg(uint8_t fd,uint8_t reg,uint16_t *val);


/**
​ * ​ ​ @brief​ : Reads the TLOW register
​ *
​ * ​ ​ Returns value of Status type-Fail/Success
​ * ​ ​ @param : fd  - accepts I2C Device file descriptor
 *	 @param : reg - Register address to read from
  *	 @param : val - Value read from file
​ */
enum Status read_tlow_reg(uint8_t fd,uint8_t reg,uint16_t *val);

/**
​ * ​ ​ @brief​ : Reads the THIGH register
​ *
​ * ​ ​ Returns value of Status type-Fail/Success
​ * ​ ​ @param : fd  - accepts I2C Device file descriptor
 *	 @param : reg - Register address to read from
  *	 @param : val - Value read from file
​ */
enum Status read_thigh_reg(uint8_t fd,uint8_t reg,uint16_t *val);

/**
​ * ​ ​ @brief​ : Writes into the Pointer register
​ *
​ * ​ ​ Returns value of Status type-Fail/Success
​ * ​ ​ @param : fd  - accepts I2C Device file descriptor
 *	 @param : reg - Register address to write value into
​ */
enum Status write_ptr_reg(uint8_t fd,uint8_t reg);

/**
​ * ​ ​ @brief​ : Writes into the Configuration register
​ *
​ * ​ ​ Returns value of Status type-Fail/Success
​ * ​ ​ @param : fd  - accepts I2C Device file descriptor
 *	 @param : reg - Register address to write value into
  *	 @param : value - Value to write into file
​ */
enum Status write_config_reg(uint8_t fd,uint8_t reg,uint16_t value);

/**
​ * ​ ​ @brief​ : Writes into the TLOW register
​ *
​ * ​ ​ Returns value of Status type-Fail/Success
​ * ​ ​ @param : fd  - accepts I2C Device file descriptor
 *	 @param : reg - Register address to write value into
  *	 @param : value - Value to write into file
​ */
enum Status write_tlow_reg(uint8_t fd,uint8_t reg,uint16_t value);

/**
​ * ​ ​ @brief​ : Writes into the THIGH register
​ *
​ * ​ ​ Returns value of Status type-Fail/Success
​ * ​ ​ @param : fd  - accepts I2C Device file descriptor
 *	 @param : reg - Register address to write value into
  *	 @param : value - Value to write into file
​ */
enum Status write_thigh_reg(uint8_t fd,uint8_t reg,uint16_t value);

/**
​ * ​ ​ @brief​ : Function to select which register to read from
​ *
​ * ​ ​ Returns value of Status type-Fail/Success
​ * ​ ​ @param : fd  - accepts I2C Device file descriptor
 *	 @param : reg_option - Register address to read from
  *	 @param : val - Value to read from file
​ */
enum Status read_temp_register(uint8_t fd,uint8_t reg_option,uint16_t *val);


/**
​ * ​ ​ @brief​ : Function to specifically configure conversion rate
​ *
​ * ​ ​ Returns value of Status type-Fail/Success
​ * ​ ​ @param : fd  - accepts I2C Device file descriptor
 *	 @param : option - To select conversion rate of 0.25Hz,1Hz,4Hz or 8Hzs
​ */
enum Status continuous_conversion_mode(uint8_t fd,uint8_t option);

/**
​ * ​ ​ @brief​ : Function to specifically configure shutdown mode
​ *
​ * ​ ​ Returns value of Status type-Fail/Success
​ * ​ ​ @param : fd  - accepts I2C Device file descriptor
 *	 @param : option - To select shutdown_enable or disable-1 indicates enable and 0 disable
​ */
enum Status shutdown_temp_mode(uint8_t fd,uint8_t option);


/**
​ * ​ ​ @brief​ : Function to select which register to write into
​ *
​ * ​ ​ Returns value of Status type-Fail/Success
​ * ​ ​ @param : fd  - accepts I2C Device file descriptor
 *	 @param : reg_option - Register address to write into
  *	 @param : val - Value to write into file
​ */
enum Status write_temp_register(uint8_t fd,uint8_t reg_option,uint16_t val);


/**
​ * ​ ​ @brief​ : Returns Celsius value
​ *
​ * ​ ​ Returns value of type float
​ * ​ ​ @param : temp_celsius  - Returns celsius temperature value
​ */
float celsius(float temp_celsius);

/**
​ * ​ ​ @brief​ : Returns Fahrenheit value
​ *
​ * ​ ​ Returns value of type float
​ * ​ ​ @param : temp_fahrenheit  - Returns fahrenheit temperature value
​ */
float fahrenheit(float temp_fahrenheit);

/**
​ * ​ ​ @brief​ : Returns Kelvin value
​ *
​ * ​ ​ Returns value of type float
​ * ​ ​ @param : temp_celsius  - Returns Kelvin temperature value
​ */
float kelvin(float temp_kelvin);

#endif
