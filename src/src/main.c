#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "system.h"
#include "io.h"

static void led_task(void *params)
{
    (void)(params);

    io_write(IO_PIN_LED, 1);
    io_init(IO_PIN_LED, IO_MODE_OUTPUT);

    while (pdTRUE) {
        io_write(IO_PIN_LED, 0);
        vTaskDelay(pdMS_TO_TICKS(100));
        io_write(IO_PIN_LED, 1);
        vTaskDelay(pdMS_TO_TICKS(900));
    }
}

int main(void)
{
    sys_io_init();

    xTaskCreate(led_task, "led_task", 512, NULL, 1, NULL);

    vTaskStartScheduler();

    while (pdTRUE)
        ;
}