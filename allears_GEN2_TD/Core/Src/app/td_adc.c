/*
 * td_stepup_setting.c
 *
 *  Created on: Oct 28, 2022
 *      Author: ECHO
 */
#include <stdint.h>
#include <math.h>
#include <td_adc.h>
#include <td_shell.h>
#include <td_stim_setting.h>
#include <td_sys_fsm_state.h>
#include "main.h"

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern DMA_HandleTypeDef hdma_adc1;
extern DMA_HandleTypeDef hdma_adc2;

/* STEPUP VOLTAGE TABLE */
extern __IO int ex_voltage_r_pw;

/* REFERENCE TABLE */
#define TD_REF_VOLTAGE_TABLE				ref_voltage_table
#define TD_REF_ADC_VALUE_TABLE				ref_adc_value_table
#define TD_REF_ADC_VOLTAGE_TABLE			ref_adc_voltage_table

/* Voltage Scale : 10 uV >> ADC BUFFER IS 10 */
int ref_voltage_table[30] =
{ 360000, 660000, 880000, 1080000, 1320000, 1520000, 1760000, 1960000, 2160000,
		2380000, 2600000, 2800000, 3000000, 3200000, 3380000, 3560000, 3720000,
		3920000, 4080000, 4240000, 4400000, 4540000, 4680000, 4820000, 4940000,
		5080000, 5180000, 5260000, 5340000, 5420000 }; // 30

int ref_adc_value_table[30] =
{ 282, 340, 384, 428, 477, 540, 585, 630, 676, 720, 763, 803, 847, 888, 928,
		968, 1005, 1042, 1080, 1118, 1150, 1188, 1216, 1247, 1275, 1300, 1322,
		1342, 1357, 1373 }; //30

/* Voltage Scale : 10 uV >> ADC BUFFER IS 10 */
int ref_adc_voltage_table[30] =
{ 10000, 18000, 23000, 30000, 37000, 42000, 49000, 56000, 62000, 68000, 74000,
		80000, 86000, 92000, 97000, 103500, 107000, 112000, 119000, 122000,
		128000, 133000, 137000, 141000, 145000, 149000, 151000, 154000, 157000,
		159000 }; // 30
/**********************/

/* ADC BUFFER */
uint16_t get_adc1_buf[ADC1_CHK_CH_NUM * ADC1_RCV_SIZE];
uint16_t get_adc2_buf[ADC2_CHK_CH_NUM * ADC2_RCV_SIZE];

uint16_t ex_setpup_adc[ADC1_RCV_SIZE]; // ADC1
uint16_t ex_peak_adc_r[ADC2_RCV_SIZE]; // ADC2
uint16_t ex_peak_adc_l[ADC2_RCV_SIZE]; // ADC2

/* ADC1 STATE */
typedef struct
{
	td_adc1_state_t state;
} td_adc1_state_data_t;

td_adc1_state_data_t td_adc1_fsm_state;
td_adc1_state_t ex_adc1_cur_state = td_adc1_idle;

/* ADC2 STATE */
typedef struct
{
	td_adc2_state_t state;
} td_adc2_state_data_t;

td_adc2_state_data_t td_adc2_fsm_state;
td_adc2_state_t ex_adc2_cur_state = td_adc2_idle;

/* ADC CALLBACK FUNCTION */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	if (hadc->Instance == hadc1.Instance)
	{
		for (int index = 0; index < ADC1_RCV_SIZE; index++)
		{
			/* ADC FILTER */
			if (fabs(
			TD_ADC1_CONV_BUF[index] - TD_ADC1_CONV_BUF[(index + 1) % 10]) < 200)
			{
				ex_setpup_adc[index] = TD_ADC1_CONV_BUF[index]; // STEPUP_FEEDBACK
			}
			else
			{
				ex_setpup_adc[index] = TD_ADC1_CONV_BUF[index - 1]; // STEPUP_FEEDBACK
			}
		}
		ex_adc1_cur_state = td_adc1_conv_ok; // td_Start_ADC1_Conv();
	}

	if (hadc->Instance == hadc2.Instance)
	{
		for (int index = 0; index < ADC2_RCV_SIZE; index++)
		{
			ex_peak_adc_r[index] = TD_ADC2_CONV_BUF[index]; // PEAK_DETECTION
		}
		ex_adc2_cur_state = td_adc2_conv_ok;
	}
}

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
{
	ex_adc1_cur_state = td_adc1_error;
	ex_adc2_cur_state = td_adc2_error;
	TD_SHELL_PRINT(("ADC ERROR CALLBACK!!\n"));
}
/**********************/

/*
 * ADC ENABLE
 * */
void td_ADC1_Enable(void)
{
	if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK)
	{
		Error_Handler();
	}
}

void td_ADC2_Enable(void)
{
	if (HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED) != HAL_OK)
	{
		Error_Handler();
	}
}
/**********************/

/*
 * ADC INIT
 * */
void td_ADC_State_Init(void)
{
	TD_ADC1_CUR_STATE = td_adc1_state_max;
	TD_ADC2_CUR_STATE = td_adc2_state_max;
	td_Set_ADC1_State(td_adc1_state_init);
	td_Set_ADC2_State(td_adc2_state_init);
	td_ADC1_Enable();
	td_ADC2_Enable();
}
/**********************/

/*
 * ADC START CONVERSION
 * */
