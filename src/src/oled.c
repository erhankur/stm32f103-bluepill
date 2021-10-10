#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "lwprintf.h"

#include "system.h"
#include "i2c.h"
#include "io.h"
#include "oled.h"
#include "font_5x8.h"
#include "font_7x16.h"

#define I2C_PORT        I2C_1
#define I2C_ADR_OLED    (0x3C << 1)

#define N_PAGES         8
#define N_SEGMENT       128
#define OLED_MAX_X      128
#define OLED_MAX_Y      64

static const uint8_t *s_font = g_font_5x8;
static uint8_t s_active_font = FONT_SMALL;
static uint8_t s_font_w = 5;    //bit
static uint8_t s_font_h = 1;    //byte

#define N_COLS      (N_SEGMENT / (s_font_w + 1))
#define N_ROWS      (N_SEGMENT / s_font_h)

static uint8_t s_active_row, s_active_col;
static uint8_t s_active_page, s_active_segment;
static uint8_t s_frame_buff[N_PAGES * N_SEGMENT];
static int cache_len;
static uint8_t cache[256];

static void oled_send_cmd(uint8_t cmd);
static void oled_send_data(uint8_t data);
static void oled_begin_command(void);
static void oled_begin_data(void);
static void oled_set_command(uint8_t val);
static void oled_set_data(uint8_t val);
static void oled_flush_cache(void);
static void oled_set_page(uint8_t page);
static void oled_set_segment(uint8_t seg);
static void oled_fill_page(uint8_t page, uint8_t ch);
static void oled_fill_display(uint8_t ch);

static void oled_send_cmd(uint8_t cmd)
{
    uint8_t buf[2] = {0x00, cmd};

    i2c_write(I2C_PORT, I2C_ADR_OLED, buf, 2);
}

static void oled_send_data(uint8_t data)
{
    uint8_t buf[2] = {0x40, data};

    i2c_write(I2C_PORT, I2C_ADR_OLED, buf, 2);

    s_frame_buff[s_active_page * N_SEGMENT + s_active_segment] = data;

    if (++s_active_segment >= N_SEGMENT) {
        s_active_segment = 0;
    }
}

static void oled_begin_command(void)
{
    cache[0] = 0x00; // Komut
    cache_len = 1;
}

static void oled_begin_data(void)
{
    cache[0] = 0x40;  // Data
    cache_len = 1;
}

static void oled_set_command(uint8_t val)
{
    cache[cache_len++] = val;
}

static void oled_set_data(uint8_t val)
{
    cache[cache_len++] = val;

    s_frame_buff[((unsigned)s_active_page << 7) + s_active_segment] = val;
    if (++s_active_segment >= N_SEGMENT) {
        s_active_segment = 0;
    }
}

static void oled_flush_cache(void)
{
    i2c_write(I2C_PORT, I2C_ADR_OLED, cache, cache_len);
}

static void oled_set_page(uint8_t page)
{
    page &= (N_PAGES - 1);
    s_active_page = page;
    oled_send_cmd(0xB0 | s_active_page);
}

static void oled_set_segment(uint8_t seg)
{
    seg &= (N_SEGMENT - 1);
    s_active_segment = seg;
    oled_begin_command();
    oled_set_command(s_active_segment & 0x0F);
    oled_set_command(0x10 | (s_active_segment >> 4));
    oled_flush_cache();
}

static void oled_update_display(void)
{
    for (int i = 0, page = 0; page < N_PAGES; ++page) {
        oled_set_page(page);
        oled_set_segment(0);

        oled_begin_data();
        for (int seg = 0; seg < N_SEGMENT; ++seg) {
            oled_set_data(s_frame_buff[i++]);
        }
        oled_flush_cache();
    }
}

/* can be used to clear line */
static void oled_fill_page(uint8_t page, uint8_t ch)
{
    oled_set_page(page);
    oled_set_segment(0);

    oled_begin_data();
    for (int i = 0; i < N_SEGMENT; ++i) {
        oled_set_data(ch);
    }
    oled_flush_cache();
}

static void oled_fill_display(uint8_t ch)
{
    for (int i = 0; i < N_PAGES; ++i) {
        oled_fill_page(i, ch);
    }
}

int oled_get_font(int font)
{
    return s_active_font;
}

void oled_set_font(int font)
{
    switch (font) {
    case FONT_SMALL:
        s_font_w = 5;
        s_font_h = 1;
        s_font = g_font_5x8;
        s_active_font = FONT_SMALL;
        break;

    case FONT_LARGE:
        s_font_w = 7;
        s_font_h = 2;
        s_font = g_font_7x16;
        s_active_font = FONT_LARGE;
        break;
    }
}

