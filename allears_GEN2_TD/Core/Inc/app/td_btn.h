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

bool td_Btn_IsHandled(void);
void td_Btn_Handled_Clear(void);
void td_Btn_HandleEnable(bool enable);
void td_Btn_Handle(void);

#endif /* INC_APP_TD_BTN_H_ */
