/*
 * td_pwm_setting.c
 *
 *  Created on: 2022. 10. 20.
 *      Author: ECHO
 */
#include <stdint.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include <math.h>
#include <td_adc.h>
#include <td_flash_memory.h>
#include <td_shell.h>
#include <td_stim_setting.h>
#include <td_sys_fsm_state.h>
#include "main.h"

extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim16;
extern DMA_HandleTypeDef hdma_tim2_ch2_ch4;
extern DMA_HandleTypeDef hdma_tim2_ch1;

/*
 * PWM Parameter
 * */
pwm_pulse_param_t ex_pwm_param;
uint64_t *ex_p64_pwm_param = (uint64_t*) &ex_pwm_param;

volatile uint32_t cathode_pwm_arr[2];
volatile uint32_t current_ctrl_proc_arr[4];

__IO int ex_voltage_r_pw = 0;
__IO int ex_voltage_val_output = 0;
__IO int ex_current_strength_step = 0;
__IO bool ex_slope_ctrl_end_f = false;
extern bool ex_gPulse_high;

/****************************************/

/*
 * Timer Setting Parameter
 * */
timer_param_t tim1_param; // SLAVE CLOCK >> PWM
timer_param_t tim2_param; // MASTER CLOCK >> PWM
timer_param_t tim16_param; // STEP UP CLOCK >> PWM
/****************************************/

/****** RESPONSE PARAMETER STRING ******/
typedef struct
{
	char *str;
	uint8_t len;
} get_prm_cmd_str_t;

/*
 * RESPONSE Message String Table
 * */
const get_prm_cmd_str_t get_prm_cmd_str_table[res_prm_cmd_max] =
{
{ "\r\n#resDT:", 9 },
{ "\r\n#resPW:", 9 },
{ "\r\n#resHZ:", 9 },
{ "\r\n#resVPW:", 10 },
{ "\r\n#resVOL:", 10 },
{ "\r\n#resDAC:", 10 },
{ "\r\n#resALLPRM:", 13 } };
/****************************************/

/****** DATA PARSER ******/
char res_msg[256] =
{ '\0', };

/*
 * SET STIMULATE DEADTIME
 * */
void td_Set_DT(uint8_t *data, uint16_t len)
{
	sscanf((const char*) data, (const char*) "#setDT,%hd%*[^\r]",
			&ex_pwm_param.dead_time);
	/* ERROR CONTROL */
	if (ex_pwm_param.dead_time < (TD_GLICH_DEBOUNCING_TIME * 2))
	{
		ex_pwm_param.dead_time = TD_GLICH_DEBOUNCING_TIME * 2;
	}
	td_Pulse_Prm_Config();
	td_Get_Res_Data(res_stim_deadtime);
}

/*
 * SET STIMULATE PULSE WIDTH
 * */
void td_Set_PW(uint8_t *data, uint16_t len)
{
	sscanf((const char*) data, (const char*) "#setPW,%hd%*[^\r]",
			&ex_pwm_param.pulse_width);
	td_Pulse_Prm_Config();
	td_Get_Res_Data(res_stim_pulse_width);
}

/*
 * SET STIMULATE FREQUENCY
 * */
void td_Set_HZ(uint8_t *data, uint16_t len)
{
	sscanf((const char*) data, (const char*) "#setHZ,%hd%*[^\r]",
			&ex_pwm_param.pulse_freq);

	/* ERROR CONTROL */
	if (TD_PULSE_FREQ_ARR <= (TD_TOTAL_PULSE_WIDTH_TIME * 2))
	{
		ex_pwm_param.pulse_freq = 1;
	}

	if (td_Get_Sys_FSM_State() == td_sys_state_run)
	{
		td_Set_Sys_FSM_State_Stop();
		td_Pulse_Prm_Config();
		td_Set_Sys_FSM_State_Start();
	}
	else
	{
		td_Pulse_Prm_Config();
	}

	td_Get_Res_Data(res_stim_frequency);

}

/*
 * SET VOLTAGE PULSE WIDTH
 * */
