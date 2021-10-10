#ifndef _BUTTON_H_
#define _BUTTON_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define BUTTON_LONG_PRESS
#define BUTTON_LONG_PRESS_TIME  100  /* 20ms scan time * 100 */

typedef struct {
    int io_index;
    int current_state;
    int active_state;
    int debounce_counter;
#ifdef BUTTON_LONG_PRESS
    int active_counter;
#endif
} button_pin_t;

enum {
    BUTTON_SET = (1 << 0),
    BUTTON_UP  = (1 << 1),
    BUTTON_DOWN = (1 << 2),
    BUTTON_LONG = 0x80000000,
};

void button_init(TaskHandle_t notify_task);
void button_scan_all(void);

extern unsigned int g_buttons_long_press[];

//todo notification

#endif