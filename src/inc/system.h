#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include <stdint.h>

typedef uint32_t tick_type_t;

tick_type_t sys_get_tick_count(void);
void sys_tick_init(void);
void sys_io_init(void);
void sys_console_init(void);

#define CLOCK_FREQ          72000000
#define CLOCK_PER_SECOND    1000
#define INSTR_FREQ          ((CLOCK_FREQ) * 8 / 9)
#define US_CYCLES           ((INSTR_FREQ) / 1000000)
#define MS_CYCLES           ((INSTR_FREQ) / 1000)

void __delay(uint32_t ncy);     /* from delay.s */
#define delay_us(us)        __delay((us) * US_CYCLES)
#define delay_ms(ms)        __delay((ms) * MS_CYCLES)

#endif
