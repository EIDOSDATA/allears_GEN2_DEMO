/*
 * echo_pwm_setting.c
 *
 *  Created on: 2022. 10. 20.
 *      Author: ECHO
 */
#include <stdint.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include <math.h>
#include <echo_adc.h>
#include <echo_flash_memory.h>
#include <echo_shell.h>
#include <echo_stim_setting.h>
#include <echo_sys_fsm_state.h>
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
void Echo_Set_DT(uint8_t *data, uint16_t len)
{
	sscanf((const char*) data, (const char*) "#setDT,%hd%*[^\r]",
			&ECHO_PULSE_DEAD_TIME);

	/* ERROR CONTROL */
	if (ECHO_PULSE_DEAD_TIME < (ECHO_GLICH_DEBOUNCING_TIME * 2))
	{
		ECHO_PULSE_DEAD_TIME = ECHO_GLICH_DEBOUNCING_TIME * 2;
	}
	Echo_Pulse_Prm_Config();
	Echo_Get_Res_Data(res_stim_deadtime);
}

/*
 * SET STIMULATE PULSE WIDTH
 * */
void Echo_Set_PW(uint8_t *data, uint16_t len)
{
	sscanf((const char*) data, (const char*) "#setPW,%hd%*[^\r]",
			&ECHO_PULSE_WIDTH_TIME);
	Echo_Pulse_Prm_Config();
	Echo_Get_Res_Data(res_stim_pulse_width);
}

/*
 * SET STIMULATE FREQUENCY
 * */
void Echo_Set_HZ(uint8_t *data, uint16_t len)
{
	sscanf((const char*) data, (const char*) "#setHZ,%hd%*[^\r]",
			&ECHO_PULSE_HZ_FREQ);

	/* ERROR CONTROL */
	if (ECHO_PULSE_FREQ_ARR <= (ECHO_TOTAL_PULSE_WIDTH_TIME * 2))
	{
		ECHO_PULSE_HZ_FREQ = 1;
	}

	if (Echo_Get_Sys_FSM_State() == echo_sys_state_run)
	{
		Echo_Set_Sys_FSM_State_Stop();
		Echo_Pulse_Prm_Config();
		Echo_Set_Sys_FSM_State_Start();
	}
	else
	{
		Echo_Pulse_Prm_Config();
	}
	Echo_Get_Res_Data(res_stim_frequency);
}

/*
 * SET VOLTAGE PULSE WIDTH
 * */
void Echo_Set_V_PW(uint8_t *data, uint16_t len)
{
	sscanf((const char*) data, (const char*) "#setVPW,%d%*[^\r]",
			&ECHO_VOLTAGE_RELATED_PULSE_WIDTH);
	if (Echo_Get_Sys_FSM_State() == echo_sys_state_run)
	{
		HAL_TIM_Base_Start_IT(&htim16);
	}
	Echo_Pulse_V_PW_Config();
	Echo_Get_Res_Data(res_voltage_pulse_width);
}

/*
 * SET TARGET VOLTAGE
 * */
void Echo_Set_Voltage_Output(uint8_t *data, uint16_t len)
{
	sscanf((const char*) data, (const char*) "#setVOL,%d%*[^\r]",
			&ECHO_VOLTAGE_VALUE_OUTPUT);
	if (Echo_Get_Sys_FSM_State() == echo_sys_state_run)
	{
		HAL_TIM_Base_Start_IT(&htim16);
	}
	Echo_Get_Res_Data(res_target_voltage_value);
}

/*
 * CURRENT CONTROL
 * */
void Echo_Set_Current_Strength(uint8_t *data, uint16_t len)
{
	sscanf((const char*) data, (const char*) "#setDAC,%d%*[^\r]",
			&ECHO_CURRENT_STRENGTH_STEP);
	if (Echo_Get_Sys_FSM_State() == echo_sys_state_run)
	{
		HAL_TIM_Base_Start_IT(&htim16);
	}

	if (ECHO_CURRENT_STRENGTH_STEP > 15 || ECHO_CURRENT_STRENGTH_STEP < 0)
	{
		ECHO_CURRENT_STRENGTH_STEP = 0;
		ECHO_CURRENT_DAC_ALL_OFF;
	}
	else
	{
		ECHO_CURRENT_DAC_ALL_OFF;
		ECHO_CURRENT_DAC_CONTROL;
	}

	Echo_Get_Res_Data(res_current_strength);
}

/****************************************/

/*
 * DATA PRINTER
 * */
