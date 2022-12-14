#include <stdio.h>
#include <td_private.h>
#include <td_uart.h>
#include "main.h"

struct __FILE
{
	int handle;
};

FILE __stdout;
FILE __stdin;

/*
 This fucntion is used in other libraries, so must be defined although in case DEBUG_PRINT_ENABLED is not defined
 */

#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else 	/* Keil */
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE
{
	if (ch == '\n')
	{
		HAL_UART_Transmit(&TD_USART2_HANDLE, (uint8_t*) "\r", 1, 0xFFFF);
	}
	HAL_UART_Transmit(&TD_USART2_HANDLE, (uint8_t*) &ch, 1, 0xFFFF);
	return ch;
}
