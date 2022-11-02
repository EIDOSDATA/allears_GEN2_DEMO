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
	uint16_t pulse_freq;		// Hz
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

#endif /* INC_APP_ECHO_STIM_SETTING_H_ */
