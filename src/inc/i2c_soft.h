#ifndef _I2C_SW_
#define _I2C_SW_

#include <stdint.h>

void sw_i2c_init(void);
int sw_i2c_start(uint8_t ctl);
void sw_i2c_stop(void);

int sw_i2c_write(uint8_t dev_addr, const void *buf, int len);
int sw_i2c_read(uint8_t dev_addr, void *buf, int len);

int sw_i2c_writeA(uint8_t dev_addr, uint8_t reg_addr, const void *buf, int len);
int sw_i2c_readA(uint8_t dev_addr, uint8_t reg_addr, void *buf, int len);

#endif