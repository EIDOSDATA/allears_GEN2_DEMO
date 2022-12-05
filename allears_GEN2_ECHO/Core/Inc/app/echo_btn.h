/*
 * echo_btn.h
 *
 *  Created on: Oct 24, 2022
 *      Author: ECHO
 */

#ifndef INC_APP_ECHO_BTN_H_
#define INC_APP_ECHO_BTN_H_

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ECHO_BTN_HELD_TIME							1000	/* 100ms */
#define ECHO_BTN_IS_PRESSED()						(HAL_GPIO_ReadPin(START_BTN_GPIO_Port, START_BTN_Pin) == GPIO_PIN_RESET)

#define ECHO_BTN_STATE_PRESSED						echo_btn_state.pressed
#define ECHO_BTN_STATE_HANDLE_ENABLE				echo_btn_state.handle_enable
#define ECHO_BTN_STATE_HANDLED						echo_btn_state.handled
#define ECHO_BTN_STATE_HELD_TICK					echo_btn_state.held_tick

bool Echo_Btn_IsHandled(void);
void Echo_Btn_Handled_Clear(void);
void Echo_Btn_HandleEnable(bool enable);
void Echo_Btn_Handle(void);

#endif /* INC_APP_ECHO_BTN_H_ */
