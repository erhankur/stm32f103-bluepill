#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include <stdint.h>

#define CLOCK_PER_SECOND    1000

typedef uint32_t tick_type_t;

tick_type_t sys_get_tick_count(void);
void sys_tick_init(void);
void sys_io_init(void);

#endif
