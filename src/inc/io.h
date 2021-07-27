#ifndef _IO_H_
#define _IO_H_

#include <stm32f10x_gpio.h>

enum {
    IO_PORT_A,
    IO_PORT_B,
    IO_PORT_C,
    IO_PORT_D,
    IO_PORT_E,
    IO_PORT_F,
    IO_PORT_G,
};

enum {
    IO_MODE_INPUT = GPIO_Mode_IN_FLOATING,
    IO_MODE_OUTPUT = GPIO_Mode_Out_PP,
    IO_MODE_ALTERNATE = GPIO_Mode_AF_PP,
    IO_MODE_ANALOG = GPIO_Mode_AIN,
    IO_MODE_INPUT_PD = GPIO_Mode_IPD,
    IO_MODE_INPUT_PU = GPIO_Mode_IPU,
    IO_MODE_OUTPUT_OD = GPIO_Mode_Out_OD,
    IO_MODE_ALTERNATE_OD = GPIO_Mode_AF_OD,
};

typedef struct {
    int port;
    int pin;
} io_pin_t;

#define IO_PIN_LED_ON       0
#define IO_PIN_LED_OFF      1

enum {
    IO_PIN_LED,
    IO_PIN_TEST_OUT,
    IO_PIN_TEST_IN,

    IO_PIN_MAX
};

#ifdef _IO_PINS_

GPIO_TypeDef *gpio_ports[] = {
    GPIOA,
    GPIOB,
    GPIOC,
    GPIOD,
    GPIOE,
    GPIOF,
    GPIOG,
};

io_pin_t io_pins[IO_PIN_MAX] = {
    {.port = IO_PORT_C, .pin = 13}, 
    {.port = IO_PORT_B, .pin = 5}, 
    {.port = IO_PORT_A, .pin = 3}, 
};
#endif

void io_init(int id, int mode);
void io_write(int id, int val);
int io_read(int id);

#endif
