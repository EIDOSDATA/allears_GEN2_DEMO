/*
 * td_pwm_setting.h
 *
 *  Created on: 2022. 10. 20.
 *      Author: ECHO
 */

#ifndef INC_APP_TD_STIM_SETTING_H_
#define INC_APP_TD_STIM_SETTING_H_

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

void td_Set_DT(uint8_t *data, uint16_t len);
void td_Set_PW(uint8_t *data, uint16_t len);
void td_Set_HZ(uint8_t *data, uint16_t len);
void td_Set_V_PW(uint8_t *data, uint16_t len);
void td_Set_Voltage_Output(uint8_t *data, uint16_t len);

/*TEST*/
#define td_VPW_SET_TP_ON()				HAL_GPIO_WritePin(DAC0_GPIO_Port, DAC0_Pin, GPIO_PIN_SET)
#define td_VPW_SET_TP_OFF()				HAL_GPIO_WritePin(DAC0_GPIO_Port, DAC0_Pin, GPIO_PIN_RESET)
#define td_VPW_SET_TP_TOGGLE()			HAL_GPIO_TogglePin(DAC0_GPIO_Port, DAC0_Pin)
/*********/

void td_Get_Res_Data(uint8_t select_msg);

/* STIM */
void td_Stim_Stop(void);
void td_Stim_Start(void);

/* STEP UP */
void td_StepUP_Stop(void);
void td_StepUP_Start(void);

/* PULSE DT, PW CONFIG */
void td_Pulse_Prm_Config(void);

/* PULSE VOLTAGE PW CONFIG */
void td_Pulse_V_PW_Config(void);
uint32_t td_Voltage_Config(uint32_t adc_voltage);

/* Factory RESET */
void td_Factory_Reset(void);

/* PULSE HZ CONFIG */
void td_Pulse_FREQ_Config(void);

#define TD_MASTER_CLK_FREQ					80000000
#define TD_MASTER_PSC						80
#define TD_MASTER_ARR						1000000
#define TD_SET_HZ_PERIOD					10

#define TD_GLICH_DEBOUNCING_TIME			5
#define TD_PULSE_HZ_FREQ					ex_pwm_param.pulse_freq
#define TD_PULSE_WIDTH_TIME					ex_pwm_param.pulse_width
#define TD_PULSE_DEAD_TIME					ex_pwm_param.dead_time

#define TD_CURRENT_CTRL_TIME0				TD_PULSE_WIDTH_TIME +  TD_GLICH_DEBOUNCING_TIME
#define TD_CURRENT_CTRL_TIME1				TD_PULSE_WIDTH_TIME + TD_PULSE_DEAD_TIME + TD_GLICH_DEBOUNCING_TIME
#define TD_CURRENT_CTRL_TIME2				(TD_PULSE_WIDTH_TIME * 2) + TD_PULSE_DEAD_TIME + TD_GLICH_DEBOUNCING_TIME // 2PW + D + S
#define TD_CURRENT_CTRL_TIME3				TD_GLICH_DEBOUNCING_TIME

#define TD_ANODE_PULSE_TIME					TD_PULSE_WIDTH_TIME + (TD_GLICH_DEBOUNCING_TIME * 2)

#define TD_CATHODE_PULSE_TIME0				(TD_PULSE_WIDTH_TIME * 2) + TD_PULSE_DEAD_TIME + (TD_GLICH_DEBOUNCING_TIME * 2)
#define TD_CATHODE_PULSE_TIME1				TD_PULSE_WIDTH_TIME + TD_PULSE_DEAD_TIME

#define TD_PULSE_FREQ_ARR					TD_MASTER_ARR / TD_PULSE_HZ_FREQ
#define TD_TOTAL_PULSE_WIDTH_TIME			(2*TD_GLICH_DEBOUNCING_TIME) + (2*TD_PULSE_WIDTH_TIME) + TD_PULSE_DEAD_TIME

#define TD_VOLTAGE_RELATED_PULSE_WIDTH		ex_voltage_r_pw
#define TD_VOLTAGE_VALUE_OUTPUT				ex_voltage_val_output
#define TD_SLOPE_CONTROL_END_FLAG			ex_slope_ctrl_end_f

#endif /* INC_APP_TD_STIM_SETTING_H_ */