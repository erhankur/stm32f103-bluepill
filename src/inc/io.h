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

// #define IO_OLED_RES
// #define IO_OLED_CS

#define IO_OLED_VDD
#define IO_OLED_GND

enum {
    IO_PIN_LED,

    /* UART pins */
    IO_PIN_UART3_RX,
    IO_PIN_UART3_TX,

    /* I2C */
    IO_PIN_I2C1_SCL,
    IO_PIN_I2C1_SDA,
    IO_PIN_I2C2_SCL,
    IO_PIN_I2C2_SDA,

#ifdef IO_OLED_VDD
    IO_PIN_OLED_VDD,
#endif
#ifdef IO_OLED_GND
    IO_PIN_OLED_GND,
#endif

    /* Matrix Keyboard */
    IO_PIN_KEY_ROW0,
    IO_PIN_KEY_ROW1,
    IO_PIN_KEY_ROW2,
    IO_PIN_KEY_ROW3,
    IO_PIN_KEY_COL0,
    IO_PIN_KEY_COL1,
    IO_PIN_KEY_COL2,

    /* Push buttons */
    IO_PIN_BUTTON_SET,
    IO_PIN_BUTTON_UP,
    IO_PIN_BUTTON_DOWN,

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
    {.port = IO_PORT_C, .pin = 13},  /* LED */

    /* Uart */
    {.port = IO_PORT_B, .pin = 11},  /* UART3_RX */
    {.port = IO_PORT_B, .pin = 10},  /* UART3_TX */

    /* I2C */
    {.port = IO_PORT_B, .pin = 6},      /* I2C1_SCL */
    {.port = IO_PORT_B, .pin = 7},      /* I2C1_SDA */
    {.port = IO_PORT_B, .pin = 10},     /* I2C2_SCL */
    {.port = IO_PORT_B, .pin = 11},     /* I2C2_SDA */

    /* OLED */
#ifdef IO_OLED_VDD
    {.port = IO_PORT_B, .pin = 4},
#endif
#ifdef IO_OLED_GND
    {.port = IO_PORT_B, .pin = 5},
#endif

    /* Matrix Keyboard */
    {.port = IO_PORT_A, .pin = 10},     /* ROW0 */
    {.port = IO_PORT_A, .pin = 9},      /* ROW1 */
    {.port = IO_PORT_A, .pin = 8},      /* ROW2 */
    {.port = IO_PORT_B, .pin = 15},     /* ROW3 */
    {.port = IO_PORT_B, .pin = 12},     /* COL0 */
    {.port = IO_PORT_B, .pin = 13},     /* COL1 */
    {.port = IO_PORT_B, .pin = 14},     /* COL2 */

    /* Push buttons */
    {.port = IO_PORT_A, .pin = 5},      /* SET */
    {.port = IO_PORT_A, .pin = 6},      /* UP */
    {.port = IO_PORT_A, .pin = 7},      /* DOWN */

};
#endif

void io_init(int id, int mode);
void io_write(int id, int val);
int io_read(int id);

#endif