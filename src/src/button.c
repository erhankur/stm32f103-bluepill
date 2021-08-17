#include "io.h"
#include "button.h"

button_pin_t s_button_pins[] = {
    {.io_index = IO_PIN_BUTTON_SET,  .current_state = 1, .active_state = 0, .debounce_counter = 0},
    {.io_index = IO_PIN_BUTTON_UP,   .current_state = 1, .active_state = 0, .debounce_counter = 0},
    {.io_index = IO_PIN_BUTTON_DOWN, .current_state = 1, .active_state = 0, .debounce_counter = 0},
};

#define N_BUTTONS sizeof(s_button_pins) / sizeof(button_pin_t)

unsigned int g_buttons[N_BUTTONS]; /* for syncronization */

#define MAX_DEBOUNCE_CNT    10

int s_start_scan = 0;

static void button_read_one(int button_index)
{
    if (!s_start_scan) {
        return;
    }

    int val = io_read(s_button_pins[button_index].io_index);

    if (val != s_button_pins[button_index].current_state) {
        if (++s_button_pins[button_index].debounce_counter >= MAX_DEBOUNCE_CNT) {
            s_button_pins[button_index].current_state = val;
            s_button_pins[button_index].debounce_counter = 0;

            if (s_button_pins[button_index].current_state == s_button_pins[button_index].active_state) {
                /* activate the signal !!! */
                g_buttons[button_index] = 1;
            }
        }
    } else {
        s_button_pins[button_index].debounce_counter = 0;
    }
}

/* will be invoked in every tick ms */
void button_scan_all(void)
{
    for (int i = 0; i < N_BUTTONS; ++i) {
        button_read_one(i);
    }
}

void button_init(void)
{
    for (int i = 0; i < N_BUTTONS; ++i) {
        io_init(s_button_pins[i].io_index, IO_MODE_INPUT_PU);
    }
    s_start_scan = 1;
}