void td_Set_V_PW(uint8_t *data, uint16_t len)
{
	sscanf((const char*) data, (const char*) "#setVPW,%d%*[^\r]",
			&TD_VOLTAGE_RELATED_PULSE_WIDTH);
	if (td_Get_Sys_FSM_State() == td_sys_state_run)
	{
		HAL_TIM_Base_Start_IT(&htim16);
	}
	td_Pulse_V_PW_Config();
	td_Get_Res_Data(res_voltage_pulse_width);
}

/*
 * SET TARGET VOLTAGE
 * */
void td_Set_Voltage_Output(uint8_t *data, uint16_t len)
{
	sscanf((const char*) data, (const char*) "#setVOL,%d%*[^\r]",
			&TD_VOLTAGE_VALUE_OUTPUT);
	if (td_Get_Sys_FSM_State() == td_sys_state_run)
	{
		HAL_TIM_Base_Start_IT(&htim16);
	}
	td_Get_Res_Data(res_target_voltage_value);
}

/*
 * CURRENT CONTROL
 * */
void td_Set_Current_Strength(uint8_t *data, uint16_t len)
{
	sscanf((const char*) data, (const char*) "#setDAC,%d%*[^\r]",
			&TD_CURRENT_STRENGTH_STEP);
	if (td_Get_Sys_FSM_State() == td_sys_state_run)
	{
		HAL_TIM_Base_Start_IT(&htim16);
	}

	if (TD_CURRENT_STRENGTH_STEP > 15 || TD_CURRENT_STRENGTH_STEP < 0)
	{
		TD_CURRENT_STRENGTH_STEP = 0;
		TD_CURRENT_DAC_ALL_OFF;
	}
	else
	{
		TD_CURRENT_DAC_ALL_OFF;
		TD_CURRENT_DAC_CONTROL;
	}

	td_Get_Res_Data(res_current_strength);
}

/****************************************/

/*
 * DATA PRINTER
 * */
void td_Get_Res_Data(uint8_t select_msg)
{
	char mes_head[11] =
	{ '\0', };
	strcpy((char*) mes_head,
			(const char*) get_prm_cmd_str_table[select_msg].str);

	switch (select_msg)
	{
	case res_stim_deadtime:
		sprintf((char*) res_msg, (const char*) "%s %d us\r\n", mes_head,
				ex_pwm_param.dead_time);
		break;

	case res_stim_pulse_width:
		sprintf((char*) res_msg, (const char*) "%s %d us\r\n", mes_head,
				ex_pwm_param.pulse_width);
		break;

	case res_stim_frequency:
		sprintf((char*) res_msg, (const char*) "%s %d Hz\r\n", mes_head,
				ex_pwm_param.pulse_freq);
		break;

	case res_voltage_pulse_width:
		sprintf((char*) res_msg, (const char*) "%s %d step\r\n", mes_head,
		TD_VOLTAGE_RELATED_PULSE_WIDTH);
		break;

	case res_target_voltage_value:
		sprintf((char*) res_msg, (const char*) "%s %d v\r\n", mes_head,
		TD_VOLTAGE_VALUE_OUTPUT);
		break;

	case res_current_strength:
		sprintf((char*) res_msg, (const char*) "%s %d step\r\n", mes_head,
		TD_CURRENT_STRENGTH_STEP);
		break;

	case res_allprm:
		sprintf((char*) res_msg, (const char*) "%s\r\n"
				"DT: %d us\r\n"
				"PW: %d us\r\n"
				"HZ: %d Hz\r\n"
				"VPW: %d\r\n"
				"VOL: %d\r\n"
				"DAC: %d step", mes_head, ex_pwm_param.dead_time,
				ex_pwm_param.pulse_width, ex_pwm_param.pulse_freq,
				TD_VOLTAGE_RELATED_PULSE_WIDTH, TD_VOLTAGE_VALUE_OUTPUT,
				TD_CURRENT_STRENGTH_STEP);
		break;
	default:
		break;
	}
	TD_SHELL_PRINT(("%s\r\n", res_msg));
}

/****************************************/

/*
 * FACTORY RESET
 * */
