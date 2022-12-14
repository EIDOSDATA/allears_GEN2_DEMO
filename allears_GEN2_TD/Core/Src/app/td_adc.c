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
#define TD_REF_VOLTAGE_TABLE				ref_voltage_table
#define TD_REF_ADC_VALUE_TABLE				ref_adc_value_table
#define TD_REF_ADC_VOLTAGE_TABLE			ref_adc_voltage_table

#define TD_NOLOAD_VOLTAGE_TABLE				noload_voltage_table
#define TD_NOLOAD_ADC_VALUE_TABLE			noload_adc_value_table
#define TD_NOLOAD_ADC_VOLTAGE_TABLE			noload_adc_voltage_table

/*
 * SYSTEM CLOCK : 80MHz
 * STEPUP PULSE TIMER : 200KHz
 * STEPUP CIRCUIT INPUY VOLTAGE : 3.3V
 * END POINT RESISTANCE : 100K
 * */
#if 0
int ref_voltage_table[TD_VOLTAGE_TABLE_MAX] =
{ 36000, 66000, 88000, 108000, 132000, 152000, 176000, 196000, 216000, 238000,
		260000, 280000, 300000, 320000, 338000, 356000, 372000, 392000, 408000,
		424000, 440000, 454000, 468000, 482000, 494000, 508000, 518000, 526000,
		534000, 542000 }; // 30

int ref_adc_value_table[TD_VOLTAGE_TABLE_MAX] =
{ 282, 340, 384, 428, 477, 540, 585, 630, 676, 720, 763, 803, 847, 888, 928,
		968, 1005, 1042, 1080, 1118, 1150, 1188, 1216, 1247, 1275, 1300, 1322,
		1342, 1357, 1373 }; //30

int ref_adc_voltage_table[TD_VOLTAGE_TABLE_MAX] =
{ 1000, 1800, 2300, 3000, 3700, 4200, 4900, 5600, 6200, 6800, 7400, 8000, 8600,
		9200, 9700, 10350, 10700, 11200, 11900, 12200, 12800, 13300, 13700,
		14100, 14500, 14900, 15100, 15400, 15700, 15900 }; // 30
#endif

/*
 * SYSTEM CLOCK : 20MHz
 * STEPUP PULSE TIMER : 120KHz
 * STEPUP CIRCUIT INPUT VOLTAGE : 3.3V
 * END POINT RESISTANCE : 100K
 * */
/* Voltage Scale : 10 uV >> ADC BUFFER IS 10 */
#if 0
int ref_voltage_table[TD_VOLTAGE_TABLE_MAX] =
{ 54000, 87000, 126200, 165000, 198500, 231000, 262400, 294800, 325800, 354200,
		380500, 403500, 422000, 433500, 439700 }; // 15

int ref_adc_value_table[TD_VOLTAGE_TABLE_MAX] =
{ 227, 370, 465, 580, 673, 765, 847, 933, 1009, 1087, 1162, 1222, 1273, 1310,
		1322 }; // 15

int ref_adc_voltage_table[TD_VOLTAGE_TABLE_MAX] =
{ 1100, 2250, 3250, 4250, 5330, 6310, 7230, 8220, 9120, 9925, 10880, 11340,
		11950, 12470, 12650 }; // 15
#endif

/*
 * SYSTEM CLOCK : 20MHz
 * STEPUP PULSE TIMER : 125KHz
 * STEPUP CIRCUIT INPUT VOLTAGE : 3.3V
 * END POINT RESISTANCE : 100K
 * */
#if 0
int ref_voltage_table[TD_VOLTAGE_TABLE_MAX] =
{ 50750, 90820, 130300, 162500, 197300, 234300, 266900, 297300, 326800, // 0 ~ 8
		355200, 381500, 405500, 426300, 443000, 449800, 463000, 463300, // 9 ~ 16
		461500, 459500, 446500 }; // 17 ~ 19

int ref_adc_value_table[TD_VOLTAGE_TABLE_MAX] =
{ 272, 375, 475, 579, 675, 769, 849, 932, 1015, 1097, 1166, 1232, 1290, 1336, // 0 ~ 13
		1372, 1395, 1399, 1385, 1352, 1315 }; // 14 ~ 19

int ref_adc_voltage_table[TD_VOLTAGE_TABLE_MAX] =
{ 1301, 2360, 3335, 4499, 5435, 7329, 8448, 9397, 10130, 11100, 11700, 12210,
		12720, 12720, 13290, 13550, 13620, 13570, 13360, 13020 }; // 19
#endif

