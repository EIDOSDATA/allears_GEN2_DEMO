/*
 * echo_sys_common.c
 *
 *  Created on: Oct 26, 2022
 *      Author: ECHO
 */
#include <echo_adc.h>
#include <echo_btn.h>
#include <echo_flash_memory.h>
#include <echo_led.h>
#include <echo_private.h>
#include <echo_schedule.h>
#include <echo_shell.h>
#include <echo_stim_setting.h>
#include <echo_sys_common.h>
#include <echo_sys_fsm_state.h>
#include <echo_uart2.h>
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
void Echo_HAL_Init()
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

void Echo_PCI_State_Init()
{
	HAL_Delay(200);
	char res_msg[55] =
	{ '\0', };
	/* FLASH READ */
	if (Echo_Flash_Read() != HAL_OK)
	{
		ECHO_PULSE_DEAD_TIME = 10;
		ECHO_PULSE_WIDTH_TIME = 1000;
		ECHO_PULSE_HZ_FREQ = 10;
		ECHO_VOLTAGE_RELATED_PULSE_WIDTH = 0;
		ECHO_VOLTAGE_VALUE_OUTPUT = 0;
	}

	Echo_Print_Version();
	sprintf((char*) res_msg, (const char*) "Echo_PCI_State_Init()\r\n"
			"SYSTEM CLOCK : %lu Hz\r\n", HAL_RCC_GetHCLKFreq());
	ECHO_SHELL_PRINT(("%s\n", res_msg));

	Echo_ADC_State_Init();
	Echo_Print_Manual();
	Echo_LED_Init();
	Echo_Sys_FSM_State_Init();
	Echo_Shell_Init();
	Echo_Uart2_Interrupt_Enable();
}

void Echo_Clock_Config()
{
	/* NOT YET */
}

/* FAULT OR EXCEPTION HANDLING FUCNTION*/
void Echo_Exception_Handle()
{
#ifdef DEBUG
	while (true)
		;
#else
	HAL_NVIC_SystemReset();
#endif
}
