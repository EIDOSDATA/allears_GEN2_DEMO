/*
 * echo_uart.c
 *
 *  Created on: Nov 1, 2022
 *      Author: ECHO
 */
#include <string.h>
#include <stdbool.h>
#include <echo_private.h>
#include <echo_shell.h>
#include <echo_uart2.h>
#include "main.h"

typedef struct
{
	uint16_t q_front; /* Q front */
	uint16_t q_rear; /* Q rear */
	uint16_t item_num;
	uint8_t queue[ECHO_USART2_RCV_Q_SIZE];
} echo_uart_state_t;
echo_uart_state_t echo_uart2_state;

uint8_t echo_uart2_rcv_buf[ECHO_USART2_RCV_Q_SIZE];
uint8_t ex_echo_uart2_rcv_byte = 0;

static __INLINE void Echo_Uart2_RCV_Q_Put_INLINE(uint8_t item)
{
	echo_uart2_state.q_rear = (echo_uart2_state.q_rear + 1) % ECHO_USART2_RCV_Q_SIZE;

	echo_uart2_state.queue[echo_uart2_state.q_rear] = item;

	echo_uart2_state.item_num++;
}

static __INLINE void Echo_Uart2_State_Init_INLINE(void)
{
	ECHO_USART2_STATE_RESET();
	//echo_uart2_state.queue = echo_uart2_rcv_buf;
}

/*
 * UART2 INIT
 * */
void Echo_Uart2_Init(void)
{
	ECHO_USART2_HANDLE.Instance = USART2;
	ECHO_USART2_HANDLE.Init.BaudRate = 115200;
	ECHO_USART2_HANDLE.Init.WordLength = UART_WORDLENGTH_8B;
	ECHO_USART2_HANDLE.Init.StopBits = UART_STOPBITS_1;
	ECHO_USART2_HANDLE.Init.Parity = UART_PARITY_NONE;
	ECHO_USART2_HANDLE.Init.Mode = UART_MODE_TX_RX;
	ECHO_USART2_HANDLE.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	ECHO_USART2_HANDLE.Init.OverSampling = UART_OVERSAMPLING_16;
	ECHO_USART2_HANDLE.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	ECHO_USART2_HANDLE.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart2) != HAL_OK)
	{
		Error_Handler();
	}
	Echo_Uart2_State_Init_INLINE();
}
/**********************/

/*
 * UART2 DeINIT
 * */
void Echo_Uart2_DeInit(void)
{
	HAL_UART_AbortReceive_IT(&ECHO_USART2_HANDLE);
	HAL_UART_DeInit(&ECHO_USART2_HANDLE);
	ECHO_USART2_STATE_RESET();
}
/**********************/

/*
 * UART2 INTERRUPT ENABLE
 * */
void Echo_Uart2_Interrupt_Enable(void)
{
	/* Clear state REGs */
	ECHO_USART2_STATE_RESET();
	HAL_UART_AbortReceive_IT(&ECHO_USART2_HANDLE);
	HAL_UART_Receive_IT(&ECHO_USART2_HANDLE, &ex_echo_uart2_rcv_byte, 1);
}
/**********************/

/*
 * 1. UART RX Interrupt
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
	}
	else if (huart->Instance == USART2)
	{
		Echo_Uart2_RCV_Q_Put_INLINE(ex_echo_uart2_rcv_byte);
	}
	HAL_UART_Receive_IT(&ECHO_USART2_HANDLE, &ex_echo_uart2_rcv_byte, 1);
}
/**********************/

bool Echo_Uart2_Get_RCV_Q(uint8_t *item)
{
	bool data_valid;
#ifdef DEBUG
	TD_SHELL_PRINT(("aulUsart1GetRcvQ()\n"));
#endif
	data_valid = false;

	if (echo_uart2_state.item_num > 0)
	{
		echo_uart2_state.q_front = (echo_uart2_state.q_front + 1)
				% ECHO_USART2_RCV_Q_SIZE;
		*item = echo_uart2_state.queue[echo_uart2_state.q_front];

		ECHO_USART2_MUTEX_LOCK
		;

		echo_uart2_state.item_num--;

		ECHO_USART2_MUTEX_UNLOCK
		;

		data_valid = true;
	}
	return data_valid;
}