/*
 * SYSTEM CLOCK : 20MHz
 * STEPUP PULSE TIMER : 8.0KHz
 * STEPUP CIRCUIT INPUY VOLTAGE : 5.0V
 * END POINT RESISTANCE : 100K
 * */
#if 1
int ref_voltage_table[TD_VOLTAGE_TABLE_MAX] =
{ 47580, 69860, 85440, 99610, 114300, 129700, 144600, 159500, 173800, 187800,
		201500, 215600, 229000, 241500, 253800, 265700, 278600, 291100, 302300,
		314100, 325100, 336500, 347600, 357700, 367900, 377500, 387200, 395200,
		401600, 406700, 407600 };

int ref_adc_value_table[TD_VOLTAGE_TABLE_MAX] =
{ 254, 368, 443, 559, 638, 713, 783, 859, 931, 1006, 1082, 1152, 1222, 1288,
		1358, 1422, 1489, 1544, 1606, 1666, 1727, 1786, 1837, 1894, 1947, 2346,
		2398, 2442, 2479, 2507, 2512, };

int ref_adc_voltage_table[TD_VOLTAGE_TABLE_MAX] =
{ 757, 1213, 1486, 1896, 2334, 2627, 2937, 3219, 3533, 3813, 4123, 4417, 4693,
		4968, 5219, 5462, 5702, 5972, 6184, 6441, 6682, 6932, 7154, 7365, 7602,
		7794, 9927, 10120, 10250, 10360, 10360 };

/*
 * SYSTEM CLOCK : 20MHz
 * STEPUP PULSE TIMER : 8.0KHz
 * STEPUP CIRCUIT INPUY VOLTAGE : 5.0V
 * END POINT RESISTANCE : 0K
 * */
int noload_voltage_table[TD_NOLOAD_TABLE_MAX] =
{ 47620, 117800, 153500, 189800, 223800, 257600, 289600, 322200, 353500, 385400,
		415700 };

int noload_adc_value_table[TD_NOLOAD_TABLE_MAX] =
{ 253, 655, 832, 1014, 1199, 1375, 1540, 1712, 1874, 2388, 2550 };

int noload_adc_voltage_table[TD_NOLOAD_TABLE_MAX] =
{ 755, 2398, 3101, 3844, 4620, 5265, 5964, 6621, 7294, 9898, 10540 };

#endif

/*
 * SYSTEM CLOCK : 20MHz
 * STEPUP PULSE TIMER : 10.0KHz
 * STEPUP CIRCUIT INPUY VOLTAGE : 5.0V
 * END POINT RESISTANCE : 100K
 * */
#if 0
int ref_voltage_table[TD_VOLTAGE_TABLE_MAX] =
{ 49250, 75660, 92790, 109800, 126700, 143500, 159500, 175600, 191500, 207200,
		222000, 236900, 251700, 265700, 279500, 293500, 307500, 321300, 333800,
		346600, 358500, 370900, 383800, 395700, 405900, 417200, 427100, 435800,
		443700, 449500, 452100 };

int ref_adc_value_table[TD_VOLTAGE_TABLE_MAX] =
{ 254, 390, 477, 603, 689, 770, 856, 938, 1025, 1109, 1189, 1269, 1344, 1422,
		1498, 1565, 1635, 1705, 1774, 1837, 1902, 1963, 2377, 2442, 2501, 2583,
		2638, 2688, 2728, 2759, 2773 };

int ref_adc_voltage_table[TD_VOLTAGE_TABLE_MAX] =
{ 409, 990, 1286, 1871, 2190, 2250, 2851, 3144, 3528, 3856, 4238, 4500, 4840,
		5188, 5459, 5767, 6058, 6342, 6644, 6897, 7165, 7422, 9509, 9758, 10020,
		10230, 10510, 10740, 10900, 1104, 1107 };

/*
 * SYSTEM CLOCK : 20MHz
 * STEPUP PULSE TIMER : 10.0KHz
 * STEPUP CIRCUIT INPUY VOLTAGE : 5.0V
 * END POINT RESISTANCE : 0K
 * */
int noload_voltage_table[TD_NOLOAD_TABLE_MAX] =
{ 49700, 129700, 168800, 207700, 252700, 281500, 318000, 353600, 389800, 423600,
		457400, };

int noload_adc_value_table[TD_NOLOAD_TABLE_MAX] =
{ 256, 706, 903, 1115, 1313, 1509, 1695, 1881, 2413, 2622, 2801 };

int noload_adc_voltage_table[TD_NOLOAD_TABLE_MAX] =
{ 377, 2231, 2998, 3911, 4893, 5545, 6319, 7119, 9601, 10340, 1116 };

#endif

