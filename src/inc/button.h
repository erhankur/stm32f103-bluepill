#ifndef _BUTTON_H_
#define _BUTTON_H_

#define BUTTON_LONG_PRESS
#define BUTTON_LONG_PRESS_TIME  2000  /* ms */
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
    BUTTON_SET,
    BUTTON_UP,
    BUTTON_DOWN,
};

void button_init(void);
void button_scan_all(void);

extern unsigned int g_buttons[];
extern unsigned int g_buttons_long_press[];

#endif
