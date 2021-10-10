#ifndef _OLED_H_
#define _OLED_H_

#include <stdint.h>
#include <stdbool.h>

#define OLED_SET_PIXEL      0
#define OLED_CLR_PIXEL      1
#define OLED_INV_PIXEL      2

#define FONT_SMALL          0   /* 5x8  8 lines 21 cols */
#define FONT_LARGE          1   /* 7x16 4 lines 16 cols */

void oled_start(int rotate);
void oled_clear_display(void);
void oled_set_font(int font);
void oled_rotate(int rotate);
void oled_set_pixel(int x, int y, int c, bool update);
int oled_get_pixel(int x, int y);
void oled_scrool(int nlines);
void oled_set_cursor(int row, int col);
void oled_get_cursor(int *prow, int *pcol);
void oled_draw_bmp(const uint8_t *bmp);
void oled_line(int x0, int y0, int x1, int y1, int c);
void oled_draw_sin(void);
void oled_circle(int x, int y, int r, int c);
void oled_return(void);
void oled_new_line(void);
void oled_put_char(char c);
void oled_putch(char c);
void oled_printf(const char *fmt, ...);
void oled_test(void);
#endif