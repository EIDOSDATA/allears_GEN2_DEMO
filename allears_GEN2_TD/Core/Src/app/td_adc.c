/*
 * td_stepup_setting.c
 *
 *  Created on: Oct 28, 2022
 *      Author: ECHO
 */
#include <stdint.h>
#include <math.h>
#include <td_adc.h>
#include <td_shell.h>
#include <td_stim_setting.h>
#include <td_sys_fsm_state.h>
#include "main.h"

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern DMA_HandleTypeDef hdma_adc1;
extern DMA_HandleTypeDef hdma_adc2;

/* STEPUP VOLTAGE TABLE */
extern __IO int ex_voltage_r_pw;

/* REFERENCE TABLE */
#define TD_REF_VOLTAGE_TABLE				ref_voltage_table_20mhz
#define TD_REF_ADC_VALUE_TABLE				ref_adc_value_table_20mhz
#define TD_REF_ADC_VOLTAGE_TABLE			ref_adc_voltage_table_20mhz

/*
 * SYSTEM CLOCK : 80MHz
 * STEPUP PULSE TIMER : 200KHz
 * STEPUP CIRCUIT INPUY VOLTAGE : 3.3V
 * */
#if 0
/* Voltage Scale : 10 uV >> ADC BUFFER IS 10 */
int ref_voltage_table_80mhz[TD_VOLTAGE_PW_MAX_VALUE] =
{ 36000, 66000, 88000, 108000, 132000, 152000, 176000, 196000, 216000, 238000,
		260000, 280000, 300000, 320000, 338000, 356000, 372000, 392000, 408000,
		424000, 440000, 454000, 468000, 482000, 494000, 508000, 518000, 526000,
		534000, 542000 }; // 30

int ref_adc_value_table_80mhz[TD_VOLTAGE_PW_MAX_VALUE] =
{ 282, 340, 384, 428, 477, 540, 585, 630, 676, 720, 763, 803, 847, 888, 928,
		968, 1005, 1042, 1080, 1118, 1150, 1188, 1216, 1247, 1275, 1300, 1322,
		1342, 1357, 1373 }; //30

/* Voltage Scale : 10 uV >> ADC BUFFER IS 10 */
int ref_adc_voltage_table_80mhz[TD_VOLTAGE_PW_MAX_VALUE] =
{ 1000, 1800, 2300, 3000, 3700, 4200, 4900, 5600, 6200, 6800, 7400, 8000, 8600,
		9200, 9700, 10350, 10700, 11200, 11900, 12200, 12800, 13300, 13700,
		14100, 14500, 14900, 15100, 15400, 15700, 15900 }; // 30
#endif

/*
 * SYSTEM CLOCK : 20MHz
 * STEPUP PULSE TIMER : 120KHz
 * STEPUP CIRCUIT INPUT VOLTAGE : 3.3V
 * */
/* Voltage Scale : 10 uV >> ADC BUFFER IS 10 */
#if 0
int ref_voltage_table_20mhz[TD_VOLTAGE_PW_MAX_VALUE] =
{ 54000, 87000, 126200, 165000, 198500, 231000, 262400, 294800, 325800, 354200,
		380500, 403500, 422000, 433500, 439700 }; // 15

int ref_adc_value_table_20mhz[TD_VOLTAGE_PW_MAX_VALUE] =
{ 227, 370, 465, 580, 673, 765, 847, 933, 1009, 1087, 1162, 1222, 1273, 1310,
		1322 }; // 15

/* Voltage Scale : 10 uV >> ADC BUFFER IS 10 */
int ref_adc_voltage_table_20mhz[TD_VOLTAGE_PW_MAX_VALUE] =
{ 1100, 2250, 3250, 4250, 5330, 6310, 7230, 8220, 9120, 9925, 10880, 11340,
		11950, 12470, 12650 }; // 15
#endif

