#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "fifo.h"

void fifo_init(fifo_t *pfifo, FIFO_VAR *buff, uint32_t len)
{
    pfifo->buff = buff;
    pfifo->len = len;
    pfifo->p_head = pfifo->p_tail = buff;
}

bool fifo_write(fifo_t *pfifo, FIFO_VAR data)
{
    FIFO_VAR *ptr = pfifo->p_tail;

    if (++ptr == pfifo->buff + pfifo->len) {
        ptr = pfifo->buff;
    }

    if (ptr == pfifo->p_head) {
        return false;
    }

    *pfifo->p_tail = data;
    pfifo->p_tail = ptr;

    return true;
}

bool fifo_is_empty(fifo_t *pfifo)
{
    return pfifo->p_head == pfifo->p_tail;
}

FIFO_VAR fifo_read(fifo_t *pfifo)
{
    while (fifo_is_empty(pfifo));

    FIFO_VAR data = *pfifo->p_head;

    if (++pfifo->p_head == pfifo->buff + pfifo->len) {
        pfifo->p_head = pfifo->buff;
    }

    return data;
}