void td_Factory_Reset()
{
	ex_pwm_param.dead_time = 20;
	ex_pwm_param.pulse_width = 1000;
	ex_pwm_param.pulse_freq = 100;
	TD_VOLTAGE_RELATED_PULSE_WIDTH = 0;
	TD_VOLTAGE_VALUE_OUTPUT = 0;
	td_Flash_Write();
}
/****************************************/

#ifdef TD_PULSE_DMA
/*
 * PWM VALUE WRITE TO REGISTOR
 * */
void td_Pulse_Prm_Config()
{
	/* HZ SETTING */
	TIM2->CNT = 0;
	TIM2->ARR = TD_PULSE_FREQ_ARR - 1;

	/* PULSE and DEAD TIME SETTING */
	TIM2->CCR2 = TD_ANODE_PULSE_TIME;
	cathode_pwm_arr[0] = TD_CATHODE_PULSE_TIME0;
	cathode_pwm_arr[1] = TD_CATHODE_PULSE_TIME1;
	current_ctrl_proc_arr[0] = TD_CURRENT_CTRL_TIME0;
	current_ctrl_proc_arr[1] = TD_CURRENT_CTRL_TIME1;
	current_ctrl_proc_arr[2] = TD_CURRENT_CTRL_TIME2;
	current_ctrl_proc_arr[3] = TD_CURRENT_CTRL_TIME3;
}
/****************************************/

/*
 * Stimulation Start AND Stop
 */
void td_Stim_Stop()
{
	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2); // ANODE
	HAL_TIM_OC_Stop_DMA(&htim2, TIM_CHANNEL_1); // CURRENT
	HAL_TIM_OC_Stop_DMA(&htim2, TIM_CHANNEL_4); // CATHODE
	td_StepUP_Stop();
	td_Stop_ADC2_Conv();
	HAL_TIM_Base_Stop_IT(&htim2);
}

void td_Stim_Start()
{

#if 1
	TIM_MasterConfigTypeDef sMasterConfig =
	{ 0 };
	TIM_OC_InitTypeDef sConfigOC =
	{ 0 };

	/* USER CODE BEGIN TIM2_Init 1 */

	/* USER CODE END TIM2_Init 1 */
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 79;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 9999;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_OC_Init(&htim2) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_OC3REF;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	sConfigOC.OCMode = TIM_OCMODE_TOGGLE;
	sConfigOC.Pulse = 10;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_OC_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
	{
		Error_Handler();
	}
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 1100;
	if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
	{
		Error_Handler();
	}
	sConfigOC.Pulse = 0;
	if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
	{
		Error_Handler();
	}
	sConfigOC.OCMode = TIM_OCMODE_TOGGLE;
	sConfigOC.Pulse = 1040;
	if (HAL_TIM_OC_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN TIM2_Init 2 */

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 40;
	if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
	{
		Error_Handler();
	}

	/* USER CODE END TIM2_Init 2 */
	HAL_TIM_MspPostInit(&htim2);
#endif

	td_Pulse_Prm_Config();

	HAL_TIM_OC_Start_DMA(&htim2, TIM_CHANNEL_1,
			(uint32_t*) current_ctrl_proc_arr, 4); // PA5 TIM2 CH_1 CURRENT CONTROL
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); // PA1 TIM2 CH_2 ANODE
	HAL_TIM_OC_Start_DMA(&htim2, TIM_CHANNEL_4, (uint32_t*) cathode_pwm_arr, 2); // PA3 TIM2 CH_4 CATHODE

	__HAL_DMA_DISABLE_IT(&hdma_tim2_ch2_ch4, (DMA_IT_TC | DMA_IT_HT)); // HAL_DMA_Start_IT
	__HAL_DMA_DISABLE_IT(&hdma_tim2_ch1, (DMA_IT_TC | DMA_IT_HT)); // HAL_DMA_Start_IT

	td_StepUP_Start();

#if NOT_USEAGE_TRGO
	td_Start_ADC2_Conv();
	HAL_TIM_Base_Start_IT(&htim2);
#endif
}
#endif

