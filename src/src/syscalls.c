#include <errno.h>
#include <stdio.h>
#include <sys/unistd.h>
#include "lcd.h"
#include "uart.h"

int _write(int file, const char *ptr, size_t len)
{
    if (!ptr) {
        return 0;
    }

    if ((file != STDOUT_FILENO) && (file != STDERR_FILENO)) {
        errno = EBADF;
        return -1;
    }

    for (size_t inx = 0; inx < len; ++inx) {
        //lcd_put_char(*ptr++);
        uart_putch(*ptr++);
    }

    return len;
}