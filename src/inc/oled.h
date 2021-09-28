#ifndef _OLED_H_
#define _OLED_H_

#define OLED_SET_PIXEL      0
#define OLED_CLR_PIXEL      1
#define OLED_INV_PIXEL      2

void oled_start(int rotate);
void oled_clear_display(void);
void oled_set_pixel(int x, int y, int c, int update_now);
void oled_update_display(void);

#endif