/*
 * echo_pwm_setting.h
 *
 *  Created on: 2022. 10. 20.
 *      Author: ECHO
 */

#ifndef INC_APP_ECHO_STIM_SETTING_H_
#define INC_APP_ECHO_STIM_SETTING_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
/* SAVE DATA STRUCT */
typedef struct
{
	uint16_t dead_time;		// TIME
	uint16_t pulse_width;	// TIME
	uint16_t pulse_freq;	// Hz
} pwm_pulse_param_t;

typedef struct
{
	uint32_t prescaler;
	uint32_t pulse_period;
	uint32_t pulse_width;
	uint32_t pulse_oc;
} timer_param_t;

void Echo_Set_DT(uint8_t *data, uint16_t len);
void Echo_Set_PW(uint8_t *data, uint16_t len);
void Echo_Set_HZ(uint8_t *data, uint16_t len);
void Echo_Set_V_PW(uint8_t *data, uint16_t len);

void Echo_VPW_SET_TP_ON(void);
void Echo_VPW_TP_OFF(void);

void Echo_Get_Res_Data(uint8_t select_msg);

// STIM
void Echo_Stim_Stop(void);
void Echo_Stim_Start(void);

// STEP UP
void Echo_StepUP_Stop(void);
void Echo_StepUP_Start(void);

// PULSE DT, PW CONFIG
void Echo_Pulse_Prm_Config(void);
// PULSE VOLTAGE PW CONFIG
void Echo_Pulse_V_PW_Config(void);

// Factory RESET
void Echo_Factory_Reset(void);

// PULSE HZ CONFIG
void Echo_Pulse_FREQ_Config(void);

#define MASTER_CLK_FREQ					80000000
#define MASTER_PSC						80
#define MASTER_ARR						1000000
#define ECHO_SET_HZ_PERIOD				10

#define GLICH_DEBOUNCING_TIME			5
#define PULSE_HZ_FREQ					pwm_param.pulse_freq
#define PULSE_WIDTH_TIME				pwm_param.pulse_width
#define PULSE_DEAD_TIME					pwm_param.dead_time

#define CURRENT_CTRL_TIME0				PULSE_WIDTH_TIME +  GLICH_DEBOUNCING_TIME
#define CURRENT_CTRL_TIME1				PULSE_WIDTH_TIME + PULSE_DEAD_TIME + GLICH_DEBOUNCING_TIME
#define CURRENT_CTRL_TIME2				(PULSE_WIDTH_TIME * 2) + PULSE_DEAD_TIME + GLICH_DEBOUNCING_TIME // 2PW + D + S
#define CURRENT_CTRL_TIME3				GLICH_DEBOUNCING_TIME

#define ANODE_PULSE_TIME				PULSE_WIDTH_TIME + (GLICH_DEBOUNCING_TIME * 2)

#define CATHODE_PULSE_TIME0				(PULSE_WIDTH_TIME * 2) + PULSE_DEAD_TIME + (GLICH_DEBOUNCING_TIME * 2)
#define CATHODE_PULSE_TIME1				PULSE_WIDTH_TIME + PULSE_DEAD_TIME

#define PULSE_FREQ_ARR					MASTER_ARR / PULSE_HZ_FREQ
#define TOTAL_PULSE_WIDTH_TIME			(2*GLICH_DEBOUNCING_TIME) + (2*PULSE_WIDTH_TIME) + PULSE_DEAD_TIME

#define VOLTAGE_STEP_TARGET_VALUE		10

#endif /* INC_APP_ECHO_STIM_SETTING_H_ */
