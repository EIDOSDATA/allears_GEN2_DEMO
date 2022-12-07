/*
 * td_pwm_stim.c
 *
 *  Created on: Oct 26, 2022
 *      Author: ECHO
 */
#include <stdint.h>
#include <math.h>
#include <td_adc.h>
#include <td_flash_memory.h>
#include <td_shell.h>
#include <td_stim_setting.h>
#include <td_sys_fsm_state.h>
#include "main.h"

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern DMA_HandleTypeDef hdma_adc1;
extern DMA_HandleTypeDef hdma_adc2;

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim16;
extern DMA_HandleTypeDef hdma_tim2_ch2_ch4;

/* PWM STRUCT */
extern pwm_pulse_param_t ex_pwm_param;

/* STEP CONTROL VALUE */
extern __IO int ex_voltage_r_pw;
extern __IO int ex_voltage_val_output;
extern __IO bool ex_slope_ctrl_end_f;

/* STEPUP AND PEAK-DETECTION ADC VALUE*/
extern uint16_t ex_peak_adc[ADC1_RCV_SIZE];
extern uint16_t ex_setpup_adc[ADC1_RCV_SIZE];

/* TIMER COUNTER VALUE */
int timer2_cnt = 1;
int timer16_cnt = 1;

/* ADC FEED BACK VALUE */
uint32_t fdbk_adc_avg_data;
uint64_t fdbk_adc_voltage;
bool stepup_print_f = false;

/* ADC PEAK DETECTION VALUE */
uint32_t peak_adc_avg_data;
uint32_t peak_adc_voltage;
bool peak_print_f = false;

void td_ADC_Voltage_Feedback()
{
	fdbk_adc_avg_data = td_Stepup_ADC1_AVG();
	fdbk_adc_voltage = td_ADC_Calc_Stepup_V(fdbk_adc_avg_data, R1_Vstup,
	R2_Vstup);
	td_Voltage_Config(fdbk_adc_voltage);
	timer16_cnt = 0;
	stepup_print_f = true;
}

void td_Stepup_ADC_Data_Print()
{
	if (stepup_print_f == true)
	{
		uint32_t n_number = fdbk_adc_voltage / STEPUP_VOLTAGE_SCALE;
		uint32_t dec_point = fdbk_adc_voltage % STEPUP_VOLTAGE_SCALE;

		TD_SHELL_PRINT(("ADC1 SUM DATA: %ld\n", fdbk_adc_avg_data));
		TD_SHELL_PRINT(("VOLTAGE : %ld.%ld\n", n_number, dec_point));

		TD_SHELL_PRINT(("SETTING VOLTAGE : %d\n", TD_VOLTAGE_VALUE_OUTPUT));
		TD_SHELL_PRINT(("STEPUP PW : %d\n", TD_VOLTAGE_RELATED_PULSE_WIDTH));

		TD_SHELL_PRINT(("----------\r\n"));
	}
	stepup_print_f = false;
}

void td_ADC_PeakDetection()
{
	peak_adc_avg_data = td_Peak_Detection_ADC2_AVG();
	peak_adc_voltage = td_ADC_Calc_Peak_V(peak_adc_avg_data);
	peak_print_f = true;
}

void td_PeakDetection_ADC_Data_Print()
{
	if (peak_print_f == true)
	{
		uint32_t n_number = peak_adc_voltage / PEAKDETECTION_VOLTAGE_SCALE;
		uint32_t dec_point = peak_adc_voltage % PEAKDETECTION_VOLTAGE_SCALE;

		TD_SHELL_PRINT(("ADC2 DATA : %ld\n", peak_adc_avg_data));
		TD_SHELL_PRINT(("VOLTAGE : %ld.%ld\n", n_number, dec_point));
		TD_SHELL_PRINT(("----------\r\n"));
	}
	peak_print_f = false;
}

#ifdef TD_PULSE_INTERRUPTx

bool ex_gPulse_high = false;
int current_ctrl_proc = 0;

/* TIM2 OC Interrupt handler */
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM2)
	{
		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
		{
			if (ex_gPulse_high == false)
			{
				TIM2->CCR4 = TD_CATHODE_PULSE_TIME0;
				ex_gPulse_high = true;
			}
			else
			{
				TIM2->CCR4 = TD_CATHODE_PULSE_TIME1;
				ex_gPulse_high = false;
			}
		}

		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
		{
			if (current_ctrl_proc == 0)
			{
				TIM2->CCR1 = TD_CURRENT_CTRL_TIME0;
				current_ctrl_proc = 1;
			}
			else if (current_ctrl_proc == 1)
			{
				TIM2->CCR1 = TD_CURRENT_CTRL_TIME1;
				current_ctrl_proc = 2;
			}
			else if (current_ctrl_proc == 2)
			{
				TIM2->CCR1 = TD_CURRENT_CTRL_TIME2;
				current_ctrl_proc = 3;
			}
			else if (current_ctrl_proc == 3)
			{
				TIM2->CCR1 = TD_CURRENT_CTRL_TIME3;
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
		td_ADC_PeakDetection();
		td_PeakDetection_ADC_Data_Print();
	}
#endif

	/* TIMER 16 100ms */
	if (htim->Instance == TIM16)
	{
		/* ADC1 FSM */
		if (td_Get_ADC1_State() == td_adc1_conv_ok)
		{
			/* SLOPE VOLTAGE RISE CONTROL */
			if (timer16_cnt == 10 && TD_SLOPE_CONTROL_END_FLAG == false)
			{
				td_ADC_Voltage_Feedback();
			}
			/* VOLTAGE RANGE KEEPING CONTROL */
			else if (timer16_cnt == 2 && TD_SLOPE_CONTROL_END_FLAG == true)
			{
				td_ADC_Voltage_Feedback();
			}
			td_Stepup_ADC_Data_Print();

			td_Set_ADC1_State(td_adc1_print_ok);
			timer16_cnt++;
		}
	}
}
