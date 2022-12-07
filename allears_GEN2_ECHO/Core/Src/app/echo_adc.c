/*
 * echo_stepup_setting.c
 *
 *  Created on: Oct 28, 2022
 *      Author: ECHO
 */
#include <stdint.h>
#include <math.h>
#include <echo_adc.h>
#include <echo_shell.h>
#include <echo_stim_setting.h>
#include <echo_sys_fsm_state.h>
#include "main.h"

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern DMA_HandleTypeDef hdma_adc1;
extern DMA_HandleTypeDef hdma_adc2;

/* STEPUP VOLTAGE TABLE */
extern __IO int ex_voltage_r_pw;

/* REFERENCE TABLE */
#define VREFINT_CAL									(*(volatile uint16_t*) 0x1FFF75AA)
#define ECHO_REF_VOLTAGE_TABLE						ref_voltage_table
#define ECHO_REF_ADC_VALUE_TABLE					ref_adc_value_table
#define ECHO_REF_ADC_VOLTAGE_TABLE					ref_adc_voltage_table

/* Voltage Scale : 10 uV >> ADC BUFFER IS 10 */
int ref_voltage_table[30] =
{ 360000, 660000, 880000, 1080000, 1320000, 1520000, 1760000, 1960000, 2160000,
		2380000, 2600000, 2800000, 3000000, 3200000, 3380000, 3560000, 3720000,
		3920000, 4080000, 4240000, 4400000, 4540000, 4680000, 4820000, 4940000,
		5080000, 5180000, 5260000, 5340000, 5420000 }; // 30

int ref_adc_value_table[30] =
{ 282, 340, 384, 428, 477, 540, 585, 630, 676, 720, 763, 803, 847, 888, 928,
		968, 1005, 1043, 1080, 1118, 1150, 1188, 1216, 1247, 1275, 1300, 1322,
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
uint16_t ex_peak_adc[ADC2_RCV_SIZE]; // ADC2

/* ADC1 STATE */
typedef struct
{
	echo_adc1_state_t state;
} echo_adc1_state_data_t;

echo_adc1_state_data_t echo_adc1_fsm_state;
echo_adc1_state_t ex_adc1_cur_state = echo_adc1_idle;

/* ADC2 STATE */
typedef struct
{
	echo_adc2_state_t state;
} echo_adc2_state_data_t;

echo_adc2_state_data_t echo_adc2_fsm_state;
echo_adc2_state_t ex_adc2_cur_state = echo_adc2_idle;

/* ADC CALLBACK FUNCTION */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	if (hadc->Instance == hadc1.Instance)
	{
		for (int index = 0; index < ADC1_RCV_SIZE; index++)
		{
			/* ADC FILTER */
			if (fabs(
			ECHO_ADC1_CONV_BUF[index] - ECHO_ADC1_CONV_BUF[(index + 1) % 10])
					< 200)
			{
				ex_setpup_adc[index] = ECHO_ADC1_CONV_BUF[index]; // STEPUP_FEEDBACK
			}
			else
			{
				ex_setpup_adc[index] = ECHO_ADC1_CONV_BUF[index - 1]; // STEPUP_FEEDBACK
			}
		}

#if 0
		ADC1_CONV_OK = true;
#endif
		//Echo_Start_ADC1_Conv();
		ex_adc1_cur_state = echo_adc1_conv_ok; //Echo_Set_ADC1_State(ECHO_ADC1_CONV_OK);
	}

	if (hadc->Instance == hadc2.Instance)
	{
		for (int index = 0; index < ADC2_RCV_SIZE; index++)
		{
			ex_peak_adc[index] = ECHO_ADC2_CONV_BUF[index]; // PEAK_DETECTION
		}
#if 0
	ADC2_CONV_OK = true;
#endif
		ex_adc2_cur_state = echo_adc2_conv_ok; //Echo_Set_ADC2_State(ECHO_ADC2_CONV_OK);
	}
}

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
{
	ex_adc1_cur_state = echo_adc1_error;
	ex_adc2_cur_state = echo_adc2_error;
	ECHO_SHELL_PRINT(("ADC ERROR CALLBACK!!\n"));
}
/**********************/

/*
 * ADC ENABLE
 * */
void Echo_ADC1_Enable(void)
{
	if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK)
	{
		Error_Handler();
	}
}

void Echo_ADC2_Enable(void)
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
void Echo_ADC_State_Init(void)
{
	ECHO_ADC1_CUR_STATE = echo_adc1_state_max;
	ECHO_ADC2_CUR_STATE = echo_adc2_state_max;
	Echo_Set_ADC1_State(echo_adc1_state_init);
	Echo_Set_ADC2_State(echo_adc2_state_init);
	Echo_ADC1_Enable();
	Echo_ADC2_Enable();
}
/**********************/

/*
 * ADC START CONVERSION
 * */
