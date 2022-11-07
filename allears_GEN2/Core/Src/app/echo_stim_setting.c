/*
 * echo_pwm_setting.c
 *
 *  Created on: 2022. 10. 20.
 *      Author: ECHO
 */

#include <string.h>
#include <strings.h>
#include "main.h"
#include "echo_flash_memory.h"
#include "echo_stim_setting.h"
#include "echo_state.h"

#define MASTER_CLK_FREQ					80000000
#define MASTER_PSC						80
#define MASTER_ARR						1000000
#define ECHO_SET_HZ_PERIOD				10

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim16;
extern DMA_HandleTypeDef hdma_tim2_ch2_ch4;

/*
 * PWM Parameter
 * */
pwm_pulse_param_t pwm_param;
uint64_t *p64_pwm_param = (uint64_t*) &pwm_param;

uint32_t ano_matching_tim1 = 0;
uint32_t cat_matching_tim1 = 0;
uint32_t cat_matching_tim2 = 0;

volatile uint32_t pwm_arr[2];

int v_step_tv = 5000;
int v_step_val = 0;
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
	if (Echo_Get_FSM_State() == ECHO_STATE_RUN)
	{
		Echo_Set_FSM_State_Stop();
		Echo_Pulse_Prm_Config();
		Echo_Set_FSM_State_Start();
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
	if (Echo_Get_FSM_State() == ECHO_STATE_RUN)
	{
		HAL_TIM_Base_Start_IT(&htim16);
	}
	//Echo_Pulse_V_PW_Config();
	Echo_Get_Res_Data(RESPONSE_VOLTAGE_PW);
}
/****************************************/

/*
 * DATA PRINTER
 * */
void Echo_Get_Res_Data(uint8_t select_msg)
{
	char mes_head[11] =
	{ '\0', };

	//get_prm_cmd_str_table[admin_cmd_cnt].str
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
		sprintf((char*) res_msg, (const char*) "%s %d us\r\n\r\n", mes_head,
				v_step_tv);
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
 * PWM VALUE WRITE TO REGISTOR
 * */
void Echo_Pulse_Prm_Config()
{
	uint32_t arr_data;
	ano_matching_tim1 = pwm_param.pulse_width;
	cat_matching_tim1 = ano_matching_tim1 + pwm_param.dead_time;
	cat_matching_tim2 = (ano_matching_tim1 * 2) + pwm_param.dead_time;

	arr_data = MASTER_ARR / pwm_param.pulse_freq;
	TIM2->CNT = 0;
	TIM2->ARR = arr_data - 1;
	TIM2->CCR1 = ano_matching_tim1;
	pwm_arr[0] = cat_matching_tim2;
	pwm_arr[1] = cat_matching_tim1;
}

void Echo_Pulse_V_PW_Config()
{
	TIM1->CCR1 = v_step_val;
}

/*
 * FACTORY RESET
 * */
void Echo_Factory_Reset()
{
	pwm_param.dead_time = 10;
	pwm_param.pulse_width = 1000;
	pwm_param.pulse_freq = 1;
	v_step_tv = 5000;
	Echo_Flash_Write();
}
/****************************************/

/*
 * Stimulation Start AND Stop
 */
void Echo_Stim_Stop()
{
	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
	HAL_TIM_OC_Stop_DMA(&htim2, TIM_CHANNEL_2);
	Echo_StepUP_Stop();
}

void Echo_Stim_Start()
{
	Echo_Pulse_Prm_Config();

	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	HAL_TIM_OC_Start_DMA(&htim2, TIM_CHANNEL_2, (uint32_t*) pwm_arr, 2);
	HAL_TIM_OC_Start_DMA(&htim2, TIM_CHANNEL_4, (uint32_t*) pwm_arr, 2);
	__HAL_DMA_DISABLE_IT(&hdma_tim2_ch2_ch4, (DMA_IT_TC | DMA_IT_HT)); // HAL_DMA_Start_IT

	Echo_StepUP_Start();
}

#if 0
void Echo_Pulse_Prm_Config()
{
	ano_matching_tim1 = pwm_param.pulse_width;
	cat_matching_tim1 = ano_matching_tim1 + pwm_param.dead_time;
	cat_matching_tim2 = (ano_matching_tim1 * 2) + pwm_param.dead_time;
	TIM2->CCR1 = ano_matching_tim1;
	if (gPulse_high == false)
	{
		TIM2->CCR2 = cat_matching_tim2;
		//gPulse_high = true;
	}
	else
	{
		TIM2->CCR2 = cat_matching_tim1;
		//gPulse_high = false;
	}
}

void Echo_Stim_Stop()
{
	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
	HAL_TIM_OC_Stop_IT(&htim2, TIM_CHANNEL_2);
	Echo_StepUP_Stop();
}

void Echo_Stim_Start()
{
	ano_matching_tim1 = pwm_param.pulse_width;
	cat_matching_tim1 = ano_matching_tim1 + pwm_param.dead_time;
	cat_matching_tim2 = (ano_matching_tim1 * 2) + pwm_param.dead_time;
	TIM2->CCR1 = ano_matching_tim1;
	TIM2->CCR2 = cat_matching_tim1;

	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	HAL_TIM_OC_Start_IT(&htim2, TIM_CHANNEL_2);

	Echo_StepUP_Start();
}
#endif

/****************************************/

/*
 * Stimulation Voltage Setting
 * */
void Echo_StepUP_Stop()
{
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
	HAL_TIM_Base_Stop_IT(&htim16);
}

void Echo_StepUP_Start()
{
	TIM1->CCR1 = v_step_val;
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_Base_Start_IT(&htim16);
}
/****************************************/

