/*
 * echo_stim_fdbk.h
 *
 *  Created on: Oct 28, 2022
 *      Author: ECHO
 */

#ifndef INC_APP_TD_ADC_H_
#define INC_APP_TD_ADC_H_

#include "main.h"

typedef enum
{
	td_adc1_state_init = 0x00,
	td_adc1_idle,
	td_adc1_run,
	td_adc1_conv_ok,
	td_adc1_print_ok,
	td_adc1_error,
	td_adc1_state_max
} td_adc1_state_t;

typedef enum
{
	td_adc2_state_init = 0x00,
	td_adc2_idle,
	td_adc2_run,
	td_adc2_conv_ok,
	td_adc2_print_ok,
	td_adc2_error,
	td_adc2_state_max
} td_adc2_state_t;

#define ADC_VDDA									3000			/* 3.0 V */
#define ADC_MAX_VAL									4096			/* ADC is 12bit resolution */
#define ADC_CONV_WAIT_TIME_MAX						10				/* ms */
#define PWR_HANDLE_PERIOD							1				/* ms */

#define R1_Vstup		3600.f //680.f;
#define R2_Vstup		110.f //22.f;
#define R1_Ved			18.f
#define R2_Ved			1.f

/* Voltage Scale : 1 uV */
#define PEAKDETECTION_VOLTAGE_SCALE							1000
#define STEPUP_VOLTAGE_SCALE								1000000
#define VOLTAGE_ERROR_RANGE_VALUE					1.8 * STEPUP_VOLTAGE_SCALE

#define ADC1_CHK_CH_NUM								1
#define ADC2_CHK_CH_NUM								1

#define TD_ADC1_RCV_SIZE							10
#define TD_ADC2_RCV_SIZE							10

#define ADC1_CONV_BUF								get_adc1_buf
#define ADC2_CONV_BUF								get_adc2_buf

void td_ADC1_Enable(void);
void td_ADC2_Enable(void);
void td_ADC_State_Init(void);

void td_Start_ADC1_Conv(void);
void td_Start_ADC2_Conv(void);

void td_Stop_ADC1_Conv(void);
void td_Stop_ADC2_Conv(void);

uint32_t td_Stepup_ADC1_AVG(void);
uint32_t td_Peak_Detection_ADC2_AVG(void);

uint32_t td_ADC_Calc_Stepup_V(uint32_t in_adc_val, float r1, float r2);
uint32_t td_ADC_Calc_Peak_V(uint32_t in_adc_val);

void Echo_ADC_Handle(void);

td_adc1_state_t td_Get_ADC1_State(void);
td_adc2_state_t td_Get_ADC2_State(void);
void td_Set_ADC1_State(td_adc1_state_t state);
void td_Set_ADC2_State(td_adc2_state_t state);
#endif /* INC_APP_TD_ADC_H_ */
