#ifndef	INC_APP_AUL_STATE_H_
#define INC_APP_AUL_STATE_H_

typedef enum
{
	ECHO_SYS_STATE_INIT = 0x00,
	ECHO_SYS_STATE_IDLE,
	ECHO_SYS_STATE_RUN,
	ECHO_SYS_STATE_ERROR,
	echo_sys_state_max
} echo_sys_state_t;

void Echo_Sys_FSM_State_Init(void);

echo_sys_state_t Echo_Get_Sys_FSM_State(void);

void Echo_Set_Sys_FSM_State_Start(void);
void Echo_Set_Sys_FSM_State_Stop(void);

void Echo_Sys_FSM_State_Handle(void);
void Echo_Set_Sys_FSM_State(echo_sys_state_t state);

#endif	/* AUL_STATE_H */

