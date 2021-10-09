#include <stdio.h>
#include <stdint.h>

#include "stm32f10x.h"

#define _IO_PINS_
#include "io.h"

void io_init(int id, int mode)
{
    GPIO_InitTypeDef io_init = {
        .GPIO_Pin = (1 << io_pins[id].pin),
        .GPIO_Mode = (GPIOMode_TypeDef)mode,
        .GPIO_Speed = GPIO_Speed_50MHz
    };

    GPIO_Init(gpio_ports[io_pins[id].port], &io_init);
}

void io_write(int id, int val)
{
    if (val) {
        gpio_ports[io_pins[id].port]->BSRR = (1 << io_pins[id].pin);
    } else {
        gpio_ports[io_pins[id].port]->BRR = (1 << io_pins[id].pin);
    }
}

int io_read(int id)
{
    return (gpio_ports[io_pins[id].port]->IDR & (1 << io_pins[id].pin)) != 0;
}