/*
 * echo_uart2.h
 *
 *  Created on: Nov 1, 2022
 *      Author: ECHO
 */

#ifndef INC_APP_ECHO_UART2_H_
#define INC_APP_ECHO_UART2_H_

void Echo_Uart2_Init(void);
void Echo_Uart2_DeInit(void);

void Echo_Uart2_INTERRUPT_ENA(void);

bool Echo_Uart2_Get_RCV_Q(uint8_t *item);

#define ECHO_USART1_HANDLE					huart1
extern UART_HandleTypeDef huart1;

#define ECHO_USART2_HANDLE			huart2
extern UART_HandleTypeDef huart2;

#endif /* INC_APP_ECHO_UART2_H_ */
