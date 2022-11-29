#include "main.h"
#include "echo_adc.h"
#include "echo_btn.h"
#include "echo_flash_memory.h"
#include "echo_led.h"
#include "echo_private.h"
#include "echo_schedule.h"
#include "echo_shell.h"
#include "echo_stim_setting.h"
#include "echo_sys_common.h"
#include "echo_sys_fsm_state.h"
//#include "echo_adc.h"

#define ECHO_CUR_SYS_STATE								echo_sys_fsm_state.state
#define ECHO_BUTTON_STATE 1
typedef struct
{
	echo_sys_state_t state;
} echo_sys_state_data_t;
echo_sys_state_data_t echo_sys_fsm_state;
echo_sys_state_t cur_state = ECHO_SYS_STATE_IDLE;

extern echo_adc1_state_t adc1_cur_state;
/* while out code*/
void Echo_Sys_FSM_State_Init(void)
{
	ECHO_CUR_SYS_STATE = echo_sys_state_max;
	//ECHO_BUTTON_STATE = Echo_Button_NoPressed();
	Echo_Set_Sys_FSM_State(ECHO_SYS_STATE_INIT);
}

echo_sys_state_t Echo_Get_Sys_FSM_State(void)
{
	return ECHO_CUR_SYS_STATE;
}

void Echo_Set_Sys_FSM_State_Start()
{
	cur_state = ECHO_SYS_STATE_RUN;
}

void Echo_Set_Sys_FSM_State_Stop()
{
	cur_state = ECHO_SYS_STATE_IDLE;
}

void Echo_Sys_FSM_State_Handle(void)
{
	/* BUTTON PRESSED FLAG */
	if (Echo_Btn_IsHandled() == true)
	{
		if (cur_state == ECHO_SYS_STATE_IDLE)
		{
			cur_state = ECHO_SYS_STATE_RUN;
		}
		else if (cur_state == ECHO_SYS_STATE_RUN)
		{
			cur_state = ECHO_SYS_STATE_IDLE;
		}
	}

	if (ECHO_CUR_SYS_STATE != cur_state)
	{
		Echo_Set_Sys_FSM_State(cur_state);
		Echo_Btn_Handled_Clear();
	}
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

	case ECHO_SYS_STATE_INIT:
#ifdef DEBUG
#ifdef ECHO_PULSE_INTERRUPT
		ECHO_SHELL_PRINT(("ECHO STATE INIT\r\n"));
#endif
#endif
		break;

	case ECHO_SYS_STATE_IDLE:
		Echo_Stim_Stop();
		Echo_Set_LED_State(ECHO_LED_IDLE);
		adc1_cur_state = ECHO_ADC1_IDLE;
#ifdef DEBUG
#ifdef ECHO_PULSE_INTERRUPT
		ECHO_SHELL_PRINT(("ECHO STATE IDLE\r\n"));
#endif
#endif
		break;

	case ECHO_SYS_STATE_RUN:
		Echo_Stim_Start();
		Echo_Set_LED_State(ECHO_LED_RUN);
		adc1_cur_state = ECHO_ADC1_RUN;
#ifdef DEBUG
#ifdef ECHO_PULSE_INTERRUPT
		ECHO_SHELL_PRINT(("ECHO STATE RUN\r\n"));
#endif
#endif
		break;

	case ECHO_SYS_STATE_ERROR:
		Echo_Set_LED_State(ECHO_LED_IDLE);
		adc1_cur_state = ECHO_ADC1_ERROR;
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

