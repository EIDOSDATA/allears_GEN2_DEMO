/*
 * td_sys_common.c
 *
 *  Created on: Oct 26, 2022
 *      Author: ECHO
 */
#include <td_adc.h>
#include <td_btn.h>
#include <td_flash_memory.h>
#include <td_led.h>
#include <td_private.h>
#include <td_schedule.h>
#include <td_shell.h>
#include <td_stim_setting.h>
#include <td_sys_common.h>
#include <td_sys_fsm_state.h>
#include <td_uart2.h>
#include "main.h"

/*
 * NOT USAGE
 * NO CUSTOM / Only the form remains.
 * */
// SYSTEM
extern void SystemClock_Config();

// INTERFACE HARDWARE
extern void MX_GPIO_Init();
extern void MX_DMA_Init();
extern void MX_USART2_UART_Init();
extern void MX_TIM2_Init();
extern void MX_ADC1_Init();
extern void MX_TIM16_Init();
extern void MX_USART1_UART_Init();
extern void MX_TIM6_Init();
/**********************/

extern pwm_pulse_param_t ex_pwm_param;
extern __IO int ex_voltage_r_pw;
extern __IO int ex_voltage_val_output;

/*
 * NOT USAGE
 * NO CUSTOM / Only the form remains.
 * */
void td_HAL_Init()
{
	SystemInit();

	HAL_Init();
	SystemClock_Config();

	MX_GPIO_Init();
	MX_DMA_Init();
	MX_USART2_UART_Init();
	MX_TIM2_Init();
	MX_ADC1_Init();
	MX_TIM16_Init();
	MX_USART1_UART_Init();
	MX_TIM6_Init();
}
/**********************/

void td_PCI_State_Init()
{
	HAL_Delay(200);
	char res_msg[55] =
	{ '\0', };
	/* FLASH READ */
	if (td_Flash_Read() != HAL_OK)
	{
		TD_PULSE_DEAD_TIME = 200;
		TD_PULSE_WIDTH_TIME = 1000;
		TD_PULSE_HZ_FREQ = 100;
		TD_VOLTAGE_RELATED_PULSE_WIDTH = 0;
		TD_VOLTAGE_VALUE_OUTPUT = 0;
	}

	td_Print_Version();
	sprintf((char*) res_msg, (const char*) "td_PCI_State_Init()\r\n"
			"SYSTEM CLOCK : %lu Hz\r\n", HAL_RCC_GetHCLKFreq());

	TD_SHELL_PRINT(("%s\n", res_msg));
	td_ADC_State_Init();
	td_Print_Manual();
	td_LED_Init();
	td_Sys_FSM_State_Init();
	td_Shell_Init();
	td_Uart2_Interrupt_Enable();
}

void td_Clock_Config()
{
	/* NOT YET */
}

/* FAULT OR EXCEPTION HANDLING FUCNTION*/
void td_Exception_Handle()
{
#ifdef DEBUG
	while (true)
		;
#else
	HAL_NVIC_SystemReset();
#endif
}
