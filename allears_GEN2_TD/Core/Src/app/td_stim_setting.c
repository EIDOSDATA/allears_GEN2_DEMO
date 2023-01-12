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
#include <td_uart.h>
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

/* TIM2_CH1 DMA buffer: WORD size */
volatile uint32_t tim2ch4_current_dma[4];
/* TIM2_CH4 DMA buffer: WORD size */
volatile uint32_t tim2ch1_cathode_dma[2];

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
char res_msg[TD_USART1_RCV_Q_SIZE] =
{ '\0', };

/*
 * SET STIMULATE DEADTIME
 * */
void td_Set_DT(uint8_t *data, uint16_t len)
{
	sscanf((const char*) data, (const char*) "#setDT,%hd%*[^\r]",
			&TD_PULSE_DEAD_TIME);
	/* ERROR CONTROL */
	if (TD_PULSE_DEAD_TIME < (TD_GLICH_DEBOUNCING_TIME * 2))
	{
		TD_PULSE_DEAD_TIME = TD_GLICH_DEBOUNCING_TIME * 2;
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
			&TD_PULSE_WIDTH_TIME);
	td_Pulse_Prm_Config();
	td_Get_Res_Data(res_stim_pulse_width);
}

/*
 * SET STIMULATE FREQUENCY
 * */
void td_Set_HZ(uint8_t *data, uint16_t len)
{
	sscanf((const char*) data, (const char*) "#setHZ,%hd%*[^\r]",
			&TD_PULSE_HZ_FREQ);

	/* ERROR CONTROL */
	if (TD_STIM_ARR <= (TD_TOTAL_PULSE_WIDTH_TIME * 2))
	{
		TD_PULSE_HZ_FREQ = 1;
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
		TD_PULSE_DEAD_TIME);
		break;

	case res_stim_pulse_width:
		sprintf((char*) res_msg, (const char*) "%s %d us\r\n", mes_head,
		TD_PULSE_WIDTH_TIME);
		break;

	case res_stim_frequency:
		sprintf((char*) res_msg, (const char*) "%s %d Hz\r\n", mes_head,
		TD_PULSE_HZ_FREQ);
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
				"DAC: %d step", mes_head, TD_PULSE_DEAD_TIME,
		TD_PULSE_WIDTH_TIME, TD_PULSE_HZ_FREQ,
		TD_VOLTAGE_RELATED_PULSE_WIDTH, TD_VOLTAGE_VALUE_OUTPUT,
		TD_CURRENT_STRENGTH_STEP);
		break;
	default:
		break;
	}
	HAL_UART_Transmit(&TD_USART1_HANDLE, (uint8_t*) "\r\nRES CHNL: UART1\r\n",
			19, 100);
	HAL_UART_Transmit(&TD_USART2_HANDLE, (uint8_t*) "\r\nRES CHNL: UART2\r\n",
			19, 100);
	TD_SHELL_PRINT(("%s\r\n", res_msg));
}

/****************************************/

/*
 * FACTORY RESET
 * */
void td_Factory_Reset()
{
	TD_PULSE_DEAD_TIME = 200;
	TD_PULSE_WIDTH_TIME = 1000;
	TD_PULSE_HZ_FREQ = 20;
	TD_VOLTAGE_RELATED_PULSE_WIDTH = 0;
	TD_VOLTAGE_VALUE_OUTPUT = 0;
	td_Flash_Write();
}

int td_Get_Stim_PSC(void)
{
	return (TIM2->PSC) + 1;
}
int td_Get_Stim_ARR(void)
{
	return (TIM2->ARR) + 1;
}

int td_Get_Stepup_PSC(void)
{
	return (TIM16->PSC) + 1;
}
int td_Get_Stepup_ARR(void)
{
	return (TIM16->ARR) + 1;
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
	TIM2->PSC = TD_STIM_PSC - 1;
	TIM2->ARR = TD_STIM_ARR - 1;

	/* PEAK DETECTION ADC READ TIME SETTING */
	TIM2->CCR3 = TD_ANODE_PULSE_TIME;

	/* PULSE AND DEAD TIME SETTING */
	TIM2->CCR2 = TD_ANODE_PULSE_TIME;
	tim2ch4_current_dma[0] = TD_CURRENT_CTRL_TIME0;
	tim2ch4_current_dma[1] = TD_CURRENT_CTRL_TIME1;
	tim2ch4_current_dma[2] = TD_CURRENT_CTRL_TIME2;
	tim2ch4_current_dma[3] = TD_CURRENT_CTRL_TIME3;

	tim2ch1_cathode_dma[0] = TD_CATHODE_PULSE_TIME0;
	tim2ch1_cathode_dma[1] = TD_CATHODE_PULSE_TIME1;
}
/****************************************/

/*
 * Stimulation Start AND Stop
 */
void td_Stim_Stop()
{
	while (HAL_TIM_OC_Stop_DMA(&htim2, TIM_CHANNEL_1) != HAL_OK)
		;

	while (HAL_TIM_OC_Stop_DMA(&htim2, TIM_CHANNEL_4) != HAL_OK)
		;

	while (HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2) != HAL_OK)
		;

	td_StepUP_Stop();
	td_Stop_ADC2_Conv();
}

