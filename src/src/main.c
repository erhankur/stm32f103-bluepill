#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "system.h"
#include "io.h"
#include "log.h"
#include "kb.h"
#include "oled.h"
#include "rtc.h"
#include "button.h"

static void init_peripherals(void)
{
    sys_io_init();
    log_level_init(":", LOG_LEVEL_INFO);
    rtc_init();
    oled_start(1);
}

static void led_task(void *params)
{
    (void)(params);

    io_write(IO_PIN_LED, 1);
    io_init(IO_PIN_LED, IO_MODE_OUTPUT);

    while (pdTRUE) {
        io_write(IO_PIN_LED, IO_PIN_LED_ON);
        vTaskDelay(pdMS_TO_TICKS(100));
        io_write(IO_PIN_LED, IO_PIN_LED_OFF);
        vTaskDelay(pdMS_TO_TICKS(900));
    }
}

static void button_process_task(void *params)
{
    (void)(params);

    uint32_t ulNotifiedValue;
    bool long_press;

    while (pdTRUE) {
        xTaskNotifyWait(0, 0, &ulNotifiedValue, portMAX_DELAY);

        if (ulNotifiedValue & BUTTON_LONG) {
            long_press = true;
            ulNotifiedValue &= ~(BUTTON_LONG);
        } else {
            long_press = false;
        }

        LOG_I("button long press: %d", long_press);

        if (ulNotifiedValue & BUTTON_SET) {
            LOG_I("BUTTON_SET pressed!");
            oled_printf("\f\rSET");
        } else if (ulNotifiedValue & BUTTON_UP) {
            LOG_I("BUTTON_UP pressed!");
            oled_printf("\f\rUP");
        } else if (ulNotifiedValue & BUTTON_DOWN) {
            LOG_I("BUTTON_DOWN pressed!");
            oled_printf("\f\rDOWN");
        } else {
            LOG_I("Noise ?");
        }
    }
}

static void button_scanner_task(void *params)
{
    (void)(params);
    TaskHandle_t button_process_task_handle;

    xTaskCreate(button_process_task, "button_process", 
                        256, 
                        NULL, 
                        1, 
                        &button_process_task_handle);

    button_init(button_process_task_handle);

    while (pdTRUE) {
        button_scan_all();
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

int main(void)
{
    init_peripherals();

    oled_draw_sin();    //for test
    oled_line(0, 32, 128, 32, OLED_SET_PIXEL);

    xTaskCreate(led_task, "led_task", 256, NULL, 1, NULL);
    xTaskCreate(button_scanner_task, "button_scan", 256, NULL, 1, NULL);

    vTaskStartScheduler();

    while (pdTRUE)
        ;
}