#ifdef TD_PULSE_INTERRUPTx
void td_Pulse_Prm_Config()
{
	TIM2->CCR2 = TD_ANODE_PULSE_TIME;
	TIM2->CCR4 = TD_CATHODE_PULSE_TIME1;
	TIM2->CCR1 = TD_CURRENT_CTRL_TIME3;
	if (ex_gPulse_high == false)
	{
		TIM2->CCR4 = TD_CATHODE_PULSE_TIME1;
		//gPulse_high = true;
	}
	else
	{
		TIM2->CCR4 = TD_CATHODE_PULSE_TIME0;
		//gPulse_high = false;
	}
}

void td_Stim_Stop()
{
	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2); // ANODE
	HAL_TIM_OC_Stop_IT(&htim2, TIM_CHANNEL_4); // CATHODE
	HAL_TIM_OC_Stop_IT(&htim2, TIM_CHANNEL_1); // CURRENT
	td_StepUP_Stop();
}

void td_Stim_Start()
{
	td_Pulse_Prm_Config();

	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); // ANODE
	HAL_TIM_OC_Start_IT(&htim2, TIM_CHANNEL_4); // CATHODE
	HAL_TIM_OC_Start_IT(&htim2, TIM_CHANNEL_1); // CURRENT

	td_StepUP_Start();
}
#endif

/****************************************/

/*
 * STEP UP PWM VALUE WRITE TO REGISTOR
 * */
void td_Pulse_V_PW_Config()
{
	TIM1->CCR1 = TD_VOLTAGE_RELATED_PULSE_WIDTH;
}

/*
 * STEP UP Feedback Control
 * */
uint32_t td_Voltage_Config(uint64_t adc_voltage)
{
	uint32_t voltage_scaleup_val = TD_VOLTAGE_VALUE_OUTPUT
			* STEPUP_VOLTAGE_SCALE;
	if (abs(voltage_scaleup_val - adc_voltage) < VOLTAGE_ERROR_RANGE_VALUE)
	{
		if (voltage_scaleup_val > adc_voltage)
		{
			TD_VOLTAGE_RELATED_PULSE_WIDTH++;
		}
		else if (voltage_scaleup_val < adc_voltage)
		{
			TD_VOLTAGE_RELATED_PULSE_WIDTH--;
		}
		TD_SLOPE_CONTROL_END_FLAG = true;
		TIM1->CCR1 = TD_VOLTAGE_RELATED_PULSE_WIDTH;
		return HAL_OK;
	}

	else if (abs(voltage_scaleup_val - adc_voltage) > VOLTAGE_ERROR_RANGE_VALUE)
	{
		if (voltage_scaleup_val > adc_voltage)
		{
			TD_VOLTAGE_RELATED_PULSE_WIDTH++;
			TD_SLOPE_CONTROL_END_FLAG = false;
		}
		else if (voltage_scaleup_val < adc_voltage)
		{
			TD_VOLTAGE_RELATED_PULSE_WIDTH--;
			TD_SLOPE_CONTROL_END_FLAG = true;
			if (TD_VOLTAGE_RELATED_PULSE_WIDTH <= 0)
			{
				TD_VOLTAGE_RELATED_PULSE_WIDTH = 0;
			}
		}
		TIM1->CCR1 = TD_VOLTAGE_RELATED_PULSE_WIDTH;
		return HAL_OK;
	}

	else if (voltage_scaleup_val == adc_voltage)
	{
		TD_SLOPE_CONTROL_END_FLAG = true;
	}

	else
	{
		TD_SLOPE_CONTROL_END_FLAG = true;
	}

	return HAL_OK;
}
/****************************************/

/*
 * Stimulation Voltage Setting Start AND Stop
 * */
void td_StepUP_Stop()
{
	TD_VOLTAGE_RELATED_PULSE_WIDTH = 0;
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
	HAL_TIM_Base_Stop_IT(&htim16);
}

void td_StepUP_Start()
{
	TIM1->CCR1 = TD_VOLTAGE_RELATED_PULSE_WIDTH;
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_Base_Start_IT(&htim16);
}
/****************************************/

