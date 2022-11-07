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

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim16;
extern DMA_HandleTypeDef hdma_tim2_ch2_ch4;

extern pwm_pulse_param_t pwm_param;
extern uint32_t ano_matching_tim1;
extern uint32_t cat_matching_tim1;
extern uint32_t cat_matching_tim2;

extern int v_step_tv;
extern int v_step_val;
bool gPulse_high = false;
#if 1
/* TIM2 OC Interrupt handler */
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM2)
	{
		//ano_matching_tim1 = pwm_param.pulse_width;
		//cat_matching_tim1 = ano_matching_tim1 + pwm_param.dead_time;
		//cat_matching_tim2 = (ano_matching_tim1 * 2) + pwm_param.dead_time;
		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
		{
			if (gPulse_high == false)
			{
				TIM2->CCR2 = cat_matching_tim2;
				TIM2->CCR4 = cat_matching_tim2;
				gPulse_high = true;
			}
			else
			{
				TIM2->CCR2 = cat_matching_tim1;
				TIM2->CCR4 = cat_matching_tim1;
				gPulse_high = false;
			}
			//HAL_GPIO_WritePin(GPIOA, AUL_GPIO_TP1_Pin, GPIO_PIN_SET);
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
