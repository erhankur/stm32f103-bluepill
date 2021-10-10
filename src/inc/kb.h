#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#define KB_NROWS   4
#define KB_NCOLS   3

#define KB_INVALIDKEY   (-1)

extern volatile int g_key;
extern int g_max_debounce;

void kb_init(void);
void kb_scan(void);

int _kbhit(void);
int _getch(void);

#endif