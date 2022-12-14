/*
 * td_uart.c
 *
 *  Created on: Nov 1, 2022
 *      Author: ECHO
 */
#include <string.h>
#include <stdbool.h>
#include <td_private.h>
#include <td_shell.h>
#include <td_uart.h>
#include "main.h"

typedef struct
{
	uint16_t q_front; /* Q front */
	uint16_t q_rear; /* Q rear */
	uint16_t item_num;
	uint8_t queue[TD_USART2_RCV_Q_SIZE];
} td_uart_state_t;
td_uart_state_t td_uart1_state;
td_uart_state_t td_uart2_state;

uint8_t td_uart1_rcv_buf[TD_USART1_RCV_Q_SIZE];
uint8_t ex_td_uart1_rcv_byte = 0;

uint8_t td_uart2_rcv_buf[TD_USART2_RCV_Q_SIZE];
uint8_t ex_td_uart2_rcv_byte = 0;

/* Data entry in Queue : UART1 */
static __INLINE void st_td_Uart1_RCV_Q_Put_INLINE(uint8_t item)
{
	td_uart1_state.q_rear = (td_uart1_state.q_rear + 1) % TD_USART1_RCV_Q_SIZE;

	td_uart1_state.queue[td_uart1_state.q_rear] = item;

	td_uart1_state.item_num++;
}
/* Data entry in Queue : UART2 */
static __INLINE void st_td_Uart2_RCV_Q_Put_INLINE(uint8_t item)
{
	td_uart2_state.q_rear = (td_uart2_state.q_rear + 1) % TD_USART2_RCV_Q_SIZE;

	td_uart2_state.queue[td_uart2_state.q_rear] = item;

	td_uart2_state.item_num++;
}

/* UART1 STATE INIT */
static __INLINE void st_td_Uart1_State_Init_INLINE(void)
{
	TD_USART1_STATE_RESET();
	//td_uart1_state.queue = td_uart1_rcv_buf;
}
/* UART2 STATE INIT */
static __INLINE void st_td_Uart2_State_Init_INLINE(void)
{
	TD_USART2_STATE_RESET();
	//td_uart2_state.queue = td_uart2_rcv_buf;
}

/*
 * UART1 INIT
 * */
void td_Uart1_Init(void)
{
	TD_USART1_HANDLE.Instance = USART1;
	TD_USART1_HANDLE.Init.BaudRate = 115200;
	TD_USART1_HANDLE.Init.WordLength = UART_WORDLENGTH_8B;
	TD_USART1_HANDLE.Init.StopBits = UART_STOPBITS_1;
	TD_USART1_HANDLE.Init.Parity = UART_PARITY_NONE;
	TD_USART1_HANDLE.Init.Mode = UART_MODE_TX_RX;
	TD_USART1_HANDLE.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	TD_USART1_HANDLE.Init.OverSampling = UART_OVERSAMPLING_16;
	TD_USART1_HANDLE.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	TD_USART1_HANDLE.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&TD_USART1_HANDLE) != HAL_OK)
	{
		Error_Handler();
	}
	st_td_Uart1_State_Init_INLINE();
}

/*
 * UART2 INIT
 * */
void td_Uart2_Init(void)
{
	TD_USART2_HANDLE.Instance = USART2;
	TD_USART2_HANDLE.Init.BaudRate = 115200;
	TD_USART2_HANDLE.Init.WordLength = UART_WORDLENGTH_8B;
	TD_USART2_HANDLE.Init.StopBits = UART_STOPBITS_1;
	TD_USART2_HANDLE.Init.Parity = UART_PARITY_NONE;
	TD_USART2_HANDLE.Init.Mode = UART_MODE_TX_RX;
	TD_USART2_HANDLE.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	TD_USART2_HANDLE.Init.OverSampling = UART_OVERSAMPLING_16;
	TD_USART2_HANDLE.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	TD_USART2_HANDLE.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart2) != HAL_OK)
	{
		Error_Handler();
	}
	st_td_Uart2_State_Init_INLINE();
}
/**********************/

/*
 * UART1 DeINIT
 * */
void td_Uart1_DeInit(void)
{
	HAL_UART_AbortReceive_IT(&TD_USART1_HANDLE);
	HAL_UART_DeInit(&TD_USART1_HANDLE);
	TD_USART1_STATE_RESET();
}

/*
 * UART2 DeINIT
 * */
void td_Uart2_DeInit(void)
{
	HAL_UART_AbortReceive_IT(&TD_USART2_HANDLE);
	HAL_UART_DeInit(&TD_USART2_HANDLE);
	TD_USART2_STATE_RESET();
}
/**********************/

/*
 * UART1 INTERRUPT ENABLE
 * */
void td_Uart1_Interrupt_Enable(void)
{
	/* Clear state REGs */
	TD_USART1_STATE_RESET();
	HAL_UART_AbortReceive_IT(&TD_USART1_HANDLE);
	HAL_UART_Receive_IT(&TD_USART1_HANDLE, &ex_td_uart1_rcv_byte, 1);
}

/*
 * UART2 INTERRUPT ENABLE
 * */
void td_Uart2_Interrupt_Enable(void)
{
	/* Clear state REGs */
	TD_USART2_STATE_RESET();
	HAL_UART_AbortReceive_IT(&TD_USART2_HANDLE);
	HAL_UART_Receive_IT(&TD_USART2_HANDLE, &ex_td_uart2_rcv_byte, 1);
}
/**********************/

/*
 * 1. UART RX Interrupt
 */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		st_td_Uart1_RCV_Q_Put_INLINE(ex_td_uart1_rcv_byte);
	}
	else if (huart->Instance == USART2)
	{
		st_td_Uart2_RCV_Q_Put_INLINE(ex_td_uart2_rcv_byte);
	}
	HAL_UART_Receive_IT(&TD_USART1_HANDLE, &ex_td_uart1_rcv_byte, 1);
	HAL_UART_Receive_IT(&TD_USART2_HANDLE, &ex_td_uart2_rcv_byte, 1);
}

/**********************/

bool td_Uart1_Get_RCV_Q(uint8_t *item)
{
	bool data_valid;
#ifdef DEBUG
	TD_SHELL_PRINT(("aulUsart1GetRcvQ()\n"));
#endif
	data_valid = false;

	if (td_uart1_state.item_num > 0)
	{
		td_uart1_state.q_front = (td_uart1_state.q_front + 1)
				% TD_USART1_RCV_Q_SIZE;
		*item = td_uart1_state.queue[td_uart1_state.q_front];

		TD_USART1_MUTEX_LOCK;

		td_uart1_state.item_num--;

		TD_USART1_MUTEX_UNLOCK;

		data_valid = true;
	}
	return data_valid;
}

bool td_Uart2_Get_RCV_Q(uint8_t *item)
{
	bool data_valid;
#ifdef DEBUG
	TD_SHELL_PRINT(("aulUsart1GetRcvQ()\n"));
#endif
	data_valid = false;

	if (td_uart2_state.item_num > 0)
	{
		td_uart2_state.q_front = (td_uart2_state.q_front + 1)
				% TD_USART2_RCV_Q_SIZE;
		*item = td_uart2_state.queue[td_uart2_state.q_front];

		TD_USART2_MUTEX_LOCK;

		td_uart2_state.item_num--;

		TD_USART2_MUTEX_UNLOCK;

		data_valid = true;
	}
	return data_valid;
}

