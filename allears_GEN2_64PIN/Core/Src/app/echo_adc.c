/*
 * echo_stepup_setting.c
 *
 *  Created on: Oct 28, 2022
 *      Author: ECHO
 */
#include <math.h>
#include "main.h"
#include "echo_adc.h"
#include "echo_stim_setting.h"
#include "echo_sys_fsm_state.h"

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern DMA_HandleTypeDef hdma_adc1;
extern DMA_HandleTypeDef hdma_adc2;

uint16_t get_adc1_buf[ADC1_CHK_CH_NUM * ECHO_ADC1_RCV_SIZE];
uint16_t get_adc2_buf[ADC2_CHK_CH_NUM * ECHO_ADC2_RCV_SIZE];

uint16_t setpup_adc[ECHO_ADC1_RCV_SIZE]; // ADC1
uint16_t peak_adc[ECHO_ADC2_RCV_SIZE]; // ADC2

/* TEST FLAG */
__IO bool adc1_conv_ok_flag = false;
__IO bool adc2_conv_ok_flag = false;

extern __IO bool vpw_set_flag;

/* ADC FSM STATE */
#define ECHO_ADC1_CUR_STATE		echo_adc1_fsm_state.state
#define ECHO_ADC2_CUR_STATE		echo_adc2_fsm_state.state

/* ADC1 STATE */
typedef struct
{
	echo_adc1_state_t state;
} echo_adc1_state_data_t;

echo_adc1_state_data_t echo_adc1_fsm_state;
echo_adc1_state_t adc1_cur_state = ECHO_ADC1_IDLE;

/* ADC2 STATE */
typedef struct
{
	echo_adc2_state_t state;
} echo_adc2_state_data_t;

echo_adc2_state_data_t echo_adc2_fsm_state;
echo_adc2_state_t adc2_cur_state = ECHO_ADC2_IDLE;

/* ADC CALLBACK FUNCTION */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	if (hadc->Instance == hadc1.Instance)
	{
		for (int index = 0; index < ECHO_ADC1_RCV_SIZE; index++)
		{
			setpup_adc[index] = ADC1_CONV_BUF[index]; // STEPUP_FEEDBACK
		}

		if (vpw_set_flag == true)
		{
			if (fabs(setpup_adc[0] - setpup_adc[1]) < 20
					&& fabs(setpup_adc[1] - setpup_adc[2]) < 20
					&& fabs(setpup_adc[2] - setpup_adc[0]) < 20)
			{
				Echo_VPW_TP_OFF();
				vpw_set_flag = false;
			}
		}
#if 0
		ADC1_CONV_OK = true;
#endif
		//Echo_Start_ADC1_Conv();
		adc1_cur_state = ECHO_ADC1_CONV_OK; //Echo_Set_ADC1_State(ECHO_ADC1_CONV_OK);
	}

	if (hadc->Instance == hadc2.Instance)
	{
		for (int index = 0; index < ECHO_ADC2_RCV_SIZE; index++)
		{
			peak_adc[index] = ADC2_CONV_BUF[index]; // PEAK_DETECTION
		}
#if 0
		ADC2_CONV_OK = true;
#endif
		adc2_cur_state = ECHO_ADC2_CONV_OK; //Echo_Set_ADC2_State(ECHO_ADC2_CONV_OK);
	}
}

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
{
	adc1_cur_state = ECHO_ADC1_ERROR;
	adc2_cur_state = ECHO_ADC2_ERROR;
//HAL_UART_Transmit(&huart2, (uint8_t*) "ADC ERROR CALLBACK!!\r\n", 22, 1000);
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
	Echo_Set_ADC1_State(ECHO_ADC1_STATE_INIT);
	Echo_Set_ADC2_State(ECHO_ADC2_STATE_INIT);
	Echo_ADC1_Enable();
	Echo_ADC2_Enable();
}
/**********************/

/*
 * ADC START CONVERSION
 * */
void Echo_Start_ADC1_Conv()
{
	if (HAL_ADC_Start_DMA(&hadc1, (uint32_t*) ADC1_CONV_BUF,
	ADC1_CHK_CH_NUM * ECHO_ADC1_RCV_SIZE) != HAL_OK)
	{
		Error_Handler();
	}
}

void Echo_Start_ADC2_Conv()
{
	if (HAL_ADC_Start_DMA(&hadc2, (uint32_t*) ADC2_CONV_BUF,
	ADC2_CHK_CH_NUM * ECHO_ADC2_RCV_SIZE) != HAL_OK)
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
float Echo_Stepup_ADC1_AVG()
{
	float adc1_avg = 0;

	for (int i = 0; i < ECHO_ADC1_RCV_SIZE; i++)
	{
		adc1_avg += setpup_adc[i];
	}
	return adc1_avg / ECHO_ADC1_RCV_SIZE;
}

float Echo_Peak_Detection_ADC2_AVG()
{
	float adc2_avg = 0;

	for (int i = 0; i < ECHO_ADC2_RCV_SIZE; i++)
	{
		adc2_avg += peak_adc[i];
	}
	return adc2_avg / ECHO_ADC2_RCV_SIZE;
}
/**********************/

/*
 * VOLTAGE CALC FUNCTION
 * */
float Echo_ADC_Calc_Stepup_V(uint16_t in_adc_val, float r1, float r2)
{
	float f_adc_val;
	uint32_t vdda = 1800UL;
	float v_ref = (float) (vdda * 0.001);
	float v_out;

	f_adc_val = in_adc_val / 4095.f;
	v_out = (v_ref * f_adc_val) * ((r1 + r2) / r2);

	return v_out;
}

float Echo_ADC_Calc_Peak_V(uint16_t in_adc_val)
{
	float f_adc_val;
	uint32_t vdda = 3300UL;
	float v_ref = (float) (vdda * 0.001);
	float v_out;

	f_adc_val = in_adc_val / 4095.f;
	v_out = v_ref * f_adc_val;

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
	if (ECHO_ADC1_CUR_STATE != adc1_cur_state)
	{
		Echo_Set_ADC1_State(adc1_cur_state);
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
	case ECHO_ADC1_STATE_INIT:
		break;
	case ECHO_ADC1_IDLE:
		Echo_Stop_ADC1_Conv();
		break;
	case ECHO_ADC1_RUN:
		Echo_Start_ADC1_Conv();
		break;
	case ECHO_ADC1_CONV_OK:
		break;
	case ECHO_ADC1_PRINT_OK:
		adc1_cur_state = ECHO_ADC1_RUN;
		break;
	case ECHO_ADC1_ERROR:
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
	case ECHO_ADC2_STATE_INIT:
		break;
	case ECHO_ADC2_IDLE:
		Echo_Stop_ADC2_Conv();
		break;
	case ECHO_ADC2_RUN:
		Echo_Start_ADC2_Conv();
		break;
	case ECHO_ADC2_CONV_OK:
		break;
	case ECHO_ADC2_PRINT_OK:
		Echo_Set_ADC2_State(ECHO_ADC2_RUN);
		break;
	case ECHO_ADC2_ERROR:
		Echo_Stop_ADC2_Conv();
		break;
	default:
		break;
	}
	ECHO_ADC2_CUR_STATE = state;
}
