#include <stdint.h>
#include "system.h"

#include "stm32f10x.h"

static volatile tick_type_t s_ticks;

void sys_tick_handler(void)
{
    ++s_ticks;
}

tick_type_t sys_get_tick_count(void)
{
    return s_ticks; 
}

void sys_tick_init(void)
{
    SysTick_Config(SystemCoreClock / CLOCK_PER_SECOND);
}

void sys_io_init(void)
{
    
}




