/*
 * td_btn.c
 *
 *  Created on: Oct 24, 2022
 *      Author: ECHO
 */
#include <td_btn.h>
#include <td_shell.h>
#include "main.h"

typedef struct
{
	bool pressed;
	bool handle_enable;
	bool handled;
	uint32_t held_tick;
} td_btn_state_data_t;

td_btn_state_data_t td_btn_state;

bool td_Btn_IsHandled(void)
{
	return TD_BTN_STATE_HANDLED;
}

void td_Btn_Handled_Clear(void)
{
	TD_BTN_STATE_HANDLED = false;
}

void td_Btn_HandleEnable(bool enable)
{
	TD_BTN_STATE_HANDLE_ENABLE = enable;
}

void td_Btn_Handle(void)
{
	bool pressed;
#ifdef DEBUG
	char res_msg[10] =
	{ '\0', };
#endif

	/* Only works when battery is normal level */
	pressed = TD_BTN_IS_PRESSED();

	if (pressed != TD_BTN_STATE_PRESSED)
	{
		TD_BTN_STATE_PRESSED = pressed;
#ifdef DEBUG
		sprintf((char*) res_msg, (const char*) "BTN: %d\r\n", pressed);
		TD_SHELL_PRINT(("%s\n",res_msg));
#endif

		if (TD_BTN_STATE_PRESSED == false)
		{
			/* Only when cover is closed, send signal */

		}
		else
		{
			/* BUTTON PRESSED, Send Signal >> td_state.c */
			TD_BTN_STATE_HANDLED = true;
			TD_BTN_STATE_HELD_TICK = 0;
		}
	}
	/* Check held */
	else if (TD_BTN_STATE_PRESSED == true)
	{
		if (TD_BTN_STATE_HELD_TICK == TD_BTN_HELD_TIME)
		{
#ifdef DEBUG
			sprintf((char*) res_msg, (const char*) "BTN: %d\r\n", pressed);
			TD_SHELL_PRINT(("%s\n",res_msg));
#endif
			TD_BTN_STATE_HELD_TICK++;
		}
		else if (TD_BTN_STATE_HELD_TICK < TD_BTN_HELD_TIME)
		{
			TD_BTN_STATE_HELD_TICK++;
		}
		else
		{
			/* Ignored */
		}
	}
}
