/*
 * echo_uart2.h
 *
 *  Created on: Nov 1, 2022
 *      Author: ECHO
 */
#include <stdbool.h>
#include "main.h"

#ifndef INC_APP_ECHO_UART2_H_
#define INC_APP_ECHO_UART2_H_

void td_Uart2_Init(void);
void td_Uart2_DeInit(void);
void td_Uart2_Interrupt_Enable(void);
bool td_Uart2_Get_RCV_Q(uint8_t *item);

#define TD_USART1_HANDLE					huart1
extern UART_HandleTypeDef huart1;

#define TD_USART2_HANDLE			huart2
extern UART_HandleTypeDef huart2;

#endif /* INC_APP_ECHO_UART2_H_ */
