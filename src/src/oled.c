#include <stdio.h>
#include <stdint.h>

#include "system.h"
#include "spi.h"
#include "io.h"
#include "oled.h"

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

static uint8_t s_active_page, s_active_segment;
static uint8_t s_display_mem[N_PAGES * N_SEGMENT];

void oled_send_cmd(uint8_t cmd)
{
    io_write(IO_PIN_OLED_DC, 0);
#ifdef IO_OLED_CS
    io_write(IO_PIN_OLED_CS, 0);  /* active */
#endif

    spi_data(cmd);

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

    spi_data(data);
    s_display_mem[s_active_page * N_SEGMENT + s_active_segment] = data;
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

void oled_start(int rotate)
{
    spi_init();
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

    oled_send_cmd(OLED_CMD_DISPLAY_OFF);   
    oled_send_cmd(0xD4);     
    oled_send_cmd(OLED_CONTROL_BYTE_CMD_SINGLE);    
    oled_send_cmd(0xD3);     
    oled_send_cmd(OLED_CONTROL_BYTE_CMD_STREAM);    
    oled_send_cmd(OLED_CMD_SET_DISPLAY_START_LINE | 0); 
    oled_send_cmd(OLED_CMD_SET_CHARGE_PUMP); 
    oled_send_cmd(0x14); 

    if (rotate) {
        oled_send_cmd(0xA0); 
        oled_send_cmd(0xC0); 
    } else {
        oled_send_cmd(OLED_CMD_SET_SEGMENT_REMAP); 
        oled_send_cmd(OLED_CMD_SET_COM_SCAN_MODE);         
    }
    oled_send_cmd(OLED_CMD_SET_COM_PIN_MAP); 
    oled_send_cmd(0x12);  
    oled_send_cmd(OLED_CMD_SET_CONTRAST); 
    oled_send_cmd(0xCF);  
    oled_send_cmd(OLED_CMD_SET_PRECHARGE); 
    oled_send_cmd(0xF1);  
    oled_send_cmd(OLED_CMD_SET_VCOMH_DESELCT); 
    oled_send_cmd(OLED_CONTROL_BYTE_DATA_STREAM);  
    oled_send_cmd(OLED_CMD_DISPLAY_RAM);  
    oled_send_cmd(OLED_CMD_DISPLAY_NORMAL); 
    oled_send_cmd(OLED_CMD_DISPLAY_ON);  

    delay_ms(100);

    oled_clear_display();
}

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

void oled_set_pixel(int x, int y, int c)
{    
    x &= OLED_MAX_X - 1;
    y &= OLED_MAX_Y - 1;

    uint8_t page = y >> 3;
    uint8_t indx = y & 7;

    uint8_t val = s_display_mem[page * N_SEGMENT + x];

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

    oled_set_page(page);
    oled_set_segment(x);
    oled_send_data(val);
}