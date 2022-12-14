#include <td_adc.h>
#include <td_btn.h>
#include <td_flash_memory.h>
#include <td_led.h>
#include <td_private.h>
#include <td_schedule.h>
#include <td_shell.h>
#include <td_stim_setting.h>
#include <td_sys_common.h>
#include <td_sys_fsm_state.h>
#include "main.h"

typedef struct
{
	td_sys_state_t state;
} td_sys_state_data_t;
td_sys_state_data_t td_sys_fsm_state;
td_sys_state_t cur_state = td_sys_state_idle;

extern td_adc1_state_t ex_adc1_cur_state;
extern td_adc1_state_t ex_adc2_cur_state;
/* while out code*/
void td_Sys_FSM_State_Init(void)
{
	TD_CUR_SYS_STATE = td_sys_state_max;
	td_Set_Sys_FSM_State(td_sys_state_init);
}

td_sys_state_t td_Get_Sys_FSM_State(void)
{
	return TD_CUR_SYS_STATE;
}

void td_Set_Sys_FSM_State_Start()
{
	cur_state = td_sys_state_run;
}

void td_Set_Sys_FSM_State_Stop()
{
	cur_state = td_sys_state_idle;
}

void td_Sys_FSM_State_Handle(void)
{
	/* BUTTON PRESSED FLAG */
	if (td_Btn_IsHandled() == true)
	{
		if (cur_state == td_sys_state_idle)
		{
			cur_state = td_sys_state_run;
		}
		else if (cur_state == td_sys_state_run)
		{
			cur_state = td_sys_state_idle;
		}
	}

	if (TD_CUR_SYS_STATE != cur_state)
	{
		td_Set_Sys_FSM_State(cur_state);
		td_Btn_Handled_Clear();
	}
}

void td_Set_Sys_FSM_State(td_sys_state_t state)
{
	/*
	 if (TD_CUR_STATE == state || state >= td_sys_state_max)
	 return;
	 */
	switch (state)
	{
	case td_sys_state_init:
#ifdef DEBUG
#ifdef TD_PULSE_INTERRUPT
		TD_SHELL_PRINT(("TD STATE INIT\r\n"));
#endif
#endif
		break;

	case td_sys_state_idle:
		//td_Stim_Stop();
		td_Set_LED_State(td_led_idle);
		//ex_adc1_cur_state = td_adc1_idle;
		//ex_adc2_cur_state = td_adc2_idle;
#ifdef DEBUG
#ifdef TD_PULSE_INTERRUPT
		TD_SHELL_PRINT(("TD STATE IDLE\r\n"));
#endif
#endif
		break;

	case td_sys_state_run:
		//td_Stim_Start();
		td_Set_LED_State(td_led_run);
		//ex_adc1_cur_state = td_adc1_run;
		//ex_adc2_cur_state = td_adc2_run;
#ifdef DEBUG
#ifdef TD_PULSE_INTERRUPT
		TD_SHELL_PRINT(("TD STATE RUN\r\n"));
#endif
#endif
		break;

	case td_sys_state_error:
		td_Set_LED_State(td_led_idle);
		//ex_adc1_cur_state = td_adc1_error;
		//ex_adc2_cur_state = td_adc2_error;
#ifdef DEBUG
#ifdef TD_PULSE_INTERRUPT
		TD_SHELL_PRINT(("TD STATE ERROR\r\n"));
#endif
#endif
		break;

	default:
		// Cannot be here
#ifdef DEBUG
		TD_SHELL_PRINT(("UNKNOWN TD STATE: %d\n", state));
#endif
		break;

	}
	TD_CUR_SYS_STATE = state;
}
