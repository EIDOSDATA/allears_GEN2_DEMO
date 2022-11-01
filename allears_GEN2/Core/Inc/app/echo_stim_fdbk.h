/*
 * echo_stim_fdbk.h
 *
 *  Created on: Oct 28, 2022
 *      Author: ECHO
 */

#ifndef INC_APP_ECHO_STIM_FDBK_H_
#define INC_APP_ECHO_STIM_FDBK_H_

typedef enum
{
	PWR_BATT_NORMAL = 0x00, PWR_BATT_LOW, PWR_BATT_CUTOFF, PWR_BATT_STATE_MAX
} PWR_BATT_StateTypeDef;

void Power_Init(void);

PWR_BATT_StateTypeDef Get_Power_Batt_State(void);

#define ADC_VDDA										3000			/* 3.0 V */
#define ADC_MAX_VAL										4095			/* ADC is 12bit resolution */
#define ADC_CONV_WAIT_TIME_MAX							1000				/* ms */
#define ADC_CHK_CH_NUM									2
#define PWR_HANDLE_PERIOD								2000		/* ms */

void Echo_ADC1_Enable(void);
void Echo_Start_ADC_Conv(void);
void Echo_Get_ADC_ConvVal(uint16_t *con_val);
void Echo_Stepup_Handle(void);
#endif /* INC_APP_ECHO_STIM_FDBK_H_ */
