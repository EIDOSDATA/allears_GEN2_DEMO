/*
 * echo_stim_fdbk.h
 *
 *  Created on: Oct 28, 2022
 *      Author: ECHO
 */

#ifndef INC_APP_ECHO_ADC_H_
#define INC_APP_ECHO_ADC_H_

typedef enum
{
	ECHO_ADC1_STATE_INIT = 0x00,
	ECHO_ADC1_IDLE,
	ECHO_ADC1_RUN,
	ECHO_ADC1_CONV_OK,
	ECHO_ADC1_PRINT_OK,
	ECHO_ADC1_ERROR,
	echo_adc1_state_max
} echo_adc1_state_t;

typedef enum
{
	ECHO_ADC2_STATE_INIT = 0x00,
	ECHO_ADC2_IDLE,
	ECHO_ADC2_RUN,
	ECHO_ADC2_CONV_OK,
	ECHO_ADC2_PRINT_OK,
	ECHO_ADC2_ERROR,
	echo_adc2_state_max
} echo_adc2_state_t;

#define ADC_VDDA									3000			/* 3.0 V */
#define ADC_MAX_VAL									4096			/* ADC is 12bit resolution */
#define ADC_CONV_WAIT_TIME_MAX						10				/* ms */
#define PWR_HANDLE_PERIOD							1				/* ms */

#define R1_Vstup		3600.f //680.f;
#define R2_Vstup		110.f //22.f;
#define R1_Ved			18.f
#define R2_Ved			1.f
#define TARGET_VOLTAGE_ADC_VAL						40000

#define ADC1_CHK_CH_NUM								1
#define ADC2_CHK_CH_NUM								1

#define ECHO_ADC1_RCV_SIZE							10
#define ECHO_ADC2_RCV_SIZE							10

#define ADC1_CONV_BUF								get_adc1_buf
#define ADC2_CONV_BUF								get_adc2_buf

#define ADC1_CONV_OK								adc1_conv_ok_flag
#define ADC2_CONV_OK								adc2_conv_ok_flag

void Echo_ADC1_Enable(void);
void Echo_ADC2_Enable(void);
void Echo_ADC_State_Init(void);

void Echo_Start_ADC1_Conv(void);
void Echo_Start_ADC2_Conv(void);

void Echo_Stop_ADC1_Conv(void);
void Echo_Stop_ADC2_Conv(void);

float Echo_Stepup_ADC1_AVG(void);
float Echo_Peak_Detection_ADC2_AVG(void);

float Echo_ADC_Calc_Stepup_V(uint16_t in_adc_val, float r1, float r2);
float Echo_ADC_Calc_Peak_V(uint16_t in_adc_val);

void Echo_ADC_Handle(void);

echo_adc1_state_t Echo_Get_ADC1_State(void);
echo_adc2_state_t Echo_Get_ADC2_State(void);
void Echo_Set_ADC1_State(echo_adc1_state_t state);
void Echo_Set_ADC2_State(echo_adc2_state_t state);
#endif /* INC_APP_ECHO_ADC_H_ */
