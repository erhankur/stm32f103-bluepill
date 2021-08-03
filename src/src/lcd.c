#include <stdio.h>
#include <stdint.h>

#include "system.h"
#include "io.h"
#include "lcd.h"

static void lcd_send_nibble(uint8_t ch)
{
    io_write(IO_PIN_LCD_D4, (ch & 0x01) != 0);
    io_write(IO_PIN_LCD_D5, (ch & 0x02) != 0);
    io_write(IO_PIN_LCD_D6, (ch & 0x04) != 0);
    io_write(IO_PIN_LCD_D7, (ch & 0x08) != 0);

    io_write(IO_PIN_LCD_E, 1);
    delay_us(3);
    io_write(IO_PIN_LCD_E, 0);
    delay_us(100);
}

void lcd_send_cmd(uint8_t cmd)
{
    io_write(IO_PIN_LCD_RS, 0);
    lcd_send_nibble(cmd >> 4);
    lcd_send_nibble(cmd);
}

void lcd_send_char(uint8_t ch)
{
    io_write(IO_PIN_LCD_RS, 1);
    lcd_send_nibble(ch >> 4);
    lcd_send_nibble(ch);
}

void lcd_clear(void)
{
    lcd_send_cmd(0x01); /* clear display */
    delay_ms(5);
}

void lcd_display_mode(uint8_t mode)
{
    lcd_send_cmd(0x08 | mode); 
}

void lcd_set_cursor(uint8_t pos)
{
    lcd_send_cmd(0x80 | pos); 
}

void lcd_init(void)
{
    io_write(IO_PIN_LCD_RS, 0);
    io_init(IO_PIN_LCD_RS, IO_MODE_OUTPUT);
    io_write(IO_PIN_LCD_E, 0);
    io_init(IO_PIN_LCD_E, IO_MODE_OUTPUT);
    io_init(IO_PIN_LCD_D4, IO_MODE_OUTPUT);
    io_init(IO_PIN_LCD_D5, IO_MODE_OUTPUT);
    io_init(IO_PIN_LCD_D6, IO_MODE_OUTPUT);
    io_init(IO_PIN_LCD_D7, IO_MODE_OUTPUT);

    delay_ms(100);

    /* configure 4 bit mode */
    lcd_send_nibble(0x03);
    delay_ms(5);
    lcd_send_cmd(0x32);
    lcd_send_cmd(0x28); /* 2 lines small font */
    lcd_display_mode(LCD_MODE_OFF);
    lcd_clear();
    lcd_send_cmd(0x06);
    lcd_display_mode(LCD_MODE_ON | LCD_MODE_BLINK);
}