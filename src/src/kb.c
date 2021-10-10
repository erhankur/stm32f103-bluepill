#include <stdio.h>
#include <stdint.h>

#include "system.h"
#include "io.h"
#include "kb.h"
//#include "fifo.h"

#define SZ_KEYBUF       16

volatile int g_key = KB_INVALIDKEY;
int g_max_debounce = 5; //ms

// FIFO_VAR    s_key_buf[SZ_KEYBUF];
// fifo_t      s_kb_fifo;

static volatile int s_kb_init = 0;

static int s_rows[KB_NROWS] = {
    IO_PIN_KEY_ROW0, IO_PIN_KEY_ROW1, IO_PIN_KEY_ROW2, IO_PIN_KEY_ROW3,
};

static int s_cols[KB_NCOLS] = {
    IO_PIN_KEY_COL0, IO_PIN_KEY_COL1, IO_PIN_KEY_COL2,
};

char g_ch_table[KB_NROWS * KB_NCOLS] = "0123456789*#";

void kb_init(void)
{
    if (s_kb_init) {
        return;
    }

    //fifo_init(&s_kb_fifo, s_key_buf, SZ_KEYBUF);

    for (int i = 0; i < KB_NROWS; ++i) {
        io_init(s_rows[i], IO_MODE_INPUT_PU);
    }

    for (int i = 0; i < KB_NCOLS; ++i) {
        io_write(s_cols[i], 0);
        io_init(s_cols[i], IO_MODE_OUTPUT);
    }

    s_kb_init = 1;
}

static int kb_scan_rows(void)
{
    for (int row = 0; row < KB_NROWS; ++row) {
        if (!io_read(s_rows[row])) {
            return row;
        }
    }
    return KB_NROWS;
}

void kb_scan(void)
{
    static int db_cnt = 0;

    if (!s_kb_init) {
        return;
    }

    int row = kb_scan_rows();

    if (g_key == KB_INVALIDKEY) {
        for (int col = 0; col < KB_NCOLS; ++col) {
            for (int i = 0; i < KB_NCOLS; ++i) {
                io_write(s_cols[i], (i == col) ? 0 : 1);
            }

            delay_us(1);

            row = kb_scan_rows();

            if (row < KB_NROWS) {
                /* scan code (g_key) = row * NCOLS + col */
                g_key = row * KB_NCOLS + col;
                //fifo_write(&s_kb_fifo, g_key);
                break;
            }
        }
        for (int i = 0; i < KB_NCOLS; ++i) {
            io_write(s_cols[i], 0);
        }
    } else {
        if (row >= KB_NROWS) {
            if (++db_cnt >= g_max_debounce) {
                // Key released
                g_key = KB_INVALIDKEY;
                db_cnt = 0;
            }
        } else {
            db_cnt = 0;
        }
    }
}

int _kbhit(void)
{
    return KB_INVALIDKEY; //!fifo_read(&s_kb_fifo);
}

int _getch(void)
{
    return KB_INVALIDKEY;
    //return g_ch_table[fifo_read(&s_kb_fifo)];
}