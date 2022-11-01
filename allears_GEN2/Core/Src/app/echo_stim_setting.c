/*
 * echo_pwm_setting.c
 *
 *  Created on: 2022. 10. 20.
 *      Author: ECHO
 */
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include "main.h"
#include "echo_adc.h"
#include "echo_flash_memory.h"
#include "echo_stim_setting.h"
#include "echo_sys_fsm_state.h"

extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim16;
extern DMA_HandleTypeDef hdma_tim2_ch2_ch4;
extern DMA_HandleTypeDef hdma_tim2_ch1;

/* TEST FLAG */
__IO bool vpw_set_flag = false;

/*
 * PWM Parameter
 * */
pwm_pulse_param_t pwm_param;
uint64_t *p64_pwm_param = (uint64_t*) &pwm_param;

volatile uint32_t cathode_pwm_arr[2];
volatile uint32_t current_ctrl_proc_arr[4];

int v_step_tv = VOLTAGE_STEP_TARGET_VALUE;
extern bool gPulse_high;

/****************************************/

/*
 * Timer Setting Parameter
 * */
timer_param_t tim1_param; // SLAVE CLOCK >> PWM
timer_param_t tim2_param; // MASTER CLOCK >> PWM
timer_param_t tim16_param; // STEP UP CLOCK >> PWM
/****************************************/

/* RESPONSE PARAMETER STRING */
typedef struct
{
	char *str;
	uint8_t len;
} get_prm_cmd_str_t;

const get_prm_cmd_str_t get_prm_cmd_str_table[response_prm_cmd_max] =
{
{ "\r\n#resDT:", 9 },
{ "\r\n#resPW:", 9 },
{ "\r\n#resHZ:", 9 },
{ "\r\n#resVPW:", 10 },
{ "\r\n#resALLPRM:", 13 } };
/****************************************/

/* DATA PARSER */
char res_msg[256] =
{ '\0', };
void Echo_Set_DT(uint8_t *data, uint16_t len)
{
	sscanf((const char*) data, (const char*) "#setDT,%hd%*[^\r]",
			&pwm_param.dead_time);
	/* ERROR CONTROL */
	if (pwm_param.dead_time < (GLICH_DEBOUNCING_TIME * 2))
	{
		pwm_param.dead_time = GLICH_DEBOUNCING_TIME * 2;
	}
	Echo_Pulse_Prm_Config();
	Echo_Get_Res_Data(RESPONSE_DEADTIME);
}
void Echo_Set_PW(uint8_t *data, uint16_t len)
{
	sscanf((const char*) data, (const char*) "#setPW,%hd%*[^\r]",
			&pwm_param.pulse_width);
	Echo_Pulse_Prm_Config();
	Echo_Get_Res_Data(RESPONSE_PULSEWIDTH);
}
void Echo_Set_HZ(uint8_t *data, uint16_t len)
{
	sscanf((const char*) data, (const char*) "#setHZ,%hd%*[^\r]",
			&pwm_param.pulse_freq);

	/* ERROR CONTROL */
	if (PULSE_FREQ_ARR <= (TOTAL_PULSE_WIDTH_TIME * 2))
	{
		pwm_param.pulse_freq = 1;
	}

	if (Echo_Get_Sys_FSM_State() == ECHO_SYS_STATE_RUN)
	{
		Echo_Set_Sys_FSM_State_Stop();
		Echo_Pulse_Prm_Config();
		Echo_Set_Sys_FSM_State_Start();
	}
	else
	{
		Echo_Pulse_Prm_Config();
	}

	Echo_Get_Res_Data(RESPONSE_FREQUENCY);

}
void Echo_Set_V_PW(uint8_t *data, uint16_t len)
{
	sscanf((const char*) data, (const char*) "#setVPW,%d%*[^\r]", &v_step_tv);
	if (Echo_Get_Sys_FSM_State() == ECHO_SYS_STATE_RUN)
	{
		HAL_TIM_Base_Start_IT(&htim16);
	}
	Echo_Pulse_V_PW_Config();
	Echo_Get_Res_Data(RESPONSE_VOLTAGE_PW);
}

void Echo_VPW_SET_TP_ON(void)
{
	HAL_GPIO_WritePin(DAC0_GPIO_Port, DAC0_Pin, GPIO_PIN_SET);
}

void Echo_VPW_TP_OFF(void)
{
	HAL_GPIO_WritePin(DAC0_GPIO_Port, DAC0_Pin, GPIO_PIN_RESET);
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
	case RESPONSE_DEADTIME:
		sprintf((char*) res_msg, (const char*) "%s %d us\r\n\r\n", mes_head,
				pwm_param.dead_time);
		break;
	case RESPONSE_PULSEWIDTH:
		sprintf((char*) res_msg, (const char*) "%s %d us\r\n\r\n", mes_head,
				pwm_param.pulse_width);
		break;
	case RESPONSE_FREQUENCY:
		sprintf((char*) res_msg, (const char*) "%s %d Hz\r\n\r\n", mes_head,
				pwm_param.pulse_freq);
		break;
	case RESPONSE_VOLTAGE_PW:
		sprintf((char*) res_msg, (const char*) "%s %d PW\r\n\r\n", mes_head,
				v_step_tv);
		/*TEST CODE*/
		Echo_VPW_SET_TP_ON();
		vpw_set_flag = true;
		/*TEST CODE*/
		break;
	case RESPONSE_ALLPRM:
		sprintf((char*) res_msg, (const char*) "%s\r\n"
				"DT: %d us\r\n"
				"PW: %d us\r\n"
				"HZ: %d Hz\r\n"
				"VPW: %d us\r\n\r\n", mes_head, pwm_param.dead_time,
				pwm_param.pulse_width, pwm_param.pulse_freq, v_step_tv);
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
	pwm_param.dead_time = 20;
	pwm_param.pulse_width = 1000;
	pwm_param.pulse_freq = 100;
	v_step_tv = VOLTAGE_STEP_TARGET_VALUE;
	Echo_Flash_Write();
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
	TIM2->ARR = PULSE_FREQ_ARR - 1;

	/* PULSE and DEAD TIME SETTING */
	TIM2->CCR2 = ANODE_PULSE_TIME;
	cathode_pwm_arr[0] = CATHODE_PULSE_TIME0;
	cathode_pwm_arr[1] = CATHODE_PULSE_TIME1;
	current_ctrl_proc_arr[0] = CURRENT_CTRL_TIME0;
	current_ctrl_proc_arr[1] = CURRENT_CTRL_TIME1;
	current_ctrl_proc_arr[2] = CURRENT_CTRL_TIME2;
	current_ctrl_proc_arr[3] = CURRENT_CTRL_TIME3;
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
	TIM2->CCR2 = ANODE_PULSE_TIME;
	TIM2->CCR4 = CATHODE_PULSE_TIME1;
	TIM2->CCR1 = CURRENT_CTRL_TIME3;
	if (gPulse_high == false)
	{
		TIM2->CCR4 = CATHODE_PULSE_TIME1;
		//gPulse_high = true;
	}
	else
	{
		TIM2->CCR4 = CATHODE_PULSE_TIME0;
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
	TIM1->CCR1 = v_step_tv;
}
/****************************************/

/*
 * Stimulation Voltage Setting Start AND Stop
 * */
void Echo_StepUP_Stop()
{
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
	HAL_TIM_Base_Stop_IT(&htim16);
}

void Echo_StepUP_Start()
{
	TIM1->CCR1 = v_step_tv;
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_Base_Start_IT(&htim16);
}
/****************************************/

