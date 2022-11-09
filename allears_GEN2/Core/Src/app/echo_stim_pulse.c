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

extern int v_step_tv;
extern int v_step_val;

#ifdef ECHO_PULSE_INTERRUPTx

bool gPulse_high = false;
int current_ctrl_proc = 0;

/* TIM2 OC Interrupt handler */
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM2)
	{
		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
		{
			if (gPulse_high == false)
			{
				TIM2->CCR4 = CATHODE_PULSE_TIME0;
				gPulse_high = true;
			}
			else
			{
				TIM2->CCR4 = CATHODE_PULSE_TIME1;
				gPulse_high = false;
			}
		}

		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
		{
			if (current_ctrl_proc == 0)
			{
				TIM2->CCR1 = CURRENT_CTRL_TIME0;
				current_ctrl_proc = 1;
			}
			else if (current_ctrl_proc == 1)
			{
				TIM2->CCR1 = CURRENT_CTRL_TIME1;
				current_ctrl_proc = 2;
			}
			else if (current_ctrl_proc == 2)
			{
				TIM2->CCR1 = CURRENT_CTRL_TIME2;
				current_ctrl_proc = 3;
			}
			else if (current_ctrl_proc == 3)
			{
				TIM2->CCR1 = CURRENT_CTRL_TIME3;
				current_ctrl_proc = 0;
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