/*
 * SYSTEM CLOCK : 20MHz
 * STEPUP PULSE TIMER : 125KHz
 * STEPUP CIRCUIT INPUT VOLTAGE : 3.3V
 * */
#if 0
int ref_voltage_table_20mhz[TD_VOLTAGE_PW_MAX_VALUE] =
{ 50750, 90820, 130300, 162500, 197300, 234300, 266900, 297300, 326800, // 0 ~ 8
		355200, 381500, 405500, 426300, 443000, 449800, 463000, 463300, // 9 ~ 16
		461500, 459500, 446500 }; // 17 ~ 19

int ref_adc_value_table_20mhz[TD_VOLTAGE_PW_MAX_VALUE] =
{ 272, 375, 475, 579, 675, 769, 849, 932, 1015, 1097, 1166, 1232, 1290, 1336, // 0 ~ 13
		1372, 1395, 1399, 1385, 1352, 1315 }; // 14 ~ 19

/* Voltage Scale : 10 uV >> ADC BUFFER IS 10 */
int ref_adc_voltage_table_20mhz[TD_VOLTAGE_PW_MAX_VALUE] =
{ 1301, 2360, 3335, 4499, 5435, 7329, 8448, 9397, 10130, 11100, 11700, 12210,
		12720, 12720, 13290, 13550, 13620, 13570, 13360, 13020 }; // 19
#endif

/*
 * SYSTEM CLOCK : 20MHz
 * STEPUP PULSE TIMER : 10.0KHz
 * STEPUP CIRCUIT INPUY VOLTAGE : 5.0V
 * */
#if 1
int ref_voltage_table_20mhz[TD_VOLTAGE_PW_MAX_VALUE] =
{ 55400, 74420, 91730, 107600, 125700, 142200, 159100, 175800, 191400, 207700,
		222100, 237300, 251100, 267100, 281600, 294800, 309400, 323300, 336000,
		350400, 363100, 375300, 386800, 399800, 409300, 420300, 430200, 438400,
		447200, 453100, 456000 };

int ref_adc_value_table_20mhz[TD_VOLTAGE_PW_MAX_VALUE] =
{ 262, 393, 491, 601, 687, 771, 853, 936, 1023, 1107, 1187, 1266, 1342, 1418,
		1494, 1559, 1633, 1703, 1772, 1833, 1908, 1974, 2367, 2432, 2486, 2561,
		2615, 2663, 2708, 2738, 2750 };

/* Voltage Scale : 100 uV >> ADC BUFFER IS 10 */
int ref_adc_voltage_table_20mhz[TD_VOLTAGE_PW_MAX_VALUE] =
{ 1761, 2325, 2900, 3309, 3791, 4320, 4940, 5312, 5730, 6250, 6764, 7153, 7714,
		8090, 8461, 8871, 9273, 9652, 9991, 10400, 10600, 11040, 11380, 11730,
		12130, 12480, 12750, 12960, 13310, 13390, 13480 };
#endif

/*
 * SYSTEM CLOCK : 20MHz
 * STEPUP PULSE TIMER : 15.0KHz
 * STEPUP CIRCUIT INPUY VOLTAGE : 5.0V
 * */
#if 0
int ref_voltage_table_20mhz[TD_VOLTAGE_PW_MAX_VALUE] =
{ 46930, 80060, 100700, 121600, 142100, 161600, 181800, 201500, 220400, 238400,
		255700, 275100, 291300, 307700, 324400, 338500, 355900, 371000, 386000,
		401400, 415800, 430900, 444700, 458800, 470900, 482900, 495200, 499400,
		506000, 512100, 517500 };

int ref_adc_value_table_20mhz[TD_VOLTAGE_PW_MAX_VALUE] =
{ 262, 432, 573, 678, 778, 881, 982, 1088, 1186, 1281, 1376, 1470, 1554, 1644,
		1731, 1799, 1882, 1961, 2381, 2454, 2534, 2626, 2696, 2768, 2830, 2897,
		2963, 3020, 3058, 3103, 3113 };

