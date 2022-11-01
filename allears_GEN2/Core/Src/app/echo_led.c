/*
 * echo_led.c
 *
 *  Created on: Oct 24, 2022
 *      Author: ECHO
 */

#include "main.h"
//#include "echo_btn.h"
#include "echo_led.h"
#include "echo_private.h"
#include "echo_schedule.h"
#include "echo_state.h"

#define ECHO_LED_STEADY_OFF_TIME						0xFFFF
#define ECHO_LED_STEADY_ON_TIME							0

#define ECHO_LED_IND_TIMEOUT							0xFFFF //((ECHO_DELAY_3SEC * 10) / ECHO_SCHED_HANDLE_PERIOD) 	/* 10s */
#define ECHO_LED_COLOR_ALL								(ECHO_LED_RED | ECHO_LED_GREEN | ECHO_LED_BLUE)

#define ECHO_LED_STATE_IS_EVENT(param)					(aul_led_low_batt <= param && param < ECHO_LED_STATE_MAX)

#define ECHO_LED_CUR_STATE								echo_led_state.led_state
#define ECHO_LED_ON										echo_led_state.led_on
#define ECHO_LED_TIME_TICK 								echo_led_state.led_tick
#define ECHO_LED_TIMEOUT_TICK 							echo_led_state.led_timeout_tick
#define ECHO_LED_TIMED_OUT	 							echo_led_state.led_timed_out

#define ECHO_LED_STATE_IND_COLORS_GET(param)			echo_led_indication_table[param].led_colors
#define ECHO_LED_STATE_IND_OFF_TIME_GET(param)			echo_led_indication_table[param].off_time
#define ECHO_LED_STATE_IND_ON_TIME_GET(param)			echo_led_indication_table[param].on_time

#define ECHO_LED_STATE_IND_TIMEOUT_APPLIED(param)		(param)

#define ECHO_LED_STATE_IND_TIMEOUT_RESET()				{ECHO_LED_TIMEOUT_TICK = 0; ECHO_LED_TIMED_OUT = false;}

#define ECHO_LED_IND_DISABLED()							(ECHO_LED_TIMED_OUT == true)

typedef enum
{
	ECHO_LED_COLOR_NONE = 0x00, ECHO_LED_RED = (1 << 0), ECHO_LED_GREEN = (1
			<< 1), ECHO_LED_BLUE = (1 << 2)
//ECHO_LED_WHITE = (aul_led_red| aul_led_green | aul_led_blue)
} echo_led_color_t;

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

const echo_led_state_ind_t echo_led_indication_table[ECHO_LED_STATE_MAX] =
{
/* aul_led_state_none */
{ ECHO_LED_COLOR_NONE, 0xFFFF, 0 },

/* aul_led_idle */
{ ECHO_LED_GREEN, 0xFFFF, 0 },

/* aul_led_charging */
{ ECHO_LED_GREEN, 0, 0xFFFF } };

__STATIC_INLINE void Echo_LED_Green_Off(void)
{
	HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
}

__STATIC_INLINE void Echo_LED_Green_On(void)
{
	HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
}

__STATIC_INLINE void Echo_LED_Off_All(void)
{
	HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
}

__STATIC_INLINE void Echo_LED_State_Reset(void)
{
	ECHO_LED_CUR_STATE = ECHO_LED_STATE_NONE;
	ECHO_LED_ON = false;
	Echo_LED_Off_All();
}

static void Echo_LED_CTRL(echo_led_color_t colors)
{
	if (colors == ECHO_LED_COLOR_NONE)
	{
		Echo_LED_Off_All();
	}
	else
	{
#ifdef LED_RED_EN
	if (colors & ECHO_LED_RED)
	Echo_LED_Red_On();
	else
	Echo_LED_Red_Off();
#endif
#ifdef LED_GREEN_EN
		if (colors & ECHO_LED_GREEN)
			Echo_LED_Green_On();
		else
			Echo_LED_Green_Off();
#endif
#ifdef LED_BLUE_EN
	if (colors & ECHO_LED_BLUE)
	Echo_LED_Blue_On();
	else
	Echo_LED_Blue_Off();
#endif
	}
}

void Echo_LED_Init(void)
{
	Echo_LED_Off_All();
	ECHO_LED_CUR_STATE = ECHO_LED_STATE_NONE;
}

void Echo_LED_Enable(void)
{
#ifdef DEBUG
	ECHO_SHELL_PRINT(("Echo_LED_Enable()\r\n"));
#endif
	Echo_LED_State_Reset();
	ECHO_LED_STATE_IND_TIMEOUT_RESET();
}

bool Echo_LED_Indication_Disable(void)
{
	return ECHO_LED_IND_DISABLED();
}

void Echo_LED_StateSet(echo_led_sate_t led_state)
{
	echo_led_state_ind_t led_ind;
#ifdef DEBUG
	ECHO_SHELL_PRINT(("Echo_LED_StateSet()\r\n"));
#endif
	Echo_LED_Enable();

	if (ECHO_LED_IND_DISABLED() == true)
		return;

	if (led_state >= ECHO_LED_STATE_MAX)
		led_state = ECHO_LED_STATE_NONE;

	if (led_state == ECHO_LED_CUR_STATE)
	{
		return;
	}

	led_ind.led_colors = ECHO_LED_STATE_IND_COLORS_GET(led_state);
	led_ind.off_time = ECHO_LED_STATE_IND_OFF_TIME_GET(led_state);
	led_ind.on_time = ECHO_LED_STATE_IND_ON_TIME_GET(led_state);

	Echo_LED_CTRL(ECHO_LED_COLOR_NONE);

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
	ECHO_LED_CUR_STATE = ECHO_LED_STATE_NONE;

	/* Set LED stat as APP state */
	switch (Echo_Get_FSM_State())
	{
	case ECHO_STATE_IDLE:
	case ECHO_STATE_ERROR:
		Echo_LED_StateSet(ECHO_LED_IDLE);
		break;

	case ECHO_STATE_RUN:
		Echo_LED_StateSet(ECHO_LED_RUN);
		break;

	default:
		/* Cannot be here */
		Echo_LED_StateSet(ECHO_LED_STATE_NONE);
		break;
	}
}
void Echo_LED_Handle(void)
{
	echo_led_state_ind_t led_ind;

	if (ECHO_LED_CUR_STATE == ECHO_LED_STATE_NONE)
	{
		return;
	}
	if (ECHO_LED_CUR_STATE >= ECHO_LED_STATE_MAX)
	{
		Echo_LED_State_Reset();
		return;
	}

	/* Check timeout of LED indication */
	if (ECHO_LED_TIMEOUT_TICK == ECHO_LED_IND_TIMEOUT)
	{
#ifdef DEBUG
		ECHO_SHELL_PRINT(("LED IND Timeout\r\n"));
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
		Echo_LED_CTRL(ECHO_LED_STATE_NONE);
		ECHO_LED_ON = false;
		ECHO_LED_TIME_TICK = HAL_GetTick();
	}
}
