#include <stdlib.h>
#include "stm32f10x.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LED_PIN         GPIO_Pin_13

static void led_init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitTypeDef gpio_out = {
        .GPIO_Pin = LED_PIN,
        .GPIO_Mode = GPIO_Mode_Out_PP,
        .GPIO_Speed = GPIO_Speed_2MHz,
    };
    GPIO_Init(GPIOC, &gpio_out);
}

static void clock_init(void)
{
    /* Reset the RCC clock configuration to the default reset state. */
    /* HSI ON, PLL OFF, HSE OFF, system clock = 72 MHz, cpu_clock = 72 MHz */
    RCC_DeInit();
    SystemCoreClockUpdate();  /* BluePill board runs at 72 MHz */
}

static void led_task(void *params)
{
    (void)(params);

    led_init();

    while (pdTRUE) {
        GPIO_SetBits(GPIOC, LED_PIN);
        vTaskDelay(pdMS_TO_TICKS(500));
        GPIO_ResetBits(GPIOC, LED_PIN);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

int main(void)
{
    clock_init();

    xTaskCreate(led_task, "led_task", 512, NULL, 1, NULL);

    vTaskStartScheduler();

    while (pdTRUE)
        ;
}