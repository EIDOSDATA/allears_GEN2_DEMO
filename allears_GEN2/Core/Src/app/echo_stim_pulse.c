/*
 * echo_pwm_stim.c
 *
 *  Created on: Oct 26, 2022
 *      Author: ECHO
 */
#include <echo_stim_setting.h>
#include "main.h"
#include "stm32l4xx_ll_tim.h"
#include "echo_flash_memory.h"

extern TIM_HandleTypeDef htim2;
extern DMA_HandleTypeDef hdma_tim2_ch2_ch4;

extern pwm_pulse_param_t pwm_param;
extern uint32_t ano_matching_tim1;
extern uint32_t cat_matching_tim1;
extern uint32_t cat_matching_tim2;

#if 0
bool gPulse_high = false;
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
				LL_TIM_OC_SetCompareCH2(htim2.Instance, cat_matching_tim2);
				gPulse_high = true;
			}
			else
			{
				LL_TIM_OC_SetCompareCH2(htim2.Instance, cat_matching_tim1);
				gPulse_high = false;
			}
			//HAL_GPIO_WritePin(GPIOA, AUL_GPIO_TP1_Pin, GPIO_PIN_SET);
		}
	}
}
#endif
