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
void td_Set_Current_Strength(uint8_t *data, uint16_t len);

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
uint32_t td_Voltage_Config(uint64_t adc_voltage);

/* Factory RESET */
void td_Factory_Reset(void);

/* GET TIMER PARAMETER */
int td_Get_Stim_PSC(void);
int td_Get_Stim_ARR(void);

int td_Get_Stepup_PSC(void);
int td_Get_Stepup_ARR(void);

/* SYSTEM MASTER CLOCK VALUE */
#define TD_MASTER_CLK_FREQ					HAL_RCC_GetHCLKFreq()
#define TD_MASTER_STIM_PSC					80
#define TD_MASTER_STEPUP_PSC				800
#define TD_TIMER_SCALE						(int)(80000000 / TD_MASTER_CLK_FREQ)
/* STIM TIMER */
#define TD_STIM_PSC							(int)(TD_MASTER_STIM_PSC / TD_TIMER_SCALE)
#define TD_STIM_FREQ_ARR					(int)(((TD_MASTER_CLK_FREQ / TD_STIM_PSC) / TD_PULSE_HZ_FREQ))
/* STEPUP TIMER */
#define TD_STEPUP_PSC						(int)(TD_MASTER_STEPUP_PSC / TD_TIMER_SCALE)
#define TD_STEPUP_FREQ_ARR					(int)(((TD_MASTER_CLK_FREQ / TD_STEPUP_PSC) / TD_STEPUP_HZ_FREQ))

#define TD_STEPUP_HZ_FREQ					10
#define TD_GLICH_DEBOUNCING_TIME			10 /* 10us */
#define TD_PULSE_HZ_FREQ					ex_pwm_param.pulse_freq
#define TD_PULSE_WIDTH_TIME					ex_pwm_param.pulse_width
#define TD_PULSE_DEAD_TIME					ex_pwm_param.dead_time

/* CURRENT CONTROL PULSE */
#define TD_CURRENT_CTRL_TIME0				(int)((TD_PULSE_WIDTH_TIME + TD_GLICH_DEBOUNCING_TIME) / 1)
#define TD_CURRENT_CTRL_TIME1				(int)((TD_PULSE_WIDTH_TIME + TD_PULSE_DEAD_TIME + TD_GLICH_DEBOUNCING_TIME) / 1)
#define TD_CURRENT_CTRL_TIME2				(int)(((TD_PULSE_WIDTH_TIME * 2) + TD_PULSE_DEAD_TIME + TD_GLICH_DEBOUNCING_TIME) / 1) // 2PW + D + S
#define TD_CURRENT_CTRL_TIME3				(int)(TD_GLICH_DEBOUNCING_TIME / 1)

/* ANODE CONTROL PULSE */
#define TD_ANODE_PULSE_TIME					(int)((TD_PULSE_WIDTH_TIME + (TD_GLICH_DEBOUNCING_TIME * 2)) / 1)

/* CATHODE CONTROL PULSE */
#define TD_CATHODE_PULSE_TIME0				(int)(((TD_PULSE_WIDTH_TIME * 2) + TD_PULSE_DEAD_TIME + (TD_GLICH_DEBOUNCING_TIME * 2)) / 1)
#define TD_CATHODE_PULSE_TIME1				(int)((TD_PULSE_WIDTH_TIME + TD_PULSE_DEAD_TIME) / 1)

/* ADC CONTROL TIMER VALUE */
#define TD_TOTAL_PULSE_WIDTH_TIME			(int)(((2*TD_GLICH_DEBOUNCING_TIME) + (2*TD_PULSE_WIDTH_TIME) + TD_PULSE_DEAD_TIME) / 1)
#define TD_TIM2_ADC2_TRG_TIME				tim2ch1_current_dma[0] + 100

#define TD_CURRENT_DAC_ALL_OFF				HAL_GPIO_WritePin(GPIOA, 0x1E00, GPIO_PIN_RESET)
#define TD_CURRENT_DAC_ALL_ON				HAL_GPIO_WritePin(GPIOA, 0x1E00, GPIO_PIN_SET)
#define TD_CURRENT_DAC_CONTROL				HAL_GPIO_WritePin(GPIOA, TD_CURRENT_STRENGTH_STEP << 9, GPIO_PIN_SET)

#define TD_VOLTAGE_RELATED_PULSE_WIDTH		ex_voltage_r_pw
#define TD_VOLTAGE_VALUE_OUTPUT				ex_voltage_val_output
#define TD_CURRENT_STRENGTH_STEP			ex_current_strength_step
#define TD_SLOPE_CONTROL_END_FLAG			ex_slope_ctrl_end_f

#endif /* INC_APP_TD_STIM_SETTING_H_ */
