/*
 * echo_led.c
 *
 *  Created on: Oct 24, 2022
 *      Author: ECHO
 */
#include <echo_led.h>
#include <echo_private.h>
#include <echo_schedule.h>
#include <echo_shell.h>
#include <echo_sys_fsm_state.h>
#include "main.h"

typedef struct
{
	uint8_t led_colors;
	uint16_t off_time;
	uint16_t on_time;
} echo_led_state_ind_t;

typedef struct
{
	bool led_on;
	uint32_t led_tick;
	uint32_t led_timeout_tick;
	bool led_timed_out;
	echo_led_sate_t led_state;
} echo_led_state_data_t;
echo_led_state_data_t echo_led_state;

const echo_led_state_ind_t echo_led_indication_table[echo_led_state_max] =
{
/* aul_led_state_none */
{ echo_led_color_none, 0xFFFF, 0 },

/* aul_led_idle */
{ echo_led_green, 0xFFFF, 0 },

/* aul_led_charging */
{ echo_led_green, 0, 0xFFFF } };

__STATIC_INLINE void Echo_LED_Green_Off(void)
{
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
}

__STATIC_INLINE void Echo_LED_Green_On(void)
{
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
}

__STATIC_INLINE void Echo_LED_Off_All(void)
{
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
}

__STATIC_INLINE void Echo_LED_State_Reset(void)
{
	ECHO_LED_CUR_STATE = echo_led_state_none;
	ECHO_LED_ON = false;
	Echo_LED_Off_All();
}

static void Echo_LED_CTRL(echo_led_color_t colors)
{
	if (colors == echo_led_color_none)
	{
		Echo_LED_Off_All();
	}
	else
	{
#ifdef LED_RED_EN
	if (colors & echo_led_red)
	Echo_LED_Red_On();
	else
	Echo_LED_Red_Off();
#endif
#ifdef LED_GREEN_EN
		if (colors & echo_led_green)
			Echo_LED_Green_On();
		else
			Echo_LED_Green_Off();
#endif
#ifdef LED_BLUE_EN
	if (colors & echo_led_blue)
	Echo_LED_Blue_On();
	else
	Echo_LED_Blue_Off();
#endif
	}
}

void Echo_LED_Init(void)
{
	Echo_LED_Off_All();
	ECHO_LED_CUR_STATE = echo_led_state_none;
}

void Echo_LED_Enable(void)
{
#ifdef DEBUG
	TD_SHELL_PRINT(("Echo_LED_Enable()\r\n"));
#endif
	Echo_LED_State_Reset();
	ECHO_LED_STATE_IND_TIMEOUT_RESET();
}

bool Echo_LED_Indication_Disable(void)
{
	return ECHO_LED_IND_DISABLED();
}

void Echo_Set_LED_State(echo_led_sate_t led_state)
{
	echo_led_state_ind_t led_ind;
#ifdef DEBUG
	TD_SHELL_PRINT(("Echo_LED_StateSet()\r\n"));
#endif
	Echo_LED_Enable();

	if (ECHO_LED_IND_DISABLED() == true)
		return;

	if (led_state >= echo_led_state_max)
		led_state = echo_led_state_none;

	if (led_state == ECHO_LED_CUR_STATE)
	{
		return;
	}

	led_ind.led_colors = ECHO_LED_STATE_IND_COLORS_GET(led_state);
	led_ind.off_time = ECHO_LED_STATE_IND_OFF_TIME_GET(led_state);
	led_ind.on_time = ECHO_LED_STATE_IND_ON_TIME_GET(led_state);

	Echo_LED_CTRL(echo_led_color_none);

	/* Check steady on or off */
	if (led_ind.off_time == ECHO_LED_STEADY_OFF_TIME)
	{
		ECHO_LED_ON = false;
		ECHO_LED_TIME_TICK = 0;
	}
	else
	{
		Echo_LED_CTRL(led_ind.led_colors);
		ECHO_LED_ON = true;
		ECHO_LED_TIME_TICK = HAL_GetTick();
	}

	ECHO_LED_CUR_STATE = led_state;

}
void Echo_LED_State_Refresh(void)
{
#ifdef DEBUG
	ECHO_SHELL_PRINT(("Echo_LED_State_Refresh()\r\n"));
#endif

	/* Force to change LED state */
	ECHO_LED_CUR_STATE = echo_led_state_none;

	/* Set LED stat as APP state */
	switch (Echo_Get_Sys_FSM_State())
	{
	case echo_sys_state_idle:
	case echo_sys_state_error:
		Echo_Set_LED_State(echo_led_idle);
		break;

	case echo_sys_state_run:
		Echo_Set_LED_State(echo_led_run);
		break;

	default:
		/* Cannot be here */
		Echo_Set_LED_State(echo_led_state_none);
		break;
	}
}
void Echo_LED_Handle(void)
{
	echo_led_state_ind_t led_ind;

	if (ECHO_LED_CUR_STATE == echo_led_state_none)
	{
		return;
	}
	if (ECHO_LED_CUR_STATE >= echo_led_state_max)
	{
		Echo_LED_State_Reset();
		return;
	}

	/* Check timeout of LED indication */
	if (ECHO_LED_TIMEOUT_TICK == ECHO_LED_IND_TIMEOUT)
	{
#ifdef DEBUG
		TD_SHELL_PRINT(("LED IND Timeout\r\n"));
#endif
		Echo_LED_State_Reset();
		ECHO_LED_TIMED_OUT = true;
		return;
	}
	else if (ECHO_LED_TIMEOUT_TICK < ECHO_LED_IND_TIMEOUT)
	{
		ECHO_LED_TIMEOUT_TICK++;
	}
	else
	{
		return;
	}

	led_ind.led_colors = ECHO_LED_STATE_IND_COLORS_GET(ECHO_LED_CUR_STATE);
	led_ind.off_time = ECHO_LED_STATE_IND_OFF_TIME_GET(ECHO_LED_CUR_STATE);
	led_ind.on_time = ECHO_LED_STATE_IND_ON_TIME_GET(ECHO_LED_CUR_STATE);

	if (led_ind.off_time == ECHO_LED_STEADY_OFF_TIME
			|| led_ind.off_time == ECHO_LED_STEADY_ON_TIME)
	{
		return;
	}

	if (ECHO_LED_ON == false
			&& HAL_GetTick() - ECHO_LED_TIME_TICK >= led_ind.off_time)
	{
		Echo_LED_CTRL(led_ind.led_colors);
		ECHO_LED_ON = true;
		ECHO_LED_TIME_TICK = HAL_GetTick();
	}
	else if (ECHO_LED_ON == true
			&& HAL_GetTick() - ECHO_LED_TIME_TICK >= led_ind.on_time)
	{
		Echo_LED_CTRL(echo_led_state_none);
		ECHO_LED_ON = false;
		ECHO_LED_TIME_TICK = HAL_GetTick();
	}
}
