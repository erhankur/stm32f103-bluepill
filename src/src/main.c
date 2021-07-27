#include <stdio.h>
#include <stdint.h>

#include "system.h"
#include "io.h"

void init(void)
{
    sys_tick_init();
    sys_io_init();

    io_write(IO_PIN_LED, 1);
    io_init(IO_PIN_LED, IO_MODE_OUTPUT);
}

void task_led(void)
{
    static enum {
        STATE_LED_OFF,
        STATE_LED_ON
    } state = STATE_LED_OFF; 

    static tick_type_t t0 = 0;
    tick_type_t t1 = sys_get_tick_count();

    switch (state) {
        case STATE_LED_OFF: 
            if (t1 >= t0 + 9 * CLOCK_PER_SECOND / 10) {
                t0 = t1;
                state = STATE_LED_ON;
                io_write(IO_PIN_LED, IO_PIN_LED_ON);
            }
            break;
        case STATE_LED_ON:
            if (t1 >= t0 + 1 * CLOCK_PER_SECOND / 10) {
                t0 = t1;
                state = STATE_LED_OFF;
                io_write(IO_PIN_LED, IO_PIN_LED_OFF);
            }
            break;
    }

}

int main()
{
    init();

    while (1) {
        task_led();
    }
}
