#ifndef _LCD_H_
#define _LCD_H_

#include <stdint.h>

#define LCD_MODE_OFF        0x00
#define LCD_MODE_BLINK      0x01
#define LCD_MODE_CURSOR     0x02
#define LCD_MODE_ON         0x04

void lcd_init(void);
void lcd_display_mode(uint8_t mode);
void lcd_clear(void);
void lcd_set_cursor(uint8_t pos);
void lcd_put_char(char ch);

#endif
