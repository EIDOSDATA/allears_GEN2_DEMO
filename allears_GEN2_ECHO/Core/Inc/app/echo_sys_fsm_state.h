#ifndef	INC_APP_AUL_STATE_H_
#define INC_APP_AUL_STATE_H_

typedef enum
{
	echo_sys_state_init = 0x00,
	echo_sys_state_idle,
	echo_sys_state_run,
	echo_sys_state_error,
	echo_sys_state_max
} echo_sys_state_t;

#define ECHO_CUR_SYS_STATE								echo_sys_fsm_state.state
#define ECHO_BUTTON_STATE 1

void Echo_Sys_FSM_State_Init(void);

echo_sys_state_t Echo_Get_Sys_FSM_State(void);

void Echo_Set_Sys_FSM_State_Start(void);
void Echo_Set_Sys_FSM_State_Stop(void);

void Echo_Sys_FSM_State_Handle(void);
void Echo_Set_Sys_FSM_State(echo_sys_state_t state);

#endif	/* AUL_STATE_H */

