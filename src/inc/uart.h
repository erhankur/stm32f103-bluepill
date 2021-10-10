#ifndef _UART_H_
#define _UART_H_

#include <stddef.h> /* size_t */
#include <stdint.h>

enum {
    UART_1,
    UART_2,
    UART_3
};

void uart_init(int idx, int baud);
void uart_send(int idx, uint8_t val);
int uart_data_ready(int idx);
unsigned char uart_recv(int idx);
void uart_send_buffer(int idx, const uint8_t *buffer, size_t len);

#endif
