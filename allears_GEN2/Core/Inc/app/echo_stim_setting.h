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

#define ANODE_PULSE_TIME				pwm_param.pulse_width
#define CATHODE_PULSE_TIME0				pwm_param.pulse_width + pwm_param.dead_time
#define CATHODE_PULSE_TIME1				(pwm_param.pulse_width * 2) + pwm_param.dead_time

#define CURRENT_CTRL_TIME0				PULSE_DEBOUNCING_TIME
#define CURRENT_CTRL_TIME1				ANODE_PULSE_TIME - PULSE_DEBOUNCING_TIME
#define CURRENT_CTRL_TIME2				CATHODE_PULSE_TIME0 + PULSE_DEBOUNCING_TIME
#define CURRENT_CTRL_TIME3				CATHODE_PULSE_TIME1 - PULSE_DEBOUNCING_TIME

#define PULSE_DEBOUNCING_TIME			5
#define VOLTAGE_STEP_TARGET_VALUE		5000

#endif /* INC_APP_ECHO_STIM_SETTING_H_ */
