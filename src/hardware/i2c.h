/**
  * @author: hxdyxd@gmail.com
  * @date: 2018 03 26
  */

#ifndef __I2C_H__
#define __I2C_H__

#include <stdint.h>

void i2c_init(void);

int i2c_write_byte(uint8_t REG_Address, uint8_t REG_data);

int8_t i2c_write(uint8_t addr, uint8_t reg, uint8_t len, uint8_t * data);
int8_t i2c_read(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf);

#endif
