/*
 * echo_stim_fdbk.h
 *
 *  Created on: Oct 28, 2022
 *      Author: ECHO
 */

#ifndef INC_APP_ECHO_ADC_H_
#define INC_APP_ECHO_ADC_H_

#include "main.h"

typedef enum
{
	echo_adc1_state_init = 0x00,
	echo_adc1_idle,
	echo_adc1_run,
	echo_adc1_conv_ok,
	echo_adc1_print_ok,
	echo_adc1_error,
	echo_adc1_state_max
} echo_adc1_state_t;

typedef enum
{
	echo_adc2_state_init = 0x00,
	echo_adc2_idle,
	echo_adc2_run,
	echo_adc2_conv_ok,
	echo_adc2_print_ok,
	echo_adc2_error,
	echo_adc2_state_max
} echo_adc2_state_t;

/* REFERENCE VALUE1 */
#define ADC_VDDA									3000			/* 3.0 V */
#define ADC_MAX_VAL									4096			/* ADC is 12bit resolution */
#define ADC_CONV_WAIT_TIME_MAX						10				/* ms */
#define PWR_HANDLE_PERIOD							1				/* ms */

/* REFERENCE VALUE2 */
#define R1_Vstup									3600.f //680.f;
#define R2_Vstup									110.f //22.f;
#define R1_Ved										18.f
#define R2_Ved										1.f

/* REFERENCE TABLE */
/*
 #define ECHO_REF_VOLTAGE_TABLE						ref_voltage_table
 #define ECHO_REF_ADC_VALUE_TABLE					ref_adc_value_table
 #define ECHO_REF_ADC_VOLTAGE_TABLE					ref_adc_voltage_table
 */

/* ERROR RANGE */
/* Voltage Scale : 1 uV */
#define PEAKDETECTION_VOLTAGE_SCALE					1000
#define STEPUP_VOLTAGE_SCALE						1000000
#define VOLTAGE_ERROR_RANGE_VALUE					1.8 * STEPUP_VOLTAGE_SCALE

/* NUMBER OF ADC CHANNEL */
#define ADC1_CHK_CH_NUM								1
#define ADC2_CHK_CH_NUM								1

/* ADC READING SIZE */
#define ADC1_RCV_SIZE								10
#define ADC2_RCV_SIZE								10

/* ADC CONVERSION BUFFER SIZE*/
#define ECHO_ADC1_CONV_BUF							get_adc1_buf
#define ECHO_ADC2_CONV_BUF							get_adc2_buf

/* ADC FSM STATE */
#define ECHO_ADC1_CUR_STATE							echo_adc1_fsm_state.state
#define ECHO_ADC2_CUR_STATE							echo_adc2_fsm_state.state

void Echo_ADC1_Enable(void);
void Echo_ADC2_Enable(void);
void Echo_ADC_State_Init(void);

void Echo_Start_ADC1_Conv(void);
void Echo_Start_ADC2_Conv(void);

void Echo_Stop_ADC1_Conv(void);
void Echo_Stop_ADC2_Conv(void);

uint32_t Echo_Stepup_ADC1_AVG(void);
uint32_t Echo_Peak_Detection_ADC2_AVG(void);

uint32_t Echo_ADC_Calc_Stepup_V(uint32_t in_adc_val, uint32_t r1, uint32_t r2);
uint32_t Echo_ADC_Calc_Peak_V(uint32_t in_adc_val);

void Echo_ADC_Handle(void);

echo_adc1_state_t Echo_Get_ADC1_State(void);
echo_adc2_state_t Echo_Get_ADC2_State(void);
void Echo_Set_ADC1_State(echo_adc1_state_t state);
void Echo_Set_ADC2_State(echo_adc2_state_t state);
#endif /* INC_APP_ECHO_ADC_H_ */
