#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "system.h"
#include "spi.h"
#include "io.h"
#include "oled.h"
#include "font_5x8.h"
#include "font_7x16.h"

// Control byte
#define OLED_CONTROL_BYTE_CMD_SINGLE    0x80
#define OLED_CONTROL_BYTE_CMD_STREAM    0x00
#define OLED_CONTROL_BYTE_DATA_STREAM   0x40

// Fundamental commands (pg.28)
#define OLED_CMD_SET_CONTRAST           0x81    // follow with 0x7F
#define OLED_CMD_DISPLAY_RAM            0xA4
#define OLED_CMD_DISPLAY_ALLON          0xA5
#define OLED_CMD_DISPLAY_NORMAL         0xA6
#define OLED_CMD_DISPLAY_INVERTED       0xA7
#define OLED_CMD_DISPLAY_OFF            0xAE
#define OLED_CMD_DISPLAY_ON             0xAF

// Addressing Command Table (pg.30)
#define OLED_CMD_SET_MEMORY_ADDR_MODE   0x20    // follow with 0x00 = HORZ mode = Behave like a KS108 graphic LCD
#define OLED_CMD_SET_COLUMN_RANGE       0x21    // can be used only in HORZ/VERT mode - follow with 0x00 and 0x7F = COL127
#define OLED_CMD_SET_PAGE_RANGE         0x22    // can be used only in HORZ/VERT mode - follow with 0x00 and 0x07 = PAGE7

// Hardware Config (pg.31)
#define OLED_CMD_SET_DISPLAY_START_LINE 0x40
#define OLED_CMD_SET_SEGMENT_REMAP      0xA1
#define OLED_CMD_SET_MUX_RATIO          0xA8    // follow with 0x3F = 64 MUX
#define OLED_CMD_SET_COM_SCAN_MODE      0xC8
#define OLED_CMD_SET_DISPLAY_OFFSET     0xD3    // follow with 0x00
#define OLED_CMD_SET_COM_PIN_MAP        0xDA    // follow with 0x12
#define OLED_CMD_NOP                    0xE3    // NOP

// Timing and Driving Scheme (pg.32)
#define OLED_CMD_SET_DISPLAY_CLK_DIV    0xD5    // follow with 0x80
#define OLED_CMD_SET_PRECHARGE          0xD9    // follow with 0xF1
#define OLED_CMD_SET_VCOMH_DESELCT      0xDB    // follow with 0x30

// Charge Pump (pg.62)
#define OLED_CMD_SET_CHARGE_PUMP        0x8D    // follow with 0x14

#define N_PAGES         8
#define N_SEGMENT       128
#define OLED_MAX_X      128
#define OLED_MAX_Y      64

/* active font type */
#define FONT_SMALL      0
#define FONT_LARGE      1
#define FONT_WIDTH      5 // bit
#define FONT_HEIGHT     1 // byte
#define N_COLS          (N_SEGMENT / (FONT_WIDTH + 1))
#define N_ROWS          (N_SEGMENT / FONT_HEIGHT)
static const unsigned char *s_font = g_ChrTab;
static const uint8_t s_active_font = FONT_SMALL;
//5x8 font 8 lines 21 cols
//7x16 font 4 lines 16 cols

static uint8_t s_active_row, s_active_col;
static uint8_t s_active_page, s_active_segment;
static uint8_t s_frame_buff[N_PAGES * N_SEGMENT];

#define PI      3.141593654

//todo
/*
begin, add, end commands
*/

void oled_send_cmd(uint8_t cmd)
{
    io_write(IO_PIN_OLED_DC, 0);
#ifdef IO_OLED_CS
    io_write(IO_PIN_OLED_CS, 0);  /* active */
#endif

    SPI_DATA(cmd);

#ifdef IO_OLED_CS
    io_write(IO_PIN_OLED_CS, 1);  /* passive */
#endif

}

void oled_send_data(uint8_t data)
{
    io_write(IO_PIN_OLED_DC, 1);
#ifdef IO_OLED_CS
    io_write(IO_PIN_OLED_CS, 0);  /* active */
#endif

    SPI_DATA(data);
    s_frame_buff[s_active_page * N_SEGMENT + s_active_segment] = data;
    if (++s_active_segment >= N_SEGMENT) {
        s_active_segment = 0;
    }

#ifdef IO_OLED_CS
    io_write(IO_PIN_OLED_CS, 1);  /* passive */
#endif
}

void oled_set_page(uint8_t page)
{
    page &= N_PAGES - 1;
    s_active_page = page;
    oled_send_cmd(0xB0 | s_active_page);
}

void oled_set_segment(uint8_t seg)
{
    seg &= N_SEGMENT - 1;
    s_active_segment = seg;
    oled_send_cmd(s_active_segment >> 0 & 0x0F);
    oled_send_cmd(0x10 | s_active_segment >> 4);
}

void oled_rotate(int rotate)
{
    if (rotate) {
        oled_send_cmd(0xA0);
        oled_send_cmd(0xC0);
    } else {
        oled_send_cmd(0xA1);
        oled_send_cmd(0xC8);
    }
}

