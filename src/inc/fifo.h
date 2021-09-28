#ifndef _FIFO_H_
#define _FIFO_H_

#include <stdint.h>
#include <stdbool.h>

typedef uint8_t FIFO_VAR;

typedef struct {
    FIFO_VAR *buff;
    FIFO_VAR *p_head;
    FIFO_VAR *p_tail;
    uint32_t len;
} fifo_t;

void fifo_init(fifo_t *pfifo, FIFO_VAR *buff, uint32_t len);
bool fifo_write(fifo_t *pfifo, FIFO_VAR data);
FIFO_VAR fifo_read(fifo_t *pfifo);
bool fifo_is_empty(fifo_t *pfifo);

#endif
