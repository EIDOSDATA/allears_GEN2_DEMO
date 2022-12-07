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
void Echo_Set_Voltage_Output(uint8_t *data, uint16_t len);
void Echo_Set_Current_Strength(uint8_t *data, uint16_t len);

/*TEST*/
#define Echo_VPW_SET_TP_ON()				HAL_GPIO_WritePin(DAC0_GPIO_Port, DAC0_Pin, GPIO_PIN_SET)
#define Echo_VPW_SET_TP_OFF()				HAL_GPIO_WritePin(DAC0_GPIO_Port, DAC0_Pin, GPIO_PIN_RESET)
#define Echo_VPW_SET_TP_TOGGLE()			HAL_GPIO_TogglePin(DAC0_GPIO_Port, DAC0_Pin)
/*********/

void Echo_Get_Res_Data(uint8_t select_msg);

/* STIM */
void Echo_Stim_Stop(void);
void Echo_Stim_Start(void);

/* STEP UP */
void Echo_StepUP_Stop(void);
void Echo_StepUP_Start(void);

/* PULSE DT, PW CONFIG */
void Echo_Pulse_Prm_Config(void);

/* PULSE VOLTAGE PW CONFIG */
void Echo_Pulse_V_PW_Config(void);
uint32_t Echo_Voltage_Config(uint64_t adc_voltage);

/* Factory RESET */
void Echo_Factory_Reset(void);

/* PULSE HZ CONFIG */
void Echo_Pulse_FREQ_Config(void);

#define ECHO_MASTER_CLK_FREQ					80000000
#define ECHO_MASTER_PSC							80
#define ECHO_MASTER_ARR							1000000
#define ECHO_SET_HZ_PERIOD						10

#define ECHO_GLICH_DEBOUNCING_TIME				5
#define ECHO_PULSE_HZ_FREQ						ex_pwm_param.pulse_freq
#define ECHO_PULSE_WIDTH_TIME					ex_pwm_param.pulse_width
#define ECHO_PULSE_DEAD_TIME					ex_pwm_param.dead_time

#define ECHO_CURRENT_CTRL_TIME0					ECHO_PULSE_WIDTH_TIME +  ECHO_GLICH_DEBOUNCING_TIME
#define ECHO_CURRENT_CTRL_TIME1					ECHO_PULSE_WIDTH_TIME + ECHO_PULSE_DEAD_TIME + ECHO_GLICH_DEBOUNCING_TIME
#define ECHO_CURRENT_CTRL_TIME2					(ECHO_PULSE_WIDTH_TIME * 2) + ECHO_PULSE_DEAD_TIME + ECHO_GLICH_DEBOUNCING_TIME // 2PW + D + S
#define ECHO_CURRENT_CTRL_TIME3					ECHO_GLICH_DEBOUNCING_TIME

#define ECHO_ANODE_PULSE_TIME					ECHO_PULSE_WIDTH_TIME + (ECHO_GLICH_DEBOUNCING_TIME * 2)

#define ECHO_CATHODE_PULSE_TIME0				(ECHO_PULSE_WIDTH_TIME * 2) + ECHO_PULSE_DEAD_TIME + (ECHO_GLICH_DEBOUNCING_TIME * 2)
#define ECHO_CATHODE_PULSE_TIME1				ECHO_PULSE_WIDTH_TIME + ECHO_PULSE_DEAD_TIME

#define ECHO_PULSE_FREQ_ARR						ECHO_MASTER_ARR / ECHO_PULSE_HZ_FREQ
#define ECHO_TOTAL_PULSE_WIDTH_TIME				(2*ECHO_GLICH_DEBOUNCING_TIME) + (2*ECHO_PULSE_WIDTH_TIME) + ECHO_PULSE_DEAD_TIME

#define ECHO_CURRENT_DAC_ALL_OFF				HAL_GPIO_WritePin(GPIOA, 0x1E00, GPIO_PIN_RESET)
#define ECHO_CURRENT_DAC_ALL_ON					HAL_GPIO_WritePin(GPIOA, 0x1E00, GPIO_PIN_SET)
#define ECHO_CURRENT_DAC_CTRL_PIN				ECHO_CURRENT_STRENGTH_STEP << 9

#define ECHO_VOLTAGE_RELATED_PULSE_WIDTH		ex_voltage_r_pw
#define ECHO_VOLTAGE_VALUE_OUTPUT				ex_voltage_val_output
#define ECHO_CURRENT_STRENGTH_STEP				ex_current_strength_step
#define ECHO_SLOPE_CONTROL_END_FLAG				ex_slope_ctrl_end_f

#endif /* INC_APP_ECHO_STIM_SETTING_H_ */
