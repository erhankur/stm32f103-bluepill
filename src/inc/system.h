#ifndef _SYSTEM_H_
#define _SYSTEM_H_

void sys_io_init(void);
void sys_reset(void);

#define CLOCK_FREQ          72000000
#define INSTR_FREQ          ((CLOCK_FREQ) * 8 / 9)
#define US_CYCLES           ((INSTR_FREQ) / 1000000)
#define MS_CYCLES           ((INSTR_FREQ) / 1000)

void __delay(unsigned int ncy);     /* from delay.s */
#define delay_us(us)        __delay((us) * US_CYCLES)
#define delay_ms(ms)        __delay((ms) * MS_CYCLES)

#endif