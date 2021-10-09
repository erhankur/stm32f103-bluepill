#ifndef _I2C_H_
#define _I2C_H_

#include <stdio.h>
#include <stdint.h>

#define I2C_WRITE   0
#define I2C_READ    1

enum {
    I2C_1,
    I2C_2
};

void i2c_init(int i2c_num, int baud);
int i2c_write(int i2c_num, uint8_t addr, const void *buff, size_t len);
int i2c_read(int i2c_num, uint8_t addr, void *buff, size_t len);

#endif 
