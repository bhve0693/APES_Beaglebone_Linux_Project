
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
#ifndef __FW_I2C_H_INCLUDED
#define __FW_I2C_H_INCLUDED

#include <stdio.h>


/*
​ * ​ ​ @brief​ : Enum for indicating I2C read/write status
​
 *   FAIL      : Data R/W failed
 *   SUCCESS   : Data R/W Success
 
​ */
enum Status {SUCCESS,FAIL} stat;

/**
​ * ​ ​ @brief​ : To initialize I2C Application driver
​ *
​ * ​ ​ Returns nothing
​ * ​ ​
​ */
void i2c_init(uint8_t addr,char *file);

/**
​ * ​ ​ @brief​ : Function to write to an I2C Device File
​ *
​ * ​ ​ Returns a status of type Status to indicate write Fail or Success
​ * ​ ​
​ */
enum Status i2c_write(uint8_t *val);

/**
​ * ​ ​ @brief​ : Function to read from an I2C Device File
​ *
​ * ​ ​ Returns a status of type Status to indicate read Fail or Success
​ * ​ ​
​ */
enum Status i2c_read(uint8_t *val);

#endif