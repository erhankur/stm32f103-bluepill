#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "io.h"
#include "button.h"

button_pin_t s_button_pins[] = {
    {.io_index = IO_PIN_BUTTON_SET,  .current_state = 1, .active_state = 0, .debounce_counter = 0},
    {.io_index = IO_PIN_BUTTON_UP,   .current_state = 1, .active_state = 0, .debounce_counter = 0},
    {.io_index = IO_PIN_BUTTON_DOWN, .current_state = 1, .active_state = 0, .debounce_counter = 0},
};

#define N_BUTTONS sizeof(s_button_pins) / sizeof(button_pin_t)

unsigned int g_buttons_long_press[N_BUTTONS]; /* for syncronization. like binary semaphore */

#define MAX_DEBOUNCE_CNT    5 // 20ms scan time * 5 = 100

int s_start_scan = 0;

TaskHandle_t s_notify_task_handle;

static void button_read_one(int button_index)
{
    int val = io_read(s_button_pins[button_index].io_index);

    if (val != s_button_pins[button_index].current_state) {
        if (++s_button_pins[button_index].debounce_counter >= MAX_DEBOUNCE_CNT) {
            s_button_pins[button_index].current_state = val;
            s_button_pins[button_index].debounce_counter = 0;

            if (s_button_pins[button_index].current_state == s_button_pins[button_index].active_state) {
                /* activate the signal !!! */
                xTaskNotify(s_notify_task_handle, (1 << button_index), eSetValueWithOverwrite);
            }
        }
    } else {
        s_button_pins[button_index].debounce_counter = 0;
    }

#ifdef BUTTON_LONG_PRESS
    if (s_button_pins[button_index].current_state == s_button_pins[button_index].active_state) {
        //if (g_buttons_long_press[button_index] == 0) {
            if (++s_button_pins[button_index].active_counter >= BUTTON_LONG_PRESS_TIME) {
                s_button_pins[button_index].active_counter = 0;
                g_buttons_long_press[button_index] = 1;
                /* activate the signal !!! */
                xTaskNotify(s_notify_task_handle, 
                    (1 << button_index) | BUTTON_LONG, eSetValueWithOverwrite);
            }
        //}
    }
#endif
}

/* will be invoked in every tick ms */
void button_scan_all(void)
{
    if (!s_start_scan) {
        return;
    }

    for (int i = 0; i < N_BUTTONS; ++i) {
        button_read_one(i);
    }
}

void button_init(TaskHandle_t notify_task)
{
    for (int i = 0; i < N_BUTTONS; ++i) {
        io_init(s_button_pins[i].io_index, IO_MODE_INPUT_PU);
    }
    s_start_scan = 1;
    s_notify_task_handle = notify_task;
}