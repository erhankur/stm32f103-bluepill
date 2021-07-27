#include <stdio.h>
#include <stdint.h>

#include "system.h"
#include "io.h"

void init(void)
{
    sys_tick_init();
    sys_io_init();

    io_write(IO_PIN_LED, 1);
    io_init(IO_PIN_LED, IO_MODE_OUTPUT);
}

void task_a(void)
{

}

void task_b(void)
{

}

void task_c(void)
{

}

int main()
{
    init();
    
    io_write(IO_PIN_LED, 0);

    io_init(IO_PIN_TEST_OUT, IO_MODE_OUTPUT);
    io_write(IO_PIN_TEST_OUT, 1);

    io_init(IO_PIN_TEST_IN, IO_MODE_INPUT);
    io_read(IO_PIN_TEST_IN);

    while (1) {
        task_a();
        task_b();
        task_c();
    }
}