void Echo_Get_Res_Data(uint8_t select_msg)
{
	char mes_head[11] =
	{ '\0', };
	strcpy((char*) mes_head,
			(const char*) get_prm_cmd_str_table[select_msg].str);

	switch (select_msg)
	{
	case res_stim_deadtime:
		sprintf((char*) res_msg, (const char*) "%s %d us\r\n", mes_head,
		ECHO_PULSE_DEAD_TIME);
		break;

	case res_stim_pulse_width:
		sprintf((char*) res_msg, (const char*) "%s %d us\r\n", mes_head,
		ECHO_PULSE_WIDTH_TIME);
		break;

	case res_stim_frequency:
		sprintf((char*) res_msg, (const char*) "%s %d Hz\r\n", mes_head,
		ECHO_PULSE_HZ_FREQ);
		break;

	case res_voltage_pulse_width:
		sprintf((char*) res_msg, (const char*) "%s %d step\r\n", mes_head,
		ECHO_VOLTAGE_RELATED_PULSE_WIDTH);
		break;

	case res_target_voltage_value:
		sprintf((char*) res_msg, (const char*) "%s %d v\r\n", mes_head,
		ECHO_VOLTAGE_VALUE_OUTPUT);
		break;

	case res_current_strength:
		sprintf((char*) res_msg, (const char*) "%s %d step\r\n", mes_head,
		ECHO_CURRENT_STRENGTH_STEP);
		break;

	case res_allprm:
		sprintf((char*) res_msg, (const char*) "%s\r\n"
				"DT: %d us\r\n"
				"PW: %d us\r\n"
				"HZ: %d Hz\r\n"
				"VPW: %d step\r\n"
				"VOL: %d v\r\n"
				"DAC: %d step", mes_head, ECHO_PULSE_DEAD_TIME,
		ECHO_PULSE_WIDTH_TIME, ECHO_PULSE_HZ_FREQ,
		ECHO_VOLTAGE_RELATED_PULSE_WIDTH, ECHO_VOLTAGE_VALUE_OUTPUT,
		ECHO_CURRENT_STRENGTH_STEP);
		break;
	default:
		break;
	}
	ECHO_SHELL_PRINT(("%s\r\n", res_msg));
}

/****************************************/

/*
 * FACTORY RESET
 * */
void Echo_Factory_Reset()
{
	ECHO_PULSE_DEAD_TIME = 10;
	ECHO_PULSE_WIDTH_TIME = 1000;
	ECHO_PULSE_HZ_FREQ = 10;
	ECHO_VOLTAGE_RELATED_PULSE_WIDTH = 0;
	ECHO_VOLTAGE_VALUE_OUTPUT = 0;
	Echo_Flash_Write();
}
int Echo_Get_Pulse_PSC(void)
{
	return (TIM2->PSC) + 1;
}
int Echo_Get_Pulse_ARR(void)
{
	return (TIM2->ARR) + 1;
}

/****************************************/

#ifdef ECHO_PULSE_DMA
/*
 * PWM VALUE WRITE TO REGISTOR
 * */
void Echo_Pulse_Prm_Config()
{
	/* HZ SETTING */
	TIM2->CNT = 0;
	TIM2->ARR = ECHO_PULSE_FREQ_ARR - 1;

	/* PULSE and DEAD TIME SETTING */
	TIM2->CCR2 = ECHO_ANODE_PULSE_TIME;
	cathode_pwm_arr[0] = ECHO_CATHODE_PULSE_TIME0;
	cathode_pwm_arr[1] = ECHO_CATHODE_PULSE_TIME1;
	current_ctrl_proc_arr[0] = ECHO_CURRENT_CTRL_TIME0;
	current_ctrl_proc_arr[1] = ECHO_CURRENT_CTRL_TIME1;
	current_ctrl_proc_arr[2] = ECHO_CURRENT_CTRL_TIME2;
	current_ctrl_proc_arr[3] = ECHO_CURRENT_CTRL_TIME3;
}
/****************************************/

/*
 * Stimulation Start AND Stop
 */
void Echo_Stim_Stop()
{
	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2); // ANODE
	HAL_TIM_OC_Stop_DMA(&htim2, TIM_CHANNEL_1); // CURRENT
	HAL_TIM_OC_Stop_DMA(&htim2, TIM_CHANNEL_4); // CATHODE
	Echo_StepUP_Stop();
	Echo_Stop_ADC2_Conv();
	HAL_TIM_Base_Stop_IT(&htim2);
}

void Echo_Stim_Start()
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
	htim2.Init.Period = 999;
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
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	sConfigOC.OCMode = TIM_OCMODE_TOGGLE;
	sConfigOC.Pulse = 5;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_OC_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
	{
		Error_Handler();
	}
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 1010;
	if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
	{
		Error_Handler();
	}
	__HAL_TIM_DISABLE_OCxPRELOAD(&htim2, TIM_CHANNEL_2);
	sConfigOC.OCMode = TIM_OCMODE_TOGGLE;
	sConfigOC.Pulse = 1040;
	if (HAL_TIM_OC_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN TIM2_Init 2 */

	/* USER CODE END TIM2_Init 2 */
	HAL_TIM_MspPostInit(&htim2);
#endif

	Echo_Pulse_Prm_Config();

	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); // PA1 TIM2 CH_2 ANODE

	HAL_TIM_OC_Start_DMA(&htim2, TIM_CHANNEL_1,
			(uint32_t*) current_ctrl_proc_arr, 4); // PA5 TIM2 CH_1 CURRENT CONTROL
	__HAL_DMA_DISABLE_IT(&hdma_tim2_ch1, (DMA_IT_TC | DMA_IT_HT)); // HAL_DMA_Start_IT

	HAL_TIM_OC_Start_DMA(&htim2, TIM_CHANNEL_4, (uint32_t*) cathode_pwm_arr, 2); // PA3 TIM2 CH_4 CATHODE
	__HAL_DMA_DISABLE_IT(&hdma_tim2_ch2_ch4, (DMA_IT_TC | DMA_IT_HT)); // HAL_DMA_Start_IT

	Echo_StepUP_Start();

