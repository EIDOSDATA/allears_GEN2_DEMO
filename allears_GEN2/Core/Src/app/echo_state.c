#include <echo_stim_setting.h>
#include "main.h"
#include "echo_btn.h"
#include "echo_flash_memory.h"
#include "echo_led.h"
#include "echo_private.h"
#include "echo_schedule.h"
#include "echo_shell.h"
#include "echo_state.h"
#include "echo_sys_common.h"
//#include "echo_adc.h"

#define ECHO_CUR_STATE								echo_fsm_state.state
#define ECHO_BUTTON_STATE 1

typedef struct
{
	echo_state_t state;
} echo_state_data_t;
echo_state_data_t echo_fsm_state;

echo_state_t cur_state = ECHO_STATE_IDLE;
/* while out code*/
void Echo_FSM_State_Init(void)
{
	ECHO_CUR_STATE = echo_state_max;
	//ECHO_BUTTON_STATE = Echo_Button_NoPressed(); //  aulGpio_hallSensor_Closed();
	Echo_Set_FSM_State(ECHO_STATE_INIT);
}

echo_state_t Echo_Get_FSM_State(void)
{
	return ECHO_CUR_STATE;
}

void Echo_Set_FSM_State_Start()
{
	cur_state = ECHO_STATE_RUN;
}

void Echo_Set_FSM_State_Stop()
{
	cur_state = ECHO_STATE_IDLE;
}

void Echo_FSM_State_Handle(void)
{
	//bool state_led_change = 0;

	/* VOLTAGE FEED BACK */

	/*
	 Guess current state
	 */
	/* BUTTON PRESSED FLAG */
	if (Echo_Btn_isHandled() == true)
	{
		if (cur_state == ECHO_STATE_IDLE)
		{
			cur_state = ECHO_STATE_RUN;
			//state_led_change = true;
		}
		else if (cur_state == ECHO_STATE_RUN)
		{
			cur_state = ECHO_STATE_IDLE;
			//state_led_change = true;
		}
	}
	else if (Echo_Btn_isHandled() == false)
	{
		cur_state = cur_state;
	}

	if (ECHO_CUR_STATE != cur_state)
	{
		Echo_Set_FSM_State(cur_state);
		Echo_Btn_Handled_clear();
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

	/*
	 If cover state is changed, activate LED indication.
	 If low battery, cover change event for LED is ingnored.
	 */
}

void Echo_Set_FSM_State(echo_state_t state)
{
	if (ECHO_CUR_STATE == state || state >= echo_state_max)
		return;

	switch (state)
	{
	case ECHO_STATE_INIT:
		ECHO_SHELL_PRINT(("ECHO STATE INIT\r\n"));
		break;

	case ECHO_STATE_IDLE:
		ECHO_SHELL_PRINT(("ECHO STATE IDLE\r\n"));
		Echo_LED_StateSet(ECHO_LED_IDLE);
		Echo_Stim_Stop();
		break;

	case ECHO_STATE_RUN:
		ECHO_SHELL_PRINT(("ECHO STATE RUN\r\n"));
		Echo_LED_StateSet(ECHO_LED_RUN);
		Echo_Stim_Start();
		break;

	case ECHO_STATE_ERROR:
		ECHO_SHELL_PRINT(("ECHO STATE ERROR\r\n"));
		Echo_LED_StateSet(ECHO_LED_IDLE);
		break;

	default:
		/* Cannot be here */
		ECHO_SHELL_PRINT(("UNKNOWN ECHO STATE: %d\n", state));
		//Echo_LedStateSet(ECHO_LED_NONE);
		break;
	}
	ECHO_CUR_STATE = state;
}

