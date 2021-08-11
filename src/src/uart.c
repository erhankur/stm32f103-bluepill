#include <stdio.h>
#include <stdint.h>

#include "system.h"
#include "io.h"
#include "uart.h"

#include "stm32f10x.h"
#include "stm32f10x_usart.h"

int g_console_uart = UART_1;

struct uart_config {
	int	io_tx;
	int	io_rx;
	uint32_t clock_source;
	USART_TypeDef *usart;
};

static struct uart_config s_uart_config[] = {
    { IO_PIN_UART1_TX, IO_PIN_UART1_RX, RCC_APB2Periph_USART1, USART1 },
    { IO_PIN_UART2_TX, IO_PIN_UART2_RX, RCC_APB1Periph_USART2, USART2 },
    { IO_PIN_UART3_TX, IO_PIN_UART3_RX, RCC_APB1Periph_USART3, USART3 },
};

void uart_init(int idx, int baud)
{
	USART_InitTypeDef uInit;

	io_init(s_uart_config[idx].io_tx, IO_MODE_ALTERNATE);
	io_init(s_uart_config[idx].io_rx, IO_MODE_INPUT);

	if (idx == UART_1)
		RCC_APB2PeriphClockCmd(s_uart_config[idx].clock_source, ENABLE);
	else
		RCC_APB1PeriphClockCmd(s_uart_config[idx].clock_source, ENABLE);

	uInit.USART_BaudRate = baud;
	uInit.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	uInit.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	uInit.USART_Parity = USART_Parity_No;
	uInit.USART_StopBits = USART_StopBits_1;
	uInit.USART_WordLength = USART_WordLength_8b;

	USART_Init(s_uart_config[idx].usart, &uInit);
	USART_Cmd(s_uart_config[idx].usart, ENABLE);
}

void uart_send(int idx, unsigned char val)
{
	while (!USART_GetFlagStatus(s_uart_config[idx].usart, USART_FLAG_TXE)) ;
	USART_SendData(s_uart_config[idx].usart, val);
}

void uart_send2(int idx, unsigned char val)
{
	USART_SendData(s_uart_config[idx].usart, val);
	while (!USART_GetFlagStatus(s_uart_config[idx].usart, USART_FLAG_TC)) ;
}

void uart_putch(unsigned char c)
{
	if (c == '\n')
		uart_send(g_console_uart, '\r');

	uart_send(g_console_uart, c);
}