#if NOT_USEAGE_TRGO
	Echo_Start_ADC2_Conv();
	HAL_TIM_Base_Start_IT(&htim2);
#endif
}
#endif

#ifdef ECHO_PULSE_INTERRUPTx
void Echo_Pulse_Prm_Config()
{
	TIM2->CCR2 = ECHO_ANODE_PULSE_TIME;
	TIM2->CCR4 = ECHO_CATHODE_PULSE_TIME1;
	TIM2->CCR1 = ECHO_CURRENT_CTRL_TIME3;
	if (ex_gPulse_high == false)
	{
		TIM2->CCR4 = ECHO_CATHODE_PULSE_TIME1;
		//gPulse_high = true;
	}
	else
	{
		TIM2->CCR4 = ECHO_CATHODE_PULSE_TIME0;
		//gPulse_high = false;
	}
}

void Echo_Stim_Stop()
{
	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2); // ANODE
	HAL_TIM_OC_Stop_IT(&htim2, TIM_CHANNEL_4); // CATHODE
	HAL_TIM_OC_Stop_IT(&htim2, TIM_CHANNEL_1); // CURRENT
	Echo_StepUP_Stop();
}

void Echo_Stim_Start()
{
	Echo_Pulse_Prm_Config();

	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); // ANODE
	HAL_TIM_OC_Start_IT(&htim2, TIM_CHANNEL_4); // CATHODE
	HAL_TIM_OC_Start_IT(&htim2, TIM_CHANNEL_1); // CURRENT

	Echo_StepUP_Start();
}
#endif

/****************************************/

/*
 * STEP UP PWM VALUE WRITE TO REGISTOR
 * */
void Echo_Pulse_V_PW_Config()
{
	TIM1->CCR1 = ECHO_VOLTAGE_RELATED_PULSE_WIDTH;
}

/*
 * STEP UP Feedback Control
 * */
uint32_t Echo_Voltage_Config(uint64_t adc_voltage)
{
	uint64_t voltage_scaleup_val = ECHO_VOLTAGE_VALUE_OUTPUT
			* STEPUP_VOLTAGE_SCALE;
	if (abs(voltage_scaleup_val - adc_voltage) < VOLTAGE_ERROR_RANGE_VALUE)
	{
		if (voltage_scaleup_val > adc_voltage)
		{
			ECHO_VOLTAGE_RELATED_PULSE_WIDTH++;
		}
		else if (voltage_scaleup_val < adc_voltage)
		{
			ECHO_VOLTAGE_RELATED_PULSE_WIDTH--;
		}
		ECHO_SLOPE_CONTROL_END_FLAG = true;
		TIM1->CCR1 = ECHO_VOLTAGE_RELATED_PULSE_WIDTH;
		return HAL_OK;
	}

	else if (abs(voltage_scaleup_val - adc_voltage) > VOLTAGE_ERROR_RANGE_VALUE)
	{
		if (voltage_scaleup_val > adc_voltage)
		{
			ECHO_VOLTAGE_RELATED_PULSE_WIDTH++;
			ECHO_SLOPE_CONTROL_END_FLAG = false;
		}
		else if (voltage_scaleup_val < adc_voltage)
		{
			ECHO_VOLTAGE_RELATED_PULSE_WIDTH--;
			ECHO_SLOPE_CONTROL_END_FLAG = true;
			if (ECHO_VOLTAGE_RELATED_PULSE_WIDTH <= 0)
			{
				ECHO_VOLTAGE_RELATED_PULSE_WIDTH = 0;
			}
		}
		TIM1->CCR1 = ECHO_VOLTAGE_RELATED_PULSE_WIDTH;
		return HAL_OK;
	}

	else if (voltage_scaleup_val == adc_voltage)
	{
		ECHO_SLOPE_CONTROL_END_FLAG = true;
	}

	else
	{
		ECHO_SLOPE_CONTROL_END_FLAG = true;
	}

	return HAL_OK;
}
/****************************************/

/*
 * Stimulation Voltage Setting Start AND Stop
 * */
void Echo_StepUP_Stop()
{
	ECHO_VOLTAGE_RELATED_PULSE_WIDTH = 0;
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
	HAL_TIM_Base_Stop_IT(&htim16);
}

void Echo_StepUP_Start()
{
	TIM16->CNT = 0;
	TIM16->ARR = 1000 / ECHO_STIM_SCALE;
	TIM1->CCR1 = ECHO_VOLTAGE_RELATED_PULSE_WIDTH;
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_Base_Start_IT(&htim16);
}
/****************************************/

