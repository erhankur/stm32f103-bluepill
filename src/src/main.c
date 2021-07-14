#include <stdio.h>
#include <stdint.h>

#include "system.h"

void init(void)
{
    sys_tick_init();
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

    while (1) {
        task_a();
        task_b();
        task_c();
    }
}
