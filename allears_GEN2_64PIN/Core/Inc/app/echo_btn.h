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

bool Echo_Btn_IsHandled(void);
void Echo_Btn_Handled_Clear(void);
void Echo_Btn_HandleEnable(bool enable);
void Echo_Btn_Handle(void);

#endif /* INC_APP_ECHO_BTN_H_ */
