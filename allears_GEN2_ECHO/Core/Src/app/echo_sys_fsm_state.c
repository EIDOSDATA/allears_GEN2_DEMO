#include <echo_adc.h>
#include <echo_btn.h>
#include <echo_flash_memory.h>
#include <echo_led.h>
#include <echo_private.h>
#include <echo_schedule.h>
#include <echo_shell.h>
#include <echo_stim_setting.h>
#include <echo_sys_common.h>
#include <echo_sys_fsm_state.h>
#include "main.h"

typedef struct
{
	echo_sys_state_t state;
} echo_sys_state_data_t;
echo_sys_state_data_t echo_sys_fsm_state;
echo_sys_state_t cur_state = echo_sys_state_idle;

extern echo_adc1_state_t ex_adc1_cur_state;
/* while out code*/
void Echo_Sys_FSM_State_Init(void)
{
	ECHO_CUR_SYS_STATE = echo_sys_state_max;
	//ECHO_BUTTON_STATE = Echo_Button_NoPressed();
	Echo_Set_Sys_FSM_State(echo_sys_state_init);
}

echo_sys_state_t Echo_Get_Sys_FSM_State(void)
{
	return ECHO_CUR_SYS_STATE;
}

void Echo_Set_Sys_FSM_State_Start()
{
	cur_state = echo_sys_state_run;
}

void Echo_Set_Sys_FSM_State_Stop()
{
	cur_state = echo_sys_state_idle;
}

void Echo_Sys_FSM_State_Handle(void)
{
	/* BUTTON PRESSED FLAG */
	if (Echo_Btn_IsHandled() == true)
	{
		if (cur_state == echo_sys_state_idle)
		{
			cur_state = echo_sys_state_run;
		}
		else if (cur_state == echo_sys_state_run)
		{
			cur_state = echo_sys_state_idle;
		}
	}

	if (ECHO_CUR_SYS_STATE != cur_state)
	{
		Echo_Set_Sys_FSM_State(cur_state);
		Echo_Btn_Handled_Clear();
	}

	/* SHELL COMMAND FLAG */
	/*
	 if (Echo_Shell_isHandled() == true)
	 {
	 if (cur_state == ECHO_STATE_IDLE)
	 {
	 cur_state = ECHO_STATE_RUN;
	 }
	 else if (cur_state == ECHO_STATE_RUN)
	 {
	 cur_state = ECHO_STATE_IDLE;
	 }
	 Echo_Shell_Handled_clear();
	 }
	 */

	/*
	 if (state_led_change == true)
	 {
	 Echo_LED_Enable();
	 Echo_LED_State_Refresh();
	 }
	 */
}

void Echo_Set_Sys_FSM_State(echo_sys_state_t state)
{
	/*
	 if (ECHO_CUR_STATE == state || state >= echo_state_max)
	 return;
	 */

	switch (state)
	{

	case echo_sys_state_init:
#ifdef DEBUG
#ifdef ECHO_PULSE_INTERRUPT
		ECHO_SHELL_PRINT(("ECHO STATE INIT\r\n"));
#endif
#endif
		break;

	case echo_sys_state_idle:
		Echo_Stim_Stop();
		Echo_Set_LED_State(echo_led_idle);
		ex_adc1_cur_state = echo_adc1_idle;
#ifdef DEBUG
#ifdef ECHO_PULSE_INTERRUPT
		ECHO_SHELL_PRINT(("ECHO STATE IDLE\r\n"));
#endif
#endif
		break;

	case echo_sys_state_run:
		Echo_Stim_Start();
		Echo_Set_LED_State(echo_led_run);
		ex_adc1_cur_state = echo_adc1_run;
#ifdef DEBUG
#ifdef ECHO_PULSE_INTERRUPT
		ECHO_SHELL_PRINT(("ECHO STATE RUN\r\n"));
#endif
#endif
		break;

	case echo_sys_state_error:
		Echo_Set_LED_State(echo_led_idle);
		ex_adc1_cur_state = echo_adc1_error;
#ifdef DEBUG
#ifdef ECHO_PULSE_INTERRUPT
		ECHO_SHELL_PRINT(("ECHO STATE ERROR\r\n"));
#endif
#endif
		break;

	default:
		// Cannot be here
#ifdef DEBUG
		ECHO_SHELL_PRINT(("UNKNOWN ECHO STATE: %d\n", state));
#endif
		break;

	}
	ECHO_CUR_SYS_STATE = state;
}

