#include <stdio.h>
#include <stdint.h>

#include "system.h"
#include "io.h"
#include "rtc.h"

#define RTC_PRSCLR          32768
#define RTC_INIT_CONSTANT   0xE3C5

volatile int s_rtc_alarm = 0;
volatile int s_rtc_changed = 0;
volatile int s_time = 0;

void rtc_irq_config(void)
{
    /* rtc second */
    RTC_ClearITPendingBit(RTC_IT_SEC);
    RTC_ITConfig(RTC_IT_SEC, ENABLE);

    /* rtc alarm */
    RTC_ClearITPendingBit(RTC_IT_ALR);
    RTC_ITConfig(RTC_IT_ALR, ENABLE);

    /* rtc overflow */
    RTC_ITConfig(RTC_IT_OW, ENABLE);

    NVIC_SetPriority(RTC_IRQn, 3);
    NVIC_EnableIRQ(RTC_IRQn);
}

void rtc_init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    PWR_BackupAccessCmd(ENABLE);

    if (BKP_ReadBackupRegister(BKP_DR1) != RTC_INIT_CONSTANT) {
        BKP_DeInit();
        /* LSE init => run low speed external oscilator 32.768khz
            Why LSE ?
                it is connected to backup domain and easy to calculatate 1 second with 32.768khz
        */
        RCC_LSEConfig(RCC_LSE_ON);
        while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
        RCC_RTCCLKCmd(ENABLE);
        RTC_WaitForSynchro();
        RTC_WaitForLastTask();
        RTC_SetPrescaler(RTC_PRSCLR - 1);
        BKP_WriteBackupRegister(BKP_DR1, RTC_INIT_CONSTANT);
    } else {
        RTC_WaitForSynchro();
    }

    rtc_irq_config();
}

uint32_t rtc_get_counter(void)
{
    return RTC_GetCounter();
}

void rtc_set_counter(uint32_t val)
{
    RTC_WaitForLastTask();
    RTC_SetCounter(val);
    RTC_WaitForLastTask();
}

void rtc_set_alarm(uint32_t val)
{
    RTC_SetAlarm(val - 1);
}

void RTC_IRQHandler(void)
{
    if (RTC_GetITStatus(RTC_IT_SEC) == SET) {
        s_rtc_changed = 1;
        RTC_ClearITPendingBit(RTC_IT_SEC);
    } else if (RTC_GetITStatus(RTC_IT_ALR) == SET) {
        s_rtc_alarm = 1;
        RTC_ClearITPendingBit(RTC_IT_ALR);
    } else if (RTC_GetITStatus(RTC_IT_OW) == SET) {
        RTC_ClearITPendingBit(RTC_IT_OW);
    }
}