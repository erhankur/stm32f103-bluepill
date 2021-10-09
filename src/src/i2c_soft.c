#include <stdio.h>
#include <stdint.h>

#include "system.h"
#include "io.h"
#include "i2c_soft.h"

enum { FALSE, TRUE };

static void sw_i2c_wait(void)
{
  //DelayUs(1);
}

static void sda_low(void)
{
    io_write(IO_PIN_I2C1_SDA, 0);
    sw_i2c_wait();
}

static void sda_high(void)
{
    io_write(IO_PIN_I2C1_SDA, 1);
    sw_i2c_wait();
}

static void scl_low(void)
{
    io_write(IO_PIN_I2C1_SCL, 0);
    sw_i2c_wait();
}

static void scl_high(void)
{
    io_write(IO_PIN_I2C1_SCL, 1);
    //while (!IO_Read(IOP_I2C_SCL)) ;     // Clock stretching
    sw_i2c_wait();
}

// SCL must be 0
// After leave from this function: SCL=0 SDA=1
static void sw_i2c_send_bit(int b)
{
    // 1) Data setup
    if (b)
        sda_high();
    else
        sda_low();

    // Data Setup Time
    // No need to wait here.
        
    // 2) Clock generation
    scl_high();
    scl_low();  

    sda_high();
}

static int sw_i2c_recv_bit(void)
{  
    // 1) Clock generation (active)
    scl_high();

    // 2) Sampling
    int b = io_read(IO_PIN_I2C1_SDA);

    // 3) Clock inactive
    scl_low();

    return b;
}

static int sw_i2c_send_byte(uint8_t val)
{  
    for (int i = 0; i < 8; ++i) {
        sw_i2c_send_bit(val & 0x80);
        val <<= 1;
    }

    return sw_i2c_recv_bit();  //ACK from slave
}

static uint8_t sw_i2c_recv_byte(void)
{
    uint8_t val = 0;

    for (int i = 0; i < 8; ++i) {
        val <<= 1;
        if (sw_i2c_recv_bit())
            val |= 1;        
    }

    return val;
}

void sw_i2c_init(void)
{
    io_write(IO_PIN_I2C1_SCL, 1);
    io_init(IO_PIN_I2C1_SCL, IO_MODE_OUTPUT_OD);

    io_write(IO_PIN_I2C1_SDA, 1);
    io_init(IO_PIN_I2C1_SDA, IO_MODE_OUTPUT_OD);
}

int sw_i2c_start(uint8_t ctl)
{
    scl_high();  
    sda_low();

    scl_low();
    sda_high();

    return sw_i2c_send_byte(ctl);
}

void sw_i2c_stop(void)
{
    sda_low();    // SCL=0, SDA=0
    scl_high();   // SCL=1, SDA=0
    sda_high();   // SCL=1, SDA=1 (idle)
}

int sw_i2c_write(uint8_t dev_addr, const void *buf, int len)
{
    const uint8_t *ptr = (const uint8_t *)buf;

    dev_addr &= 0xFE;       // 11111110 devAdr<0> = 0 (write)

    if (sw_i2c_start(dev_addr)) {
        sw_i2c_stop();
        return FALSE;
    }

    while (len--) {
        if (sw_i2c_send_byte(*ptr++)) {
            sw_i2c_stop();
            return FALSE;
        }
    }

    sw_i2c_stop();
    return TRUE;
}
int sw_i2c_read(uint8_t dev_addr, void *buf, int len)
{
    uint8_t *ptr = (uint8_t *)buf;
    
    if (len <= 0)
        return FALSE;
    
    if (sw_i2c_start(dev_addr | 1)) {
        sw_i2c_stop();
        return FALSE;
    }
    
    do {
        *ptr++ = sw_i2c_recv_byte();
        sw_i2c_send_bit(len == 1);    
    } while(--len);
    
    sw_i2c_stop();
    return TRUE;
}

int sw_i2c_writeA(uint8_t dev_addr, uint8_t reg_addr, const void *buf, int len)
{
    return TRUE;
}
int sw_i2c_readA(uint8_t dev_addr, uint8_t reg_addr, void *buf, int len)
{
    return TRUE;
}