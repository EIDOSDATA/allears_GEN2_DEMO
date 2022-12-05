/*
 * td_uart2.h
 *
 *  Created on: Nov 1, 2022
 *      Author: ECHO
 */
#include <stdbool.h>
#include "main.h"

#ifndef INC_APP_TD_UART2_H_
#define INC_APP_TD_UART2_H_

/* UART 1 */
void td_Uart1_Init(void);
void td_Uart1_DeInit(void);
void td_Uart1_Interrupt_Enable(void);
bool td_Uart1_Get_RCV_Q(uint8_t *item);

#define TD_USART1_RCV_Q_SIZE						256
#define TD_USART1_MUTEX_LOCK						HAL_NVIC_DisableIRQ(USART1_IRQn)
#define TD_USART1_MUTEX_UNLOCK						HAL_NVIC_EnableIRQ(USART1_IRQn)
#define TD_USART1_STATE_RESET()						{td_uart1_state.q_front = 0; td_uart1_state.q_rear = 0; td_uart1_state.item_num = 0;}

#define TD_USART1_HANDLE							huart1
extern UART_HandleTypeDef huart1;

/* UART 2 */
void td_Uart2_Init(void);
void td_Uart2_DeInit(void);
void td_Uart2_Interrupt_Enable(void);
bool td_Uart2_Get_RCV_Q(uint8_t *item);

#define TD_USART2_RCV_Q_SIZE						256
#define TD_USART2_MUTEX_LOCK						HAL_NVIC_DisableIRQ(USART2_IRQn)
#define TD_USART2_MUTEX_UNLOCK						HAL_NVIC_EnableIRQ(USART2_IRQn)
#define TD_USART2_STATE_RESET()						{td_uart2_state.q_front = 0; td_uart2_state.q_rear = 0; td_uart2_state.item_num = 0;}

#define TD_USART2_HANDLE							huart2
extern UART_HandleTypeDef huart2;

#endif /* INC_APP_TD_UART2_H_ */
