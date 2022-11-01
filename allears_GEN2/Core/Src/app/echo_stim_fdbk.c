/*
 * echo_stepup_setting.c
 *
 *  Created on: Oct 28, 2022
 *      Author: ECHO
 */

#include "main.h"
#include "echo_state.h"
#include "echo_stim_setting.h"
#include "echo_stim_fdbk.h"

extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;

#define ADC1_CONV_BUF									get_adc1_buf
uint16_t get_adc1_buf[ADC_CHK_CH_NUM];

static void ADC1_Conv_Waiting(void)
{
	uint32_t wait_tick;

	wait_tick = HAL_GetTick();

	while (LL_ADC_IsActiveFlag_EOS(ADC1) == 0
			&& (HAL_GetTick() - wait_tick < ADC_CONV_WAIT_TIME_MAX))
		;;
}

void Echo_ADC1_Enable()
{
	// Enable ADC DMA
	HAL_ADC_IRQHandler(&hadc1);
	HAL_ADC_Start_IT(&hadc1);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*) ADC1_CONV_BUF, ADC_CHK_CH_NUM);
	HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
	ADC1_Conv_Waiting();
}

void Echo_Start_ADC_Conv()
{
	HAL_ADC_IRQHandler(&hadc1);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*) ADC1_CONV_BUF, ADC_CHK_CH_NUM);

	LL_ADC_REG_StartConversion(ADC1);

	ADC1_Conv_Waiting();
}

void Echo_Get_ADC_ConvVal(uint16_t *con_val)
{
	uint8_t i;
	float temp;

	for (i = 0; i < ADC_CHK_CH_NUM; i++)
	{
		//AUL_DEBUG_PRINT(("ADCVal[%d] = %u\n", i, ADCVal[i]));

		temp = (float) ADC1_CONV_BUF[i] * ADC_VDDA;
		temp /= (float) ADC_MAX_VAL;

		/* Voltage value is inaccurate which is changed from ADC. So, not change */
#if 0
		/* 2nd is BAT. Ref circuit */
		if(i == 1)
		{
			temp *= (ADC_BAT_R1 + ADC_BAT_R2);
			temp /= ADC_BAT_R2;
		}
#endif
		con_val[i] = (uint16_t) temp;
	}
}

void Echo_Stepup_Handle(void)
{
	if (Echo_Get_FSM_State() == ECHO_STATE_RUN)
	{
		static uint32_t st_handle_tick = 0;
		uint16_t convVal[ADC_CHK_CH_NUM];

		if (HAL_GetTick() - st_handle_tick >= PWR_HANDLE_PERIOD)
		{
			st_handle_tick = HAL_GetTick();

			Echo_Start_ADC_Conv();

			Echo_Get_ADC_ConvVal(convVal);
		}
	}

}
