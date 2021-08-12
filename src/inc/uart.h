#ifndef _UART_H_
#define _UART_H_

enum {
    UART_1,
	UART_2,
	UART_3
};

void uart_init(int idx, int baud);
void uart_send(int idx, unsigned char val);
void uart_putch(unsigned char c);
int uart_data_ready(int idx);
unsigned char uart_recv(int idx);

extern int g_console_uart;

#endif
