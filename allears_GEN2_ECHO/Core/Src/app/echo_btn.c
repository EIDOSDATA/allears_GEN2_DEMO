/*
 * echo_btn.c
 *
 *  Created on: Oct 24, 2022
 *      Author: ECHO
 */
#include <echo_btn.h>
#include <echo_shell.h>
#include "main.h"

typedef struct
{
	bool pressed;
	bool handle_enable;
	bool handled;
	uint32_t held_tick;
} echo_btn_state_data_t;

echo_btn_state_data_t echo_btn_state;

bool Echo_Btn_IsHandled(void)
{
	return ECHO_BTN_STATE_HANDLED;
}

void Echo_Btn_Handled_Clear(void)
{
	ECHO_BTN_STATE_HANDLED = false;
}

void Echo_Btn_HandleEnable(bool enable)
{
	ECHO_BTN_STATE_HANDLE_ENABLE = enable;
}

void Echo_Btn_Handle(void)
{
	bool pressed;
#ifdef DEBUG
	char res_msg[10] =
	{ '\0', };
#endif

	/* Only works when battery is normal level */
	pressed = ECHO_BTN_IS_PRESSED();

	if (pressed != ECHO_BTN_STATE_PRESSED)
	{
		ECHO_BTN_STATE_PRESSED = pressed;
#ifdef DEBUG
		sprintf((char*) res_msg, (const char*) "BTN: %d\r\n", pressed);
		TD_SHELL_PRINT(("%s\n",res_msg));
#endif

		if (ECHO_BTN_STATE_PRESSED == false)
		{
			/* Only when cover is closed, send signal */

		}
		else
		{
			/* BUTTON PRESSED, Send Signal >> echo_state.c */
			ECHO_BTN_STATE_HANDLED = true;
			ECHO_BTN_STATE_HELD_TICK = 0;
		}
	}
	/* Check held */
	else if (ECHO_BTN_STATE_PRESSED == true)
	{
		if (ECHO_BTN_STATE_HELD_TICK == ECHO_BTN_HELD_TIME)
		{
#ifdef DEBUG
			sprintf((char*) res_msg, (const char*) "BTN: %d\r\n", pressed);
			TD_SHELL_PRINT(("%s\n",res_msg));
#endif
			ECHO_BTN_STATE_HELD_TICK++;
		}
		else if (ECHO_BTN_STATE_HELD_TICK < ECHO_BTN_HELD_TIME)
		{
			ECHO_BTN_STATE_HELD_TICK++;
		}
		else
		{
			/* Ignored */
		}
	}
}

