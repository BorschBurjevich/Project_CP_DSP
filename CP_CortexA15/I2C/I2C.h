/*
 * I2C.h
 *
 *  Created on: 26 мая 2020 г.
 *      Author: Your Majesty
 */

#ifndef I2C_I2C_H_
#define I2C_I2C_H_

#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include <stdio.h>
#include <ti/csl/hw_types.h>
#include <ti/drv/i2c/soc/I2C_soc.h>
//#include <ti/drv/i2c/test/eeprom_read/src/I2C_log.h>
#include "delay/delay.h"


//обяъвление функций
I2C_Handle  I2Cx_open(uint32_t port_num);
void        I2Cx_close(I2C_Handle h);
int16_t     I2Cx_transfer(I2C_Handle h, uint32_t slave_addr, uint32_t data);
void        I2Cx_mux(void);
uint16_t sensor_drv(I2C_Handle h, uint32_t slave_addr);
#endif /* I2C_I2C_H_ */
