#include <stdint.h>
#include "system.h"

#include "stm32f10x.h"

static volatile tick_type_t s_ticks;

static volatile int s_gpio_clocks[] = {
    RCC_APB2Periph_GPIOA,
    RCC_APB2Periph_GPIOB,
    RCC_APB2Periph_GPIOC,
    RCC_APB2Periph_GPIOD,
    RCC_APB2Periph_GPIOE,
    RCC_APB2Periph_GPIOF,
    RCC_APB2Periph_GPIOG,
};

#define N_PORTS (sizeof(s_gpio_clocks)  / sizeof(s_gpio_clocks[0]))

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
    for (int i = 0; i < N_PORTS; ++i) {
        RCC_APB2PeriphClockCmd(s_gpio_clocks[i], ENABLE);
    }

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);    
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
}




