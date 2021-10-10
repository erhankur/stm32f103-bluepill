#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "stm32f10x_i2c.h"
#include "system.h"
#include "io.h"
#include "i2c.h"

#define I2C_TIMEOUT     1000
#define I2C_WRITE       0
#define I2C_READ        1

I2C_TypeDef *s_pi2c[] = {
    I2C1,
    I2C2
};

enum { ACK, NACK };

void i2c_init(int i2c_num, int baud)
{
    I2C_InitTypeDef i2cInit;
    I2C_TypeDef *pi2c = s_pi2c[i2c_num];

    if (i2c_num == I2C_1) {
        io_init(IO_PIN_I2C1_SCL, IO_MODE_ALTERNATE_OD);
        io_init(IO_PIN_I2C1_SDA, IO_MODE_ALTERNATE_OD);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    } else if (i2c_num == I2C_2) {
        io_init(IO_PIN_I2C2_SCL, IO_MODE_ALTERNATE_OD);
        io_init(IO_PIN_I2C2_SDA, IO_MODE_ALTERNATE_OD);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
    } else {
        return;
    }

    i2cInit.I2C_Ack = I2C_Ack_Enable;
    i2cInit.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    i2cInit.I2C_ClockSpeed = baud;
    i2cInit.I2C_DutyCycle = I2C_DutyCycle_2;
    i2cInit.I2C_Mode = I2C_Mode_I2C;
    i2cInit.I2C_OwnAddress1 = 0x55;

    I2C_Init(pi2c, &i2cInit);
    I2C_Cmd(pi2c, ENABLE);
}

static int i2c_event(int i2c_num, int event)
{
    unsigned long tout = I2C_TIMEOUT;

    I2C_TypeDef *pi2c = s_pi2c[i2c_num];

    do {
        if (I2C_CheckEvent(pi2c, event) == SUCCESS) {
            break;
        }
    } while (--tout);

    return (tout != 0);
}

static void i2c_stop(int i2c_num)
{
    I2C_TypeDef *pi2c = s_pi2c[i2c_num];

    I2C_GenerateSTOP(pi2c, ENABLE);
}

int i2c_send(int i2c_num, uint8_t data)
{
    I2C_TypeDef *pi2c = s_pi2c[i2c_num];

    I2C_SendData(pi2c, data);  //9 clock  = 8 data + 1 ACK

    return i2c_event(i2c_num, I2C_EVENT_MASTER_BYTE_TRANSMITTED);
}

void i2c_send_ack(int i2c_num, int ack)
{
    I2C_TypeDef *pi2c = s_pi2c[i2c_num];

    I2C_AcknowledgeConfig(pi2c, ack == ACK ? ENABLE : DISABLE);
}

int i2c_recv(int i2c_num, uint8_t *pdata)
{
    I2C_TypeDef *pi2c = s_pi2c[i2c_num];

    if (!i2c_event(i2c_num, I2C_EVENT_MASTER_BYTE_RECEIVED)) {
        return false;
    }

    *pdata = I2C_ReceiveData(pi2c); //8 clock = 8 data

    return true;
}

int i2c_start(int i2c_num, uint8_t ctl)
{
    I2C_TypeDef *pi2c = s_pi2c[i2c_num];

    I2C_GenerateSTART(pi2c, ENABLE);

    if (!i2c_event(i2c_num, I2C_EVENT_MASTER_MODE_SELECT)) {
        return false;
    }

    I2C_SendData(pi2c, ctl);

    int stat = i2c_event(i2c_num, ctl & 1 ?
                         I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED :
                         I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);

    if (stat != SUCCESS) {
        I2C_GenerateSTOP(pi2c, ENABLE);
        return false;
    }

    return true;
}

int i2c_write(int i2c_num, uint8_t addr, const void *buff, size_t len)
{
    const uint8_t *ptr = (const uint8_t *)buff;

    addr &= 0xFE;

    if (!i2c_start(i2c_num, addr | I2C_WRITE)) {
        return false;
    }

    while (len) {
        if (!i2c_send(i2c_num, *ptr++)) {
            break;
        }
        --len;
    }

    i2c_stop(i2c_num);

    return !len;
}

int i2c_read(int i2c_num, uint8_t addr, void *buff, size_t len)
{
    uint8_t *ptr = (uint8_t *)buff;

    if (!len) {
        return false;
    }

    addr &= 0xFE;

    if (i2c_start(i2c_num, addr | I2C_READ) != true) {
        return false;
    }

    do {
        i2c_send_ack(i2c_num, len == 1);  //if this is last byte send 1 as ack
        if (!i2c_recv(i2c_num, ptr++)) {
            break;
        }
    } while (--len);

    return !len;
}

