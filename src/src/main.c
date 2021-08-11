#include <stdio.h>
#include <stdint.h>

#include "system.h"
#include "io.h"
#include "lcd.h"

void init(void)
{
    sys_tick_init();
    sys_io_init();

    io_write(IO_PIN_LED, 1);
    io_init(IO_PIN_LED, IO_MODE_OUTPUT);
    sys_console_init();
}

void task_led(void)
{
    static enum {
        INIT_LED_OFF,
        STATE_LED_OFF,
        INIT_LED_ON,
        STATE_LED_ON
    } state = INIT_LED_OFF;

    static tick_type_t t0 = 0;
    tick_type_t t1 = sys_get_tick_count();

    switch (state) {
    case STATE_LED_OFF:
        if (t1 < t0 + 9 * CLOCK_PER_SECOND / 10) {
            break;
        }
        state = INIT_LED_ON;
    /* fallthrough */
    case INIT_LED_ON:
        t0 = t1;
        io_write(IO_PIN_LED, IO_PIN_LED_ON);
        state = STATE_LED_ON;
        break;
    case STATE_LED_ON:
        if (t1 < t0 + 1 * CLOCK_PER_SECOND / 10) {
            break;
        }
        state = INIT_LED_OFF;
    /* fallthrough */
    case INIT_LED_OFF:
        t0 = t1;
        io_write(IO_PIN_LED, IO_PIN_LED_OFF);
        state = STATE_LED_OFF;
        break;
    }
}

void task_print(void)
{
    static uint32_t count = 0;

    printf("\nCNT:%10lu", ++count);
}

int main()
{
    init();

    printf("\rHello World!\nThis is ARM-CM3");

    while (1) {
        task_led();
        task_print();
    }
}
