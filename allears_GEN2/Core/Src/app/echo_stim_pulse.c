/*
 * echo_pwm_stim.c
 *
 *  Created on: Oct 26, 2022
 *      Author: ECHO
 */
#include <math.h>
#include "main.h"
#include "echo_adc.h"
#include "echo_flash_memory.h"
#include "echo_stim_setting.h"
#include "echo_sys_fsm_state.h"

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern DMA_HandleTypeDef hdma_adc1;
extern DMA_HandleTypeDef hdma_adc2;

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim16;
extern DMA_HandleTypeDef hdma_tim2_ch2_ch4;

/* PWM STRUCT */
extern pwm_pulse_param_t pwm_param;

/* ADC BUFFER FLAG*/
extern bool adc1_buff_full_flag;
extern bool adc2_buff_full_flag;

/* STEP CONTROL VALUE */
extern int v_step_tv;

/* STEPUP AND PEAK-DETECTION ADC VALUE*/
extern uint16_t peak_adc[ECHO_ADC1_RCV_SIZE];
extern uint16_t setpup_adc[ECHO_ADC1_RCV_SIZE];

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
#if NOT_USEAGE_TRGO
	if (htim->Instance == TIM2)
	{
		/* ADC2 TRGO SIGNAL */
		float adc_avg_data = Echo_Peak_Detection_ADC2_AVG();
		ECHO_SHELL_PRINT(("ADC2 DATA : %f\n",adc_avg_data));
		ECHO_SHELL_PRINT(
				("PEAK VOLTAGE : %f\n",Echo_ADC_Calc_Peak_V(adc_avg_data)));
	}
#endif

	if (htim->Instance == TIM16)
	{
		/* ADC1 FSM */
		if (Echo_Get_ADC1_State() == ECHO_ADC1_CONV_OK)
		{
			float adc_avg_data = Echo_Stepup_ADC1_AVG();
			ECHO_SHELL_PRINT(("ADC1 DATA : %f\n",adc_avg_data));
			ECHO_SHELL_PRINT(
					("STEPUP VOLTAGE : %f\n",Echo_ADC_Calc_Stepup_V(adc_avg_data, R1_Vstup, R2_Vstup)));
			ECHO_SHELL_PRINT(("----------\r\n"));
			Echo_Set_ADC1_State(ECHO_ADC1_PRINT_OK);
		}
#if 0
		/* ADC1 NONE FSM */
		if (ADC1_CONV_OK == true)
		{
			float adc_avg_data = Echo_Stepup_ADC1_AVG();
			ECHO_SHELL_PRINT(("ADC1 DATA : %f\n",adc_avg_data));
			ECHO_SHELL_PRINT(
					("STEPUP VOLTAGE : %f\n",Echo_ADC_Calc_Stepup_V(adc_avg_data, R1_Vstup, R2_Vstup)));
			ECHO_SHELL_PRINT(("----------\r\n"));
			ADC1_CONV_OK = false;
		}
#endif
#if 0
		/* ADC2 NONE TRGO SIGNAL */
		if (ADC2_CONV_OK == true)
		{
			float adc_avg_data = Echo_PEAK_DETECTION_ADC2_AVG();
			ECHO_SHELL_PRINT(("ADC2 DATA : %f\n",adc_avg_data));
			ECHO_SHELL_PRINT(
					("PEAK VOLTAGE : %f\n",Echo_Calc_Peak_V(adc_avg_datas)));
			ADC2_CONV_OK = false;
		}

#endif
	}
}
