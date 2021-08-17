#ifndef _BUTTON_H_
#define _BUTTON_H_

typedef struct {
    int io_index;
    int current_state;
    int active_state;
    int debounce_counter;
} button_pin_t;

enum {
    BUTTON_SET,
    BUTTON_UP,
    BUTTON_DOWN,
};

void button_init(void);
void button_scan_all(void);

extern unsigned int g_buttons[];

#endif