void td_Stim_Start()
{

#if 1
	/* USER CODE BEGIN TIM2_Init 0 */

	/* USER CODE END TIM2_Init 0 */

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
	sConfigOC.Pulse = 1200;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_OC_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
	{
		Error_Handler();
	}
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = TD_ANODE_PULSE_TIME;
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
	sConfigOC.Pulse = 10;
	if (HAL_TIM_OC_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN TIM2_Init 2 */
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = TD_STIM_PSC;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = TD_STIM_ARR;
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

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = TD_ANODE_PULSE_TIME;
	if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
	{
		Error_Handler();
	}

	sConfigOC.Pulse = TD_TIM2_ADC2_TRG_TIME;
	if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
	{
		Error_Handler();
	}

	/* USER CODE END TIM2_Init 2 */
	HAL_TIM_MspPostInit(&htim2);

#endif

	td_Pulse_Prm_Config();
	/* START PULSE TIMER */
	while (HAL_TIM_OC_Start_DMA(&htim2, TIM_CHANNEL_4,
			(uint32_t*) tim2ch4_current_dma, 4) != HAL_OK)
		;
	while (HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2) != HAL_OK)
		;
	while (HAL_TIM_OC_Start_DMA(&htim2, TIM_CHANNEL_1,
			(uint32_t*) tim2ch1_cathode_dma, 2) != HAL_OK)
		;

	/* ENABLE DMA ERROR INTERRUPT ONLYs */
	__HAL_DMA_DISABLE_IT(&hdma_tim2_ch1, (DMA_IT_TC | DMA_IT_HT));
	__HAL_DMA_DISABLE_IT(&hdma_tim2_ch2_ch4, (DMA_IT_TC | DMA_IT_HT));

	td_StepUP_Start();
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

#define VOLTAGE_DIFFERENCE_ABS		abs(voltage_scaleup_val - adc_voltage)
#define FAST_STEPUP_VOLTAGE			4 * STEPUP_VOLTAGE_SCALE // 4 voltage
#define FAST_STEPUP_ENABLE			VOLTAGE_DIFFERENCE_ABS > FAST_STEPUP_VOLTAGE

#define OUTPUT_VOLTAGE_IS_LOW		voltage_scaleup_val > adc_voltage
#define OUTPUT_VOLTAGE_IS_HIGH		voltage_scaleup_val < adc_voltage
#define OUTPUT_VOLTAGE_IS_SAME		voltage_scaleup_val == adc_voltage

	if (VOLTAGE_DIFFERENCE_ABS < FEEDBACK_VOLTAGE_RANGE_VALUE)
	{
		if (OUTPUT_VOLTAGE_IS_LOW)
		{
			TD_VOLTAGE_RELATED_PULSE_WIDTH++;
			if (TD_VOLTAGE_RELATED_PULSE_WIDTH > TD_VOLTAGE_TABLE_MAX_VALUE - 1)
			{
				TD_VOLTAGE_RELATED_PULSE_WIDTH = TD_VOLTAGE_TABLE_MAX_VALUE - 1;
			}
		}
		else if (OUTPUT_VOLTAGE_IS_HIGH)
		{
			TD_VOLTAGE_RELATED_PULSE_WIDTH--;
		}
		TD_SLOPE_CONTROL_END_FLAG = true;
		TIM1->CCR1 = TD_VOLTAGE_RELATED_PULSE_WIDTH;
		return HAL_OK;
	}

	else if (VOLTAGE_DIFFERENCE_ABS > FEEDBACK_VOLTAGE_RANGE_VALUE)
	{
		if (OUTPUT_VOLTAGE_IS_LOW)
		{
			/* FAST STEPUP */
			/* When you set the voltage, raise the voltage here.
			 * */
			if (FAST_STEPUP_ENABLE)
			{
				TD_VOLTAGE_RELATED_PULSE_WIDTH += 7;
			}
			else
			{
				TD_VOLTAGE_RELATED_PULSE_WIDTH += 1;
			}

			TD_SLOPE_CONTROL_END_FLAG = false;
			if (TD_VOLTAGE_RELATED_PULSE_WIDTH > TD_VOLTAGE_TABLE_MAX_VALUE - 1)
			{
				TD_VOLTAGE_RELATED_PULSE_WIDTH = TD_VOLTAGE_TABLE_MAX_VALUE - 1;
			}
		}
		else if (OUTPUT_VOLTAGE_IS_HIGH)
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

	else if (OUTPUT_VOLTAGE_IS_SAME)
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
	TIM16->CNT = 0;
	TIM16->PSC = TD_STEPUP_FDBK_PSC - 1;
	TIM16->ARR = TD_STEPUP_FDBK_ARR - 1;

	TIM1->CCR1 = TD_VOLTAGE_RELATED_PULSE_WIDTH;
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_Base_Start_IT(&htim16);
}
/****************************************/