/* Voltage Scale : 10 uV >> ADC BUFFER IS 10 */
int ref_adc_voltage_table_20mhz[TD_VOLTAGE_PW_MAX_VALUE] =
{ 1424, 2412, 2991, 3655, 4128, 4928, 5427, 6078, 6526, 7152, 7710, 8102, 8573,
		9143, 9612, 10010, 10510, 10930, 11340, 11810, 12310, 12730, 13270,
		13550, 14020, 14270, 14580, 14920, 15130, 15360, 15390 };
#endif
/*********** END OF SYSTEM CLOCK : 20MHZ ***********/

/* ADC BUFFER */
uint16_t ex_get_adc1_buf[ADC1_CHK_CH_NUM * ADC1_RCV_SIZE];
uint16_t ex_get_adc2_buf[ADC2_CHK_CH_NUM * ADC2_RCV_SIZE];

uint16_t ex_setpup_adc[ADC1_RCV_SIZE]; // ADC1
uint16_t ex_peak_adc_r[ADC2_RCV_SIZE]; // ADC2
uint16_t ex_peak_adc_l[ADC2_RCV_SIZE]; // ADC2

/* ADC1 STATE */
typedef struct
{
	td_adc1_state_t state;
} td_adc1_state_data_t;

td_adc1_state_data_t td_adc1_fsm_state;
td_adc1_state_t ex_adc1_cur_state = td_adc1_idle;

/* ADC2 STATE */
typedef struct
{
	td_adc2_state_t state;
} td_adc2_state_data_t;

td_adc2_state_data_t td_adc2_fsm_state;
td_adc2_state_t ex_adc2_cur_state = td_adc2_idle;

void HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef *hadc)
{
	/* Set variable to report analog watchdog out of window status to main      */
	/* program.                                                                 */
	//HAL_GPIO_WritePin(QCC_CRTL0_GPIO_Port, QCC_CRTL0_Pin, GPIO_PIN_SET);
	td_Stim_Stop();
	td_Set_Sys_FSM_State_Stop();
}

/* ADC CALLBACK FUNCTION */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
#if 0
	if (hadc->Instance == hadc1.Instance)
	{
		for (int index = 0; index < ADC1_RCV_SIZE; index++)
		{
			/* ADC FILTER */
			if (fabs(
			TD_ADC1_CONV_BUF[index] - TD_ADC1_CONV_BUF[(index + 1) % 10]) < 200)
			{
				ex_setpup_adc[index] = TD_ADC1_CONV_BUF[index]; // STEPUP_FEEDBACK
			}
			else
			{
				ex_setpup_adc[index] = TD_ADC1_CONV_BUF[index - 1]; // STEPUP_FEEDBACK
			}
		}
		ex_adc1_cur_state = td_adc1_conv_ok;
	}
#endif

	if (hadc->Instance == hadc2.Instance)
	{
		for (int index = 0; index < ADC2_RCV_SIZE; index++)
		{
			ex_peak_adc_r[index] = TD_ADC2_CONV_BUF[index * 2]; // PEAK_DETECTION
			ex_peak_adc_l[index] = TD_ADC2_CONV_BUF[(index * 2) + 1]; // PEAK_DETECTION
		}
		HAL_ADC_Stop_DMA(&hadc2);
		ex_adc2_cur_state = td_adc2_conv_ok;
	}
}

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
{
	ex_adc1_cur_state = td_adc1_error;
	ex_adc2_cur_state = td_adc2_error;
	TD_SHELL_PRINT(("ADC ERROR CALLBACK!!\n"));
}
/**********************/

/*
 * ADC ENABLE
 * */
void td_ADC1_Enable(void)
{
	if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK)
	{
		Error_Handler();
	}
}

void td_ADC2_Enable(void)
{
	if (HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED) != HAL_OK)
	{
		Error_Handler();
	}
}
/**********************/

/*
 * ADC INIT
 * */