void oled_rotate(int rotate)
{
    uint8_t remap = 0xA1, scan = 0xC8;

    if (rotate) {
        remap = 0xA0;
        scan = 0xC0;
    }

    oled_begin_command();
    oled_set_command(remap);
    oled_set_command(scan);
    oled_flush_cache();
}

void oled_start(int rotate)
{
    static bool oled_started = false;

    if (oled_started) {
        return;
    }

    i2c_init(I2C_PORT, 100000);

#ifdef IO_OLED_GND
    io_write(IO_PIN_OLED_GND, 0);
    io_init(IO_PIN_OLED_GND, IO_MODE_OUTPUT);
#endif  

#ifdef IO_OLED_VDD
    io_write(IO_PIN_OLED_VDD, 1);
    io_init(IO_PIN_OLED_VDD, IO_MODE_OUTPUT);
#endif 

    delay_ms(80);

    oled_begin_command();

    oled_set_command(0xAE); // Set display OFF

    oled_set_command(0xD4); // Set Display Clock Divide Ratio / OSC Frequency
    oled_set_command(0x80); // Display Clock Divide Ratio / OSC Frequency

    oled_set_command(0xA8); // Set Multiplex Ratio
    oled_set_command(0x3F); // Multiplex Ratio for 128x64 (64-1)

    oled_set_command(0xD3); // Set Display Offset
    oled_set_command(0x00); // Display Offset

    oled_set_command(0x40); // Set Display Start Line

    oled_set_command(0x8D); // Set Charge Pump
    oled_set_command(0x14); // Charge Pump (0x10 External, 0x14 Internal DC/DC)

    oled_set_command(0xDA); // Set COM Hardware Configuration
    oled_set_command(0x12); // COM Hardware Configuration

    oled_set_command(0x81); // Set Contrast
    oled_set_command(0x80); // Contrast

    oled_set_command(0xD9); // Set Pre-Charge Period
    oled_set_command(0xF1); // Set Pre-Charge Period (0x22 External, 0xF1 Internal)

    oled_set_command(0xDB); // Set VCOMH Deselect Level
    oled_set_command(0x40); // VCOMH Deselect Level

    oled_set_command(0xA4); // Enable display outputs according to the GDDRAM contents //0xA5 for test all pixels
    oled_set_command(0xA6); // Set display not inverted

    oled_flush_cache();

    oled_rotate(rotate);
    oled_clear_display();
    oled_send_cmd(0xAF);    // Set display On

    oled_set_font(FONT_LARGE);

    oled_started = true;
}

void oled_clear_display(void)
{
    oled_fill_display(0x00);
}

void oled_set_pixel(int x, int y, int c, bool update)
{
    x &= (OLED_MAX_X - 1);
    y &= (OLED_MAX_Y - 1);

    uint8_t page = y >> 3;
    uint8_t indx = y & 7;

    uint8_t val = s_frame_buff[page * N_SEGMENT + x];

    switch (c) {
    case OLED_SET_PIXEL:
        val |= (1 << indx);
        break;
    case OLED_CLR_PIXEL:
        val &= ~(1 << indx);
        break;
    case OLED_INV_PIXEL:
        val ^= (1 << indx);
        break;
    }

    if (update) {
        oled_set_page(page);
        oled_set_segment(x);
        oled_send_data(val);
    } else {
        s_frame_buff[page * N_SEGMENT + x] = val;
    }
}

int oled_get_pixel(int x, int y)
{
    x &= OLED_MAX_X - 1;
    y &= OLED_MAX_Y - 1;

    uint8_t page = y >> 3;
    uint8_t indx = y & 7;

    uint8_t val = s_frame_buff[page * N_SEGMENT + x];

    return (val & (1 << indx)) != 0;
}

void oled_scrool(int nlines)
{
    int dest, src;

    src = nlines * N_SEGMENT;

    for (dest = 0; dest < (N_PAGES - nlines) * N_SEGMENT; ++dest) {
        s_frame_buff[dest] = s_frame_buff[src++];
    }

    for (; dest < N_PAGES * N_SEGMENT; ++dest) {
        s_frame_buff[dest] = 0;
    }
}

void oled_set_cursor(int row, int col)
{
    s_active_row = row;
    s_active_col = col;
}

void oled_get_cursor(int *prow, int *pcol)
{
    *prow = s_active_row;
    *pcol = s_active_col;
}

void oled_draw_bmp(const uint8_t *bmp)
{
    for (int i = 0; i < sizeof(s_frame_buff); ++i) {
        s_frame_buff[i] = bmp[i];
    }
    oled_update_display();
}

