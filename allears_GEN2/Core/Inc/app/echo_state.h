#ifndef	INC_APP_AUL_STATE_H_
#define INC_APP_AUL_STATE_H_

typedef enum
{
	ECHO_STATE_INIT = 0x00,
	ECHO_STATE_IDLE,
	ECHO_STATE_RUN,
	ECHO_STATE_ERROR,
	echo_state_max
} echo_state_t;

void Echo_FSM_State_Init(void);

echo_state_t Echo_Get_FSM_State(void);

void Echo_Set_FSM_State_Start(void);
void Echo_Set_FSM_State_Stop(void);

void Echo_FSM_State_Handle(void);
void Echo_Set_FSM_State(echo_state_t state);

#endif	/* AUL_STATE_H */