void td_ADC_State_Init(void)
{
	TD_ADC1_CUR_STATE = td_adc1_state_max;
	TD_ADC2_CUR_STATE = td_adc2_state_max;
	td_Set_ADC1_State(td_adc1_state_init);
	td_Set_ADC2_State(td_adc2_state_init);
	td_ADC1_Enable();
	td_ADC2_Enable();
}
/**********************/

/*
 * ADC START CONVERSION
 * */
void td_Start_ADC1_Conv()
{
	if (HAL_ADC_Start_DMA(&hadc1, (uint32_t*) TD_ADC1_CONV_BUF,
	ADC1_CHK_CH_NUM * ADC1_RCV_SIZE) != HAL_OK)
	{
		Error_Handler();
	}
#if 1
	__HAL_DMA_DISABLE_IT(&hdma_adc1, DMA_IT_HT | DMA_IT_TC);
	__HAL_ADC_DISABLE_IT(&hadc1, ADC_IT_OVR);
#endif
}

void td_Start_ADC2_Conv()
{
	if (HAL_ADC_Start_DMA(&hadc2, (uint32_t*) TD_ADC2_CONV_BUF,
	ADC2_CHK_CH_NUM * ADC2_RCV_SIZE) != HAL_OK)
	{
		Error_Handler();
	}
#if 0
	__HAL_DMA_DISABLE_IT(&hdma_adc2, DMA_IT_HT | DMA_IT_TC);
	__HAL_ADC_DISABLE_IT(&hadc2, ADC_IT_OVR);
#endif
}
/**********************/

/*
 * ADC STOP CONVERSION
 * */
void td_Stop_ADC1_Conv()
{
	if (HAL_ADC_Stop_DMA(&hadc1) != HAL_OK)
	{
		Error_Handler();
	}
}

void td_Stop_ADC2_Conv()
{
	if (HAL_ADC_Stop_DMA(&hadc2) != HAL_OK)
	{
		Error_Handler();
	}
}
/**********************/

/*
 * NON FSM & NON ADC Conversion Buffer Read
 * */
void td_Non_Conv_ADC1_Buff_Read()
{
	for (int index = 0; index < ADC1_RCV_SIZE; index++)
	{
		/* ADC FILTER */
		if (fabs(
		TD_ADC1_CONV_BUF[index] - TD_ADC1_CONV_BUF[(index + 1) % 10]) < 200)
		{
			ex_setpup_adc[index] = TD_ADC1_CONV_BUF[index]; // STEPUP_FEEDBACK
		}
		else
		{
			ex_setpup_adc[index] = TD_ADC1_CONV_BUF[index - 1]; // STEPUP_FEEDBACK
		}
	}
}

void td_Non_Conv_ADC2_Buff_Read()
{
	for (int index = 0; index < ADC2_RCV_SIZE; index++)
	{
		ex_peak_adc_r[index] = TD_ADC2_CONV_BUF[index * 2]; // PEAK_DETECTION
		ex_peak_adc_l[index] = TD_ADC2_CONV_BUF[(index * 2) + 1]; // PEAK_DETECTION
	}
}

/**********************/

/*
 * STEPUP ADC AVG CALC
 * */
uint32_t td_Stepup_ADC1_AVG()
{
	uint32_t adc1_avg = 0;

	for (int i = 0; i < ADC1_RCV_SIZE; i++)
	{
		adc1_avg += ex_setpup_adc[i];
	}
	return adc1_avg; //return adc1_avg / ADC1_RCV_SIZE;
}

uint32_t td_Peak_Detection_ADC2_AVG(uint8_t channel)
{
	uint32_t adc2_avg = 0;
	if (channel == ADC2_R_CH)
	{
		for (int i = 0; i < ADC2_RCV_SIZE; i++)
		{
			adc2_avg += ex_peak_adc_r[i];
		}
	}
	else if (channel == ADC2_L_CH)
	{
		for (int i = 0; i < ADC2_RCV_SIZE; i++)
		{
			adc2_avg += ex_peak_adc_l[i];
		}
	}
	return adc2_avg / ADC2_RCV_SIZE;
}
/**********************/