#define abs(a)      (((a) > 0) ? (a) : -(a))

//y = mx + c
void oled_line(int x0, int y0, int x1, int y1, int c)
{
    int steep, t ;
    int deltax, deltay, error;
    int x, y;
    int ystep;

    steep = abs(y1 - y0) > abs(x1 - x0);

    if (steep) {
        // swap x and y
        t = x0; x0 = y0; y0 = t;
        t = x1; x1 = y1; y1 = t;
    }

    if (x0 > x1) {
        // swap ends
        t = x0; x0 = x1; x1 = t;
        t = y0; y0 = y1; y1 = t;
    }

    deltax = x1 - x0;
    deltay = abs(y1 - y0);
    error = 0;
    y = y0;

    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (x = x0; x < x1; x++) {
        if (steep) {
            oled_set_pixel(y, x, c, false);
        } else {
            oled_set_pixel(x, y, c, false);
        }

        error += deltay;
        if ((error << 1) >= deltax) {
            y += ystep;
            error -= deltax;
        }
    }

    oled_update_display();
} 

#define PI      3.141593654

void oled_draw_sin(void)
{
    // y = A*sin(2*pi*x)

    for (int x = 0; x < OLED_MAX_X; ++x) {
        double fx = 32 + 30.0 * sin(2 * PI * (x / 50.0));
        int y = OLED_MAX_Y - 1 - (int)(fx + 0.5);
        oled_set_pixel(x, y, OLED_SET_PIXEL, false);
    }
    oled_update_display();
}

void oled_circle(int x, int y, int r, int c)
{
    double step, t;
    int dx, dy;

    step = PI / 2 / 64;

    for (t = 0; t <= PI / 2; t += step) {
        dx = (int)(r * cos(t) + 0.5);
        dy = (int)(r * sin(t) + 0.5);

        if (x + dx < 128) {
            if (y + dy < 64) {
                oled_set_pixel(x + dx, y + dy, c, false);
            }
            if (y - dy >= 0) {
                oled_set_pixel(x + dx, y - dy, c, false);
            }
        }
        if (x - dx >= 0) {
            if (y + dy < 64) {
                oled_set_pixel(x - dx, y + dy, c, false);
            }
            if (y - dy >= 0) {
                oled_set_pixel(x - dx, y - dy, c, false);
            }
        }
    }

    oled_update_display();
}

void oled_return(void)
{
    s_active_col = 0;
}

void oled_new_line(void)
{
    if (++s_active_row >= N_ROWS) {
        s_active_row = N_ROWS - 1;
        oled_scrool(s_font_h);
    }
}

void oled_put_char(char c)
{
    if (s_active_col >= N_COLS) {
        oled_return();
        oled_new_line();
    }

    for (int h = 0, w; h < s_font_h; ++h) {
        oled_set_page(s_active_row * s_font_h + h);
        oled_set_segment(s_active_col * (s_font_w + 1));

        w = s_font_w * s_font_h * c + h;

        oled_begin_data();
        for (int i = 0; i < s_font_w; ++i) {
            oled_set_data(s_font[w]);
            w += s_font_h;
        }
        oled_set_data(0);
        oled_flush_cache();
    }
    ++s_active_col;
}

/* printf support */
void oled_putch(char c)
{
    switch (c) {
    case '\n': oled_new_line();
    case '\r': oled_return(); break;
    case '\f': oled_clear_display(); break;
    default: oled_put_char(c); break;
    }
}

void oled_printf(const char *fmt, ...)
{
    char buffer[128] = {0};

    va_list ap;
    va_start(ap, fmt);
    vsnprintf(&buffer[0], sizeof(buffer), fmt, ap);
    va_end(ap);
    for (size_t i = 0; i < strlen(buffer); i++) {
        oled_putch(buffer[i]);
    }
}

void oled_test(void)
{
    oled_start(0);
    oled_draw_sin();
    oled_putch('A');
    oled_putch('B');
    oled_putch('C');
    oled_line(37, 11, 93, 55, OLED_SET_PIXEL);
    oled_line(127, 0, 0, 63, OLED_SET_PIXEL);
    oled_circle(61, 30, 15, OLED_SET_PIXEL);
    oled_circle(41, 17, 6, OLED_SET_PIXEL);
    delay_ms(100);
    oled_clear_display();
    oled_printf("Line1\n");
    oled_printf("Line2\n");
    oled_printf("Line3 : line3");
    oled_printf("\nLine4");
    delay_ms(200);
    oled_printf("Line5123465768\n");
    oled_printf("*************\n");
}