/*
 * SYSTEM CLOCK : 20MHz
 * STEPUP PULSE TIMER : 15.0KHz
 * STEPUP CIRCUIT INPUY VOLTAGE : 5.0V
 * END POINT RESISTANCE : 100K
 * */
#if 0
int ref_voltage_table[TD_VOLTAGE_TABLE_MAX] =
{ 46930, 80060, 100700, 121600, 142100, 161600, 181800, 201500, 220400, 238400,
		255700, 275100, 291300, 307700, 324400, 338500, 355900, 371000, 386000,
		401400, 415800, 430900, 444700, 458800, 470900, 482900, 495200, 499400,
		506000, 512100, 517500 };

int ref_adc_value_table[TD_VOLTAGE_TABLE_MAX] =
{ 262, 432, 573, 678, 778, 881, 982, 1088, 1186, 1281, 1376, 1470, 1554, 1644,
		1731, 1799, 1882, 1961, 2381, 2454, 2534, 2626, 2696, 2768, 2830, 2897,
		2963, 3020, 3058, 3103, 3113 };

int ref_adc_voltage_table[TD_VOLTAGE_TABLE_MAX] =
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

/* ADC END POINT LOAD DETECT FLAG */
uint8_t ex_load_flage = 0;

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
	HAL_GPIO_WritePin(QCC_CRTL0_GPIO_Port, QCC_CRTL0_Pin, GPIO_PIN_SET);
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
	uint32_t adc1_avg_div10 = 0;

	for (int i = 0; i < ADC1_RCV_SIZE; i++)
	{
		adc1_avg += ex_setpup_adc[i];
	}
	adc1_avg_div10 = (uint32_t) (adc1_avg / 10);

#if 1
	if (TD_VOLTAGE_RELATED_PULSE_WIDTH < 11
			&& TD_VOLTAGE_RELATED_PULSE_WIDTH != 0)
	{
		if (TD_NOLOAD_ADC_VALUE_TABLE[TD_VOLTAGE_RELATED_PULSE_WIDTH] - 200
				<= adc1_avg_div10
				&& TD_NOLOAD_ADC_VALUE_TABLE[TD_VOLTAGE_RELATED_PULSE_WIDTH]
						+ 200 >= adc1_avg_div10)
		{
			LOAD_DETECTION = 0;
		}
		else
		{
			LOAD_DETECTION = 1;
		}
	}
	else if (TD_VOLTAGE_RELATED_PULSE_WIDTH == 0)
	{
		LOAD_DETECTION = 0;
	}
	else
	{
		LOAD_DETECTION = 1;
	}
#endif

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
	if (LOAD_DETECTION == 1)
	{
		uint64_t adc_val =
				(TD_REF_ADC_VOLTAGE_TABLE[TD_VOLTAGE_RELATED_PULSE_WIDTH]
						* in_adc_val)
						/ TD_REF_ADC_VALUE_TABLE[TD_VOLTAGE_RELATED_PULSE_WIDTH];
		uint64_t v_out = (TD_REF_VOLTAGE_TABLE[TD_VOLTAGE_RELATED_PULSE_WIDTH]
				* adc_val)
				/ TD_REF_ADC_VOLTAGE_TABLE[TD_VOLTAGE_RELATED_PULSE_WIDTH];
		return v_out;
	}
	else if (LOAD_DETECTION == 0)
	{
		uint64_t adc_val =
				(TD_NOLOAD_ADC_VOLTAGE_TABLE[TD_VOLTAGE_RELATED_PULSE_WIDTH]
						* in_adc_val)
						/ TD_NOLOAD_ADC_VALUE_TABLE[TD_VOLTAGE_RELATED_PULSE_WIDTH];
		uint64_t v_out =
				(TD_NOLOAD_VOLTAGE_TABLE[TD_VOLTAGE_RELATED_PULSE_WIDTH]
						* adc_val)
						/ TD_NOLOAD_ADC_VOLTAGE_TABLE[TD_VOLTAGE_RELATED_PULSE_WIDTH];
		return v_out;
	}
	else
	{
		uint64_t adc_val =
				(TD_REF_ADC_VOLTAGE_TABLE[TD_VOLTAGE_RELATED_PULSE_WIDTH]
						* in_adc_val)
						/ TD_REF_ADC_VALUE_TABLE[TD_VOLTAGE_RELATED_PULSE_WIDTH];
		uint64_t v_out = (TD_REF_VOLTAGE_TABLE[TD_VOLTAGE_RELATED_PULSE_WIDTH]
				* adc_val)
				/ TD_REF_ADC_VOLTAGE_TABLE[TD_VOLTAGE_RELATED_PULSE_WIDTH];
		return v_out;
	}

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
