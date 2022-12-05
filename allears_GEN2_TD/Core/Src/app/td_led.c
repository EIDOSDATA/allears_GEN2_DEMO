/*
 * td_led.c
 *
 *  Created on: Oct 24, 2022
 *      Author: ECHO
 */
#include <td_led.h>
#include <td_private.h>
#include <td_schedule.h>
#include <td_shell.h>
#include <td_sys_fsm_state.h>
#include "main.h"

typedef struct
{
	uint8_t led_colors;
	uint16_t off_time;
	uint16_t on_time;
} td_led_state_ind_t;

typedef struct
{
	bool led_on;
	uint32_t led_tick;
	uint32_t led_timeout_tick;
	bool led_timed_out;
	td_led_sate_t led_state;
} td_led_state_data_t;
td_led_state_data_t td_led_state;

const td_led_state_ind_t td_led_indication_table[td_led_state_max] =
{
/* aul_led_state_none */
{ td_led_color_none, 0xFFFF, 0 },

/* aul_led_idle */
{ td_led_green, 0xFFFF, 0 },

/* aul_led_charging */
{ td_led_green, 0, 0xFFFF } };

__STATIC_INLINE void st_td_LED_Green_Off(void)
{
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
}

__STATIC_INLINE void st_td_LED_Green_On(void)
{
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
}

__STATIC_INLINE void st_td_LED_Off_All(void)
{
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
}

__STATIC_INLINE void st_td_LED_State_Reset(void)
{
	TD_LED_CUR_STATE = td_led_state_none;
	TD_LED_ON = false;
	st_td_LED_Off_All();
}

static void st_td_LED_CTRL(td_led_color_t colors)
{
	if (colors == td_led_color_none)
	{
		st_td_LED_Off_All();
	}
	else
	{
#ifdef LED_RED_EN
	if (colors & td_led_red)
	td_LED_Red_On();
	else
	td_LED_Red_Off();
#endif
#ifdef LED_GREEN_EN
		if (colors & td_led_green)
			st_td_LED_Green_On();
		else
			st_td_LED_Green_Off();
#endif
#ifdef LED_BLUE_EN
	if (colors & td_led_blue)
	td_LED_Blue_On();
	else
	td_LED_Blue_Off();
#endif
	}
}

void td_LED_Init(void)
{
	st_td_LED_Off_All();
	TD_LED_CUR_STATE = td_led_state_none;
}

void td_LED_Enable(void)
{
#ifdef DEBUG
	TD_SHELL_PRINT(("td_LED_Enable()\r\n"));
#endif
	st_td_LED_State_Reset();
	TD_LED_STATE_IND_TIMEOUT_RESET();
}

bool td_LED_Indication_Disable(void)
{
	return TD_LED_IND_DISABLED();
}

void td_Set_LED_State(td_led_sate_t led_state)
{
	td_led_state_ind_t led_ind;
#ifdef DEBUG
	TD_SHELL_PRINT(("td_LED_StateSet()\r\n"));
#endif
	td_LED_Enable();

	if (TD_LED_IND_DISABLED() == true)
		return;

	if (led_state >= td_led_state_max)
		led_state = td_led_state_none;

	if (led_state == TD_LED_CUR_STATE)
	{
		return;
	}

	led_ind.led_colors = TD_LED_STATE_IND_COLORS_GET(led_state);
	led_ind.off_time = TD_LED_STATE_IND_OFF_TIME_GET(led_state);
	led_ind.on_time = TD_LED_STATE_IND_ON_TIME_GET(led_state);

	st_td_LED_CTRL(td_led_color_none);

	/* Check steady on or off */
	if (led_ind.off_time == TD_LED_STEADY_OFF_TIME)
	{
		TD_LED_ON = false;
		TD_LED_TIME_TICK = 0;
	}
	else
	{
		st_td_LED_CTRL(led_ind.led_colors);
		TD_LED_ON = true;
		TD_LED_TIME_TICK = HAL_GetTick();
	}

	TD_LED_CUR_STATE = led_state;

}
void td_LED_State_Refresh(void)
{
#ifdef DEBUG
	TD_SHELL_PRINT(("td_LED_State_Refresh()\r\n"));
#endif

	/* Force to change LED state */
	TD_LED_CUR_STATE = td_led_state_none;

	/* Set LED stat as APP state */
	switch (td_Get_Sys_FSM_State())
	{
	case td_sys_state_idle:
	case td_sys_state_error:
		td_Set_LED_State(td_led_idle);
		break;

	case td_sys_state_run:
		td_Set_LED_State(td_led_run);
		break;

	default:
		/* Cannot be here */
		td_Set_LED_State(td_led_state_none);
		break;
	}
}
void td_LED_Handle(void)
{
	td_led_state_ind_t led_ind;

	if (TD_LED_CUR_STATE == td_led_state_none)
	{
		return;
	}
	if (TD_LED_CUR_STATE >= td_led_state_max)
	{
		st_td_LED_State_Reset();
		return;
	}

	/* Check timeout of LED indication */
	if (TD_LED_TIMEOUT_TICK == TD_LED_IND_TIMEOUT)
	{
#ifdef DEBUG
		TD_SHELL_PRINT(("LED IND Timeout\r\n"));
#endif
		st_td_LED_State_Reset();
		TD_LED_TIMED_OUT = true;
		return;
	}
	else if (TD_LED_TIMEOUT_TICK < TD_LED_IND_TIMEOUT)
	{
		TD_LED_TIMEOUT_TICK++;
	}
	else
	{
		return;
	}

	led_ind.led_colors = TD_LED_STATE_IND_COLORS_GET(TD_LED_CUR_STATE);
	led_ind.off_time = TD_LED_STATE_IND_OFF_TIME_GET(TD_LED_CUR_STATE);
	led_ind.on_time = TD_LED_STATE_IND_ON_TIME_GET(TD_LED_CUR_STATE);

	if (led_ind.off_time == TD_LED_STEADY_OFF_TIME
			|| led_ind.off_time == TD_LED_STEADY_ON_TIME)
	{
		return;
	}

	if (TD_LED_ON == false
			&& HAL_GetTick() - TD_LED_TIME_TICK >= led_ind.off_time)
	{
		st_td_LED_CTRL(led_ind.led_colors);
		TD_LED_ON = true;
		TD_LED_TIME_TICK = HAL_GetTick();
	}
	else if (TD_LED_ON == true
			&& HAL_GetTick() - TD_LED_TIME_TICK >= led_ind.on_time)
	{
		st_td_LED_CTRL(td_led_state_none);
		TD_LED_ON = false;
		TD_LED_TIME_TICK = HAL_GetTick();
	}
}
