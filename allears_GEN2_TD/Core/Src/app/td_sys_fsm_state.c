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
//#include "echo_adc.h"

#define ECHO_CUR_SYS_STATE								echo_sys_fsm_state.state
#define ECHO_BUTTON_STATE 1
typedef struct
{
	td_sys_state_t state;
} echo_sys_state_data_t;
echo_sys_state_data_t echo_sys_fsm_state;
td_sys_state_t cur_state = td_sys_state_idle;

extern td_adc1_state_t ex_adc1_cur_state;
/* while out code*/
void td_Sys_FSM_State_Init(void)
{
	ECHO_CUR_SYS_STATE = td_sys_state_max;
	//ECHO_BUTTON_STATE = Echo_Button_NoPressed();
	td_Set_Sys_FSM_State(td_sys_state_init);
}

td_sys_state_t td_Get_Sys_FSM_State(void)
{
	return ECHO_CUR_SYS_STATE;
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

	if (ECHO_CUR_SYS_STATE != cur_state)
	{
		td_Set_Sys_FSM_State(cur_state);
		td_Btn_Handled_Clear();
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

void td_Set_Sys_FSM_State(td_sys_state_t state)
{
	/*
	 if (ECHO_CUR_STATE == state || state >= echo_state_max)
	 return;
	 */

	switch (state)
	{

	case td_sys_state_init:
#ifdef DEBUG
#ifdef ECHO_PULSE_INTERRUPT
		TD_SHELL_PRINT(("ECHO STATE INIT\r\n"));
#endif
#endif
		break;

	case td_sys_state_idle:
		td_Stim_Stop();
		td_Set_LED_State(td_led_idle);
		ex_adc1_cur_state = td_adc1_idle;
#ifdef DEBUG
#ifdef ECHO_PULSE_INTERRUPT
		TD_SHELL_PRINT(("ECHO STATE IDLE\r\n"));
#endif
#endif
		break;

	case td_sys_state_run:
		td_Stim_Start();
		td_Set_LED_State(td_led_run);
		ex_adc1_cur_state = td_adc1_run;
#ifdef DEBUG
#ifdef ECHO_PULSE_INTERRUPT
		TD_SHELL_PRINT(("ECHO STATE RUN\r\n"));
#endif
#endif
		break;

	case td_sys_state_error:
		td_Set_LED_State(td_led_idle);
		ex_adc1_cur_state = td_adc1_error;
#ifdef DEBUG
#ifdef ECHO_PULSE_INTERRUPT
		TD_SHELL_PRINT(("ECHO STATE ERROR\r\n"));
#endif
#endif
		break;

	default:
		// Cannot be here
#ifdef DEBUG
		TD_SHELL_PRINT(("UNKNOWN ECHO STATE: %d\n", state));
#endif
		break;

	}
	ECHO_CUR_SYS_STATE = state;
}