/*
 * VOLTAGE CALC FUNCTION
 * */
uint64_t td_ADC_Calc_Stepup_V(uint32_t in_adc_val, uint32_t r1, uint32_t r2)
{
	/*
	 40v == 1065 ADC

	 1.14v : 1065 == ADC_vol : ADC_val
	 ADC_vol = (1.14v * ADC_val) / 1065
	 --------------
	 40v : 1.2v == x : ADC_vol
	 x = (40 * ADC_val) / 1.14v
	 */

	/* REFERENCE TABLE */
	uint64_t adc_val = (TD_REF_ADC_VOLTAGE_TABLE[TD_VOLTAGE_RELATED_PULSE_WIDTH]
			* in_adc_val)
			/ TD_REF_ADC_VALUE_TABLE[TD_VOLTAGE_RELATED_PULSE_WIDTH];
	uint64_t v_out = (TD_REF_VOLTAGE_TABLE[TD_VOLTAGE_RELATED_PULSE_WIDTH]
			* adc_val)
			/ TD_REF_ADC_VOLTAGE_TABLE[TD_VOLTAGE_RELATED_PULSE_WIDTH];

	return v_out;

#if 0
	float f_adc_val;
	uint32_t vdda = 3300UL;
	float v_ref = (float) (vdda * 0.001);
	float v_out;

	f_adc_val = in_adc_val / 3005.101815202915f;
	v_out = (v_ref * f_adc_val) * ((r1 + r2) / r2);

	return v_out;
#endif
}

uint32_t td_ADC_Calc_Peak_V(uint32_t in_adc_val)
{
	uint32_t vdda = 1800UL;
	uint32_t v_out;

	v_out = (vdda * in_adc_val) / 4095;

	return v_out;
}
/**********************/

/*
 * ADC STEPUP HANDLER
 * */
void td_ADC_Handle(void)
{
	/* FSM ADC */
	if (TD_ADC1_CUR_STATE != ex_adc1_cur_state)
	{
		td_Set_ADC1_State(ex_adc1_cur_state);
	}

	if (TD_ADC2_CUR_STATE != ex_adc2_cur_state)
	{
		td_Set_ADC2_State(ex_adc2_cur_state);
	}

}
/**********************/

td_adc1_state_t td_Get_ADC1_State(void)
{
	return TD_ADC1_CUR_STATE;
}

td_adc2_state_t td_Get_ADC2_State(void)
{
	return TD_ADC2_CUR_STATE;
}

void td_Set_ADC1_State(td_adc1_state_t state)
{
	switch (state)
	{
	case td_adc1_state_init:
		break;
	case td_adc1_idle:
		td_Stop_ADC1_Conv();
		break;
	case td_adc1_run:
		td_ADC1_Enable();
		td_Start_ADC1_Conv();
		break;
	case td_adc1_conv_ok:
		break;
	case td_adc1_print_ok:
		ex_adc1_cur_state = td_adc1_run;
		break;
	case td_adc1_error:
		td_Stop_ADC1_Conv();
		break;
	default:
		break;
	}
	TD_ADC1_CUR_STATE = state;
}

void td_Set_ADC2_State(td_adc2_state_t state)
{
	switch (state)
	{
	case td_adc2_state_init:
		break;
	case td_adc2_idle:
		td_Stop_ADC2_Conv();
		break;
	case td_adc2_run:
		td_ADC2_Enable();
		td_Start_ADC2_Conv();
		break;
	case td_adc2_conv_ok:
		break;
	case td_adc2_print_ok:
		ex_adc2_cur_state = td_adc2_run;
		break;
	case td_adc2_error:
		td_Stop_ADC2_Conv();
		break;
	default:
		break;
	}
	TD_ADC2_CUR_STATE = state;
}
