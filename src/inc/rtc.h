#ifndef _RTC_H_
#define _RTC_H_

#include <stdint.h>

void rtc_init(void);
uint32_t rtc_get_counter(void);
void rtc_set_counter(uint32_t val);
void rtc_set_alarm(uint32_t val);

extern volatile int s_rtc_alarm;

#endif