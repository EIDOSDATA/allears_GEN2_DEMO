/*
 * echo_pwm_stim.c
 *
 *  Created on: Oct 26, 2022
 *      Author: ECHO
 */
#include "echo_stim_setting.h"
#include "echo_state.h"
#include "main.h"
#include "echo_flash_memory.h"

#define PULSE_DEBOUNCING_TIME			5

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim16;
extern DMA_HandleTypeDef hdma_tim2_ch2_ch4;

extern pwm_pulse_param_t pwm_param;
extern uint32_t ano_matching_tim1;
extern uint32_t cat_matching_tim1;
extern uint32_t cat_matching_tim2;

extern int v_step_tv;
extern int v_step_val;

#if 1
bool gPulse_high = false;
int dac_procedure_cnt = 0;

/* TIM2 OC Interrupt handler */
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM2)
	{
		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
		{
			if (gPulse_high == false)
			{
				TIM2->CCR2 = cat_matching_tim2;
				gPulse_high = true;
			}
			else
			{
				TIM2->CCR2 = cat_matching_tim1;
				gPulse_high = false;
			}
		}

		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
		{
			if (dac_procedure_cnt == 0)
			{
				TIM2->CCR4 = ano_matching_tim1 - PULSE_DEBOUNCING_TIME;
				dac_procedure_cnt = 1;
			}
			else if (dac_procedure_cnt == 1)
			{
				TIM2->CCR4 = cat_matching_tim1 + PULSE_DEBOUNCING_TIME;
				dac_procedure_cnt = 2;
			}
			else if (dac_procedure_cnt == 2)
			{
				TIM2->CCR4 = cat_matching_tim2 - PULSE_DEBOUNCING_TIME;
				dac_procedure_cnt = 3;
			}
			else if (dac_procedure_cnt == 3)
			{
				TIM2->CCR4 = PULSE_DEBOUNCING_TIME;
				dac_procedure_cnt = 0;
			}
		}
	}
}
#endif

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM16)
	{
		if (Echo_Get_FSM_State() == ECHO_STATE_RUN)
		{
			if (v_step_val < v_step_tv)
			{
				v_step_val += 10;
				Echo_Pulse_V_PW_Config();
			}
			else if (v_step_val > v_step_tv)
			{
				v_step_val = v_step_tv;
				Echo_Pulse_V_PW_Config();
			}
			else if (v_step_val == v_step_tv)
			{
				HAL_TIM_Base_Stop_IT(&htim16);
			}
		}
	}
}