void td_Start_ADC1_Conv()
{
	if (HAL_ADC_Start_DMA(&hadc1, (uint32_t*) TD_ADC1_CONV_BUF,
	ADC1_CHK_CH_NUM * ADC1_RCV_SIZE) != HAL_OK)
	{
		Error_Handler();
	}
}

void td_Start_ADC2_Conv()
{
	if (HAL_ADC_Start_DMA(&hadc2, (uint32_t*) TD_ADC2_CONV_BUF,
	ADC2_CHK_CH_NUM * ADC2_RCV_SIZE) != HAL_OK)
	{
		Error_Handler();
	}
}
/**********************/

/*
 * ADC STOP CONVERSION
 * */
void td_Stop_ADC1_Conv()
{
	if (HAL_ADC_Stop_DMA(&hadc1) != HAL_OK)
	{
		Error_Handler();
	}
}

void td_Stop_ADC2_Conv()
{
	if (HAL_ADC_Stop_DMA(&hadc2) != HAL_OK)
	{
		Error_Handler();
	}
}
/**********************/

/*
 * STEPUP ADC AVG CALC
 * */
uint32_t td_Stepup_ADC1_AVG()
{
	uint32_t adc1_avg = 0;

	for (int i = 0; i < ADC1_RCV_SIZE; i++)
	{
		adc1_avg += ex_setpup_adc[i];
	}
	return adc1_avg; //return adc1_avg / TD_ADC1_RCV_SIZE;
}

uint32_t td_Peak_Detection_ADC2_AVG()
{
	uint32_t adc2_avg = 0;

	for (int i = 0; i < ADC2_RCV_SIZE; i++)
	{
		adc2_avg += ex_peak_adc_r[i];
	}
	return adc2_avg / ADC2_RCV_SIZE;
}
/**********************/

/*
 * VOLTAGE CALC FUNCTION
 * */
uint32_t td_ADC_Calc_Stepup_V(uint32_t in_adc_val, uint32_t r1, uint32_t r2)
{
	/*
	 40v == 1065 ADC

	 1.14v : 1065 == ADC_vol : ADC_val
	 ADC_vol = (1.14v * ADC_val) / 1065
	 --------------
	 40v : 1.2v == x : ADC_vol
	 x = (40 * ADC_val) / 1.14v
	 */

	/* REFERENCE TABLE */
	if (TD_VOLTAGE_RELATED_PULSE_WIDTH > 24)
	{
		TD_VOLTAGE_RELATED_PULSE_WIDTH = 24;
	}
	uint64_t adc_val = (TD_REF_ADC_VOLTAGE_TABLE[TD_VOLTAGE_RELATED_PULSE_WIDTH]
			* in_adc_val)
			/ TD_REF_ADC_VALUE_TABLE[TD_VOLTAGE_RELATED_PULSE_WIDTH];
	uint64_t v_out = (TD_REF_VOLTAGE_TABLE[TD_VOLTAGE_RELATED_PULSE_WIDTH]
			* adc_val)
			/ TD_REF_ADC_VOLTAGE_TABLE[TD_VOLTAGE_RELATED_PULSE_WIDTH];

	return v_out;

#if 0
	float f_adc_val;
	uint32_t vdda = 3300UL;
	float v_ref = (float) (vdda * 0.001);
	float v_out;

	f_adc_val = in_adc_val / 3005.101815202915f;
	v_out = (v_ref * f_adc_val) * ((r1 + r2) / r2);

	return v_out;
#endif
}

uint32_t td_ADC_Calc_Peak_V(uint32_t in_adc_val)
{
	uint32_t vdda = 1800UL;
	uint32_t v_out;

	v_out = (vdda * in_adc_val) / 4095;

	return v_out;
}
/**********************/

/*
 * ADC STEPUP HANDLER
 * */
void td_ADC_Handle(void)
{
	/* FSM ADC */
	if (TD_ADC1_CUR_STATE != ex_adc1_cur_state)
	{
		td_Set_ADC1_State(ex_adc1_cur_state);
	}

	if (TD_ADC2_CUR_STATE != ex_adc2_cur_state)
	{
		td_Set_ADC2_State(ex_adc2_cur_state);
	}

}
/**********************/

td_adc1_state_t td_Get_ADC1_State(void)
{
	return TD_ADC1_CUR_STATE;
}

td_adc2_state_t td_Get_ADC2_State(void)
{
	return TD_ADC2_CUR_STATE;
}

void td_Set_ADC1_State(td_adc1_state_t state)
{
	switch (state)
	{
	case td_adc1_state_init:
		break;
	case td_adc1_idle:
		td_Stop_ADC1_Conv();
		break;
	case td_adc1_run:
		td_Start_ADC1_Conv();
		break;
	case td_adc1_conv_ok:
		break;
	case td_adc1_print_ok:
		ex_adc1_cur_state = td_adc1_run;
		break;
	case td_adc1_error:
		td_Stop_ADC1_Conv();
		break;
	default:
		break;
	}
	TD_ADC1_CUR_STATE = state;
}

void td_Set_ADC2_State(td_adc2_state_t state)
{
	switch (state)
	{
	case td_adc2_state_init:
		break;
	case td_adc2_idle:
		td_Stop_ADC2_Conv();
		break;
	case td_adc2_run:
		td_Start_ADC2_Conv();
		break;
	case td_adc2_conv_ok:
		break;
	case td_adc2_print_ok:
		ex_adc2_cur_state = td_adc2_run;
		break;
	case td_adc2_error:
		td_Stop_ADC2_Conv();
		break;
	default:
		break;
	}
	TD_ADC2_CUR_STATE = state;
}