void Echo_Start_ADC1_Conv()
{
	if (HAL_ADC_Start_DMA(&hadc1, (uint32_t*) ECHO_ADC1_CONV_BUF,
	ADC1_CHK_CH_NUM * ADC1_RCV_SIZE) != HAL_OK)
	{
		Error_Handler();
	}
}

void Echo_Start_ADC2_Conv()
{
	if (HAL_ADC_Start_DMA(&hadc2, (uint32_t*) ECHO_ADC2_CONV_BUF,
	ADC2_CHK_CH_NUM * ADC2_RCV_SIZE) != HAL_OK)
	{
		Error_Handler();
	}
}
/**********************/

/*
 * ADC STOP CONVERSION
 * */
void Echo_Stop_ADC1_Conv()
{
	if (HAL_ADC_Stop_DMA(&hadc1) != HAL_OK)
	{
		Error_Handler();
	}
}

void Echo_Stop_ADC2_Conv()
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
uint32_t Echo_Stepup_ADC1_AVG()
{
	uint32_t adc1_avg = 0;

	for (int i = 0; i < ADC1_RCV_SIZE; i++)
	{
		adc1_avg += ex_setpup_adc[i];
	}
	return adc1_avg; //return adc1_avg / ECHO_ADC1_RCV_SIZE;
}

uint32_t Echo_Peak_Detection_ADC2_AVG()
{
	uint32_t adc2_avg = 0;

	for (int i = 0; i < ADC2_RCV_SIZE; i++)
	{
		adc2_avg += ex_peak_adc[i];
	}
	return adc2_avg / ADC2_RCV_SIZE;
}
/**********************/

/*
 * VOLTAGE CALC FUNCTION
 * */
uint32_t Echo_ADC_Calc_Stepup_V(uint32_t in_adc_val, float r1, float r2)
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
	if (VOLTAGE_RELATED_PULSE_WIDTH > 24)
	{
		VOLTAGE_RELATED_PULSE_WIDTH = 24;
	}
	uint64_t adc_val = (ECHO_REF_ADC_VOLTAGE_TABLE[VOLTAGE_RELATED_PULSE_WIDTH]
			* in_adc_val)
			/ ECHO_REF_ADC_VALUE_TABLE[VOLTAGE_RELATED_PULSE_WIDTH];
	uint64_t v_out = (ECHO_REF_VOLTAGE_TABLE[VOLTAGE_RELATED_PULSE_WIDTH]
			* adc_val)
			/ ECHO_REF_ADC_VOLTAGE_TABLE[VOLTAGE_RELATED_PULSE_WIDTH];

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

uint32_t Echo_ADC_Calc_Peak_V(uint32_t in_adc_val)
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
void Echo_ADC_Handle(void)
{
#if 0
/* NONE FSM ADC */
if (Echo_Get_Sys_FSM_State()
		== ECHO_SYS_STATE_RUN&& ADC1_BUFFER_FULL != true)
//if (Echo_Get_FSM_State() == ECHO_STATE_RUN && ADC2_BUFFER_FULL != true)
{
	static uint32_t st_handle_tick = 0;

	if (HAL_GetTick() - st_handle_tick >= PWR_HANDLE_PERIOD)
	{
		Echo_Start_ADC1_Conv();
		//Echo_Start_ADC2_Conv();
		st_handle_tick = HAL_GetTick();
	}
}
#endif

	/* FSM ADC */
	if (ECHO_ADC1_CUR_STATE != ex_adc1_cur_state)
	{
		Echo_Set_ADC1_State(ex_adc1_cur_state);
	}
}
/**********************/

echo_adc1_state_t Echo_Get_ADC1_State(void)
{
	return ECHO_ADC1_CUR_STATE;
}

echo_adc2_state_t Echo_Get_ADC2_State(void)
{
	return ECHO_ADC2_CUR_STATE;
}

void Echo_Set_ADC1_State(echo_adc1_state_t state)
{
	switch (state)
	{
	case echo_adc1_state_init:
		break;
	case echo_adc1_idle:
		Echo_Stop_ADC1_Conv();
		break;
	case echo_adc1_run:
		Echo_Start_ADC1_Conv();
		break;
	case echo_adc1_conv_ok:
		break;
	case echo_adc1_print_ok:
		ex_adc1_cur_state = echo_adc1_run;
		break;
	case echo_adc1_error:
		Echo_Stop_ADC1_Conv();
		break;
	default:
		break;
	}
	ECHO_ADC1_CUR_STATE = state;
}

void Echo_Set_ADC2_State(echo_adc2_state_t state)
{
	switch (state)
	{
	case echo_adc2_state_init:
		break;
	case echo_adc2_idle:
		Echo_Stop_ADC2_Conv();
		break;
	case echo_adc2_run:
		Echo_Start_ADC2_Conv();
		break;
	case echo_adc2_conv_ok:
		break;
	case echo_adc2_print_ok:
		Echo_Set_ADC2_State(echo_adc2_run);
		break;
	case echo_adc2_error:
		Echo_Stop_ADC2_Conv();
		break;
	default:
		break;
	}
	ECHO_ADC2_CUR_STATE = state;
}
