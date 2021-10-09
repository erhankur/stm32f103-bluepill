#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "system.h"
#include "spi.h"
#include "io.h"
#include "i2c.h"
#include "e24.h"

#define E24XX_I2C_ADDR  0xA0
#define E24XX_I2C_NUM   I2C_1

#define ADDR_SIZE_MAX   4
#define PAGE_SIZE_MAX   512

static int s_addr_size = 4;
static int s_page_size = 512;
static uint8_t s_i2c_buff[PAGE_SIZE_MAX + ADDR_SIZE_MAX];

enum {FALSE, TRUE};

void e24_config(int addr_size, int page_size)
{
    if (addr_size <= ADDR_SIZE_MAX) {
        s_addr_size = addr_size;
    }
    if (s_page_size <= PAGE_SIZE_MAX) {
        s_page_size = page_size;
    }
}

int e24_write_page(int chip_addr, uint32_t offset, const void *buff, size_t len)
{
    if (len > s_page_size) {
        return FALSE;
    }

    chip_addr <<= 1;
    for (int i = s_addr_size - 1; i >= 0; --i) {
        s_i2c_buff[i] = (uint8_t)offset; 
        offset >>= 8;
    }
    memcpy(s_i2c_buff + s_addr_size, buff, len);
    if (!i2c_write(E24XX_I2C_NUM, E24XX_I2C_ADDR + chip_addr, s_i2c_buff, len + s_addr_size)) {
        return FALSE;
    }

    //delay_ms(3); or ack polling to wait write operation
    while (!i2c_write(E24XX_I2C_NUM, E24XX_I2C_ADDR + chip_addr, NULL, 0));

    return TRUE;
}

int e24_read(int chip_addr, uint32_t offset, void *buff, size_t len)
{
    chip_addr <<= 1;

    for (int i = s_addr_size - 1; i >= 0; --i) {
        s_i2c_buff[i] = (uint8_t)offset; 
        offset >>= 8;
    }

    if (!i2c_write(E24XX_I2C_NUM, E24XX_I2C_ADDR + chip_addr, s_i2c_buff, s_addr_size)) {
        return FALSE;
    }

    if (!i2c_read(E24XX_I2C_NUM, E24XX_I2C_ADDR + chip_addr, buff, len)) {
        return FALSE;
    }

    return TRUE;
}

int e24_write(int chip_addr, uint32_t offset, const void *buff, size_t len)
{
    int block_size = s_page_size - (offset % s_page_size);
    const uint8_t *ptr = (const uint8_t *)buff;

    while (len > 0) {
        if (block_size > len) {
            block_size = len;
        }
        if (!e24_write_page(chip_addr, offset, ptr, block_size)) {
            return FALSE;
        }
        offset += block_size;
        ptr += block_size;
        len -= block_size;
        block_size = s_page_size;
    }
    
    return TRUE;
}