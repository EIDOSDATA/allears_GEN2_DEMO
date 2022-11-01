#ifndef	INC_APP_AUL_STATE_H_
#define INC_APP_AUL_STATE_H_

typedef enum
{
	td_sys_state_init = 0x00,
	td_sys_state_idle,
	td_sys_state_run,
	td_sys_state_error,
	td_sys_state_max
} td_sys_state_t;

#define TD_CUR_SYS_STATE								td_sys_fsm_state.state
#define TD_BUTTON_STATE 1

void td_Sys_FSM_State_Init(void);

td_sys_state_t td_Get_Sys_FSM_State(void);

void td_Set_Sys_FSM_State_Start(void);
void td_Set_Sys_FSM_State_Stop(void);

void td_Sys_FSM_State_Handle(void);
void td_Set_Sys_FSM_State(td_sys_state_t state);

#endif	/* AUL_STATE_H */

