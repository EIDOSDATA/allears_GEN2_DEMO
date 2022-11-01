/*
 * td_btn.h
 *
 *  Created on: Oct 24, 2022
 *      Author: ECHO
 */

#ifndef INC_APP_TD_BTN_H_
#define INC_APP_TD_BTN_H_

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TD_BTN_HELD_TIME						1000	/* 100ms */
#define TD_BTN_IS_PRESSED()						(HAL_GPIO_ReadPin(START_BTN_GPIO_Port, START_BTN_Pin) == GPIO_PIN_RESET)

#define TD_BTN_STATE_PRESSED					td_btn_state.pressed
#define TD_BTN_STATE_HANDLE_ENABLE				td_btn_state.handle_enable
#define TD_BTN_STATE_HANDLED					td_btn_state.handled
#define TD_BTN_STATE_HELD_TICK					td_btn_state.held_tick

bool td_Btn_IsHandled(void);
void td_Btn_Handled_Clear(void);
void td_Btn_HandleEnable(bool enable);
void td_Btn_Handle(void);

#endif /* INC_APP_TD_BTN_H_ */