void oled_draw_sin(void)
{
    // y = A*sin(2*pi*x)

    for (int x = 0; x < OLED_MAX_X; ++x) {
        double fx = 32 + 30.0 * sin(2 * PI * (x / 50.0));
        int y = OLED_MAX_Y - 1 - (int)(fx + 0.5);
        oled_set_pixel(x, y, OLED_SET_PIXEL, 0);
    }
    oled_update_display();
}

void oled_start(int rotate)
{
    SPI_INIT();
#ifdef IO_OLED_GND
    io_write(IO_PIN_OLED_GND, 0);
    io_init(IO_PIN_OLED_GND, IO_MODE_OUTPUT);
#endif

#ifdef IO_OLED_VDD
    io_write(IO_PIN_OLED_VDD, 1);
    io_init(IO_PIN_OLED_VDD, IO_MODE_OUTPUT);
#endif

#ifdef IO_OLED_CS
    io_write(IO_PIN_OLED_CS, 1);
    io_init(IO_PIN_OLED_CS, IO_MODE_OUTPUT);
#endif

    io_init(IO_PIN_OLED_DC, IO_MODE_OUTPUT);

#ifdef IO_OLED_RES
    io_write(IO_PIN_OLED_RES, 0);
    io_init(IO_PIN_OLED_RES, IO_MODE_OUTPUT);
    delay_ms(10);
    io_write(IO_PIN_OLED_RES, 1);
#endif

    delay_ms(100);

    oled_send_cmd(OLED_CMD_DISPLAY_OFF);

    oled_send_cmd(0xD4);
    oled_send_cmd(0x80);

    oled_send_cmd(0xA8);
    oled_send_cmd(0x3F);

    oled_send_cmd(0xD3);
    oled_send_cmd(0x00);

    oled_send_cmd(0x40);

    oled_send_cmd(OLED_CMD_SET_CHARGE_PUMP);
    oled_send_cmd(0x14);

    oled_rotate(rotate);

    oled_send_cmd(OLED_CMD_SET_COM_PIN_MAP);
    oled_send_cmd(0x12);

    oled_send_cmd(OLED_CMD_SET_CONTRAST);
    oled_send_cmd(0x80);

    oled_send_cmd(OLED_CMD_SET_PRECHARGE);
    oled_send_cmd(0xF1);

    oled_send_cmd(OLED_CMD_SET_VCOMH_DESELCT);
    oled_send_cmd(0x40);

    oled_send_cmd(0xA4);   //0xA5 for test all pixels
    oled_send_cmd(OLED_CMD_DISPLAY_NORMAL);

    oled_clear_display();

    oled_send_cmd(OLED_CMD_DISPLAY_ON);
}

/* can be used to clear line */
void oled_fill_page(uint8_t page, uint8_t ch)
{
    oled_set_page(page);
    oled_set_segment(0);

    for (int i = 0; i < N_SEGMENT; ++i) {
        oled_send_data(ch);
    }
}

void oled_fill_display(uint8_t ch)
{
    for (int i = 0; i < N_PAGES; ++i) {
        oled_fill_page(i, ch);
    }
}

void oled_clear_display(void)
{
    oled_fill_display(0x00);
}

void oled_update_display(void)
{
    for (int i = 0, page = 0; page < N_PAGES; ++page) {
        oled_set_page(page);
        oled_set_segment(0);

        for (int seg = 0; seg < N_SEGMENT; ++seg) {
            oled_send_data(s_frame_buff[i++]);
        }
    }
}

void oled_set_pixel(int x, int y, int c, int update_now)
{
    x &= OLED_MAX_X - 1;
    y &= OLED_MAX_Y - 1;

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

    if (update_now) {
        oled_set_page(page);
        oled_set_segment(x);
        oled_send_data(val);
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
            oled_set_pixel(y, x, c, 0);
        } else {
            oled_set_pixel(x, y, c, 0);
        }

        error += deltay;
        if ((error << 1) >= deltax) {
            y += ystep;
            error -= deltax;
        } // if
    } // for

    oled_update_display();

} // line

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
                oled_set_pixel(x + dx, y + dy, c, 0);
            }
            if (y - dy >= 0) {
                oled_set_pixel(x + dx, y - dy, c, 0);
            }
        }
        if (x - dx >= 0) {
            if (y + dy < 64) {
                oled_set_pixel(x - dx, y + dy, c, 0);
            }
            if (y - dy >= 0) {
                oled_set_pixel(x - dx, y - dy, c, 0);
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
        oled_scrool(FONT_HEIGHT);
    }
}

void oled_put_char(char c)
{
    if (s_active_col >= N_COLS) {
        oled_return();
        oled_new_line();
    }

    for (int h = 0, w; h < FONT_HEIGHT; ++h) {
        oled_set_page(s_active_row * FONT_HEIGHT + h);
        oled_set_segment(s_active_col * (FONT_WIDTH + 1));

        w = FONT_WIDTH * FONT_HEIGHT * c + h;

        for (int i = 0; i < FONT_WIDTH; ++i) {
            oled_send_data(s_font[i]);
            w += FONT_HEIGHT;
        }

        oled_send_data(0);
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