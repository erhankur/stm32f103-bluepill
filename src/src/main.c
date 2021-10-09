#include <stdio.h>
#include <stdint.h>

#include "system.h"
#include "io.h"
#include "oled.h"

void init(void)
{
    sys_tick_init();
    sys_io_init();

    io_write(IO_PIN_LED, 1);
    io_init(IO_PIN_LED, IO_MODE_OUTPUT);
    oled_start(0);
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

void display_unique_id(void)
{
    const uint32_t *pID = (const uint32_t *)0x1FFFF7E8;

    printf("%08lX\n", pID[2]);
    printf("%08lX\n", pID[1]);
    printf("%08lX\n", pID[0]);
}

void task_print(void)
{
    static unsigned count = 0;
    int row, col;

    oled_get_cursor(&row, &col);
    oled_get_cursor(0, 0);
    printf("CNT:%10u", ++count);
    oled_set_cursor(row, col);
}

void task_display_clock(void)
{
  static clock_t t0, t1;
  int row, col;
  
  t1 = sys_get_tick_count();
  
  if (t1 != t0) {
    oled_get_cursor(&row, &col);
    oled_get_cursor(0, 0);
    printf("Clock:%10lu\r", t1);
    oled_set_cursor(row, col);
    t0 = t1;
  }
}

int main()
{
    init();

    display_unique_id();

    while (1) {
        task_led();
        task_display_clock();
        task_print();
    }
}
