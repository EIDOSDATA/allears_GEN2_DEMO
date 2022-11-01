#include <echo_stim_setting.h>
#include "main.h"
#include "echo_btn.h"
#include "echo_flash_memory.h"
#include "echo_led.h"
#include "echo_private.h"
#include "echo_schedule.h"
#include "echo_shell.h"
#include "echo_state.h"
#include "echo_sys_common.h"

#ifdef	ECHO_SHELL_ENABLED

extern pwm_pulse_param_t pwm_param;

uint8_t uart1_input_data = 0;
uint8_t uart2_input_data = 0;
uint8_t uart1_buff[256];
uint8_t uart2_buff[256];
int uart1_buff_pos = 0;
int uart2_buff_pos = 0;

const char *help_manual = "\r\n----------   MANUAL   ----------\r\n"
		"\r\n********  ADMIN COMMAND  ********\r\n"
		"#start		:	START PWM\r\n"
		"#stop		:	STOP PWM\r\n"
		"#save		:	SAVE PWM PARAMETER\r\n"
		"#factoryreset	:	FACTORY RESET\r\n"
		"#help		:	PRINT HELP MANUAL\r\n"
		"#version	:	PRINT VERSION INFO\r\n\r\n"

		"\r\n********  PARAMETER COMMAND  ********\r\n"
		"#setDT		:	Dead Time Setting\r\n"
		"#setPW		:	Pulse Width Setting\r\n"
		"#setHZ		:	Pulse Frequency Setting\r\n"
		"#setVPW		:	Pulse Voltage PW Setting\r\n"

		"#getDT		:	Get Dead Time\r\n"
		"#getPW		:	Get Pulse Width\r\n"
		"#getHZ		:	Get Pulse Frequency\r\n"
		"#GetVPW		:	Get Voltage PW\r\n"
		"#getALLPRM	:	Get Period\r\n\r\n";

/* ADMIN COMMAND */
typedef struct
{
	uint8_t *str;
	uint8_t len;
} admin_cmd_str_t;

const admin_cmd_str_t admin_cmd_str_table[admin_cmd_max] =
{
{ (unsigned char*) "#start", 6 },
{ (unsigned char*) "#stop", 5 },
{ (unsigned char*) "#save", 5 },
{ (unsigned char*) "#factoryreset", 13 },
{ (unsigned char*) "#help", 5 },
{ (unsigned char*) "#version", 8 } };

/* PARAMETER COMMAND */
typedef struct
{
	uint8_t *str;
	uint8_t len;
} parameter_cmd_str_t;

const parameter_cmd_str_t parameter_cmd_str_table[parameter_cmd_max] =
{
{ (unsigned char*) "#setDT", 6 },
{ (unsigned char*) "#setPW", 6 },
{ (unsigned char*) "#setHZ", 6 },
{ (unsigned char*) "#setVPW", 7 },

{ (unsigned char*) "#getDT", 6 },
{ (unsigned char*) "#getPW", 6 },
{ (unsigned char*) "#getHZ", 6 },
{ (unsigned char*) "#getVPW", 7 },
{ (unsigned char*) "#getALLPRM", 10 } };

void Echo_Shell_Init(void)
{
	ECHO_H_SHELL.Instance = USART2;
	ECHO_H_SHELL.Init.BaudRate = 115200;
	ECHO_H_SHELL.Init.WordLength = UART_WORDLENGTH_8B;
	ECHO_H_SHELL.Init.StopBits = UART_STOPBITS_1;
	ECHO_H_SHELL.Init.Parity = UART_PARITY_NONE;
	ECHO_H_SHELL.Init.Mode = UART_MODE_TX_RX;
	ECHO_H_SHELL.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	ECHO_H_SHELL.Init.OverSampling = UART_OVERSAMPLING_16;
	ECHO_H_SHELL.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	ECHO_H_SHELL.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart2) != HAL_OK)
	{
		Error_Handler();
	}
}

void Echo_Shell_DeInit(void)
{
	HAL_UART_AbortReceive_IT(&ECHO_H_SHELL);
	HAL_UART_DeInit(&ECHO_H_SHELL);
}

void Echo_Shell_RxPoll(void)
{
	static uint8_t st_rcv_byte;

	while (!(ECHO_SHELL_UART->ISR & UART_FLAG_RXNE))
		;

	st_rcv_byte = ECHO_SHELL_UART->RDR;

	while (!(ECHO_SHELL_UART->ISR & UART_FLAG_TXE))
		;

	ECHO_SHELL_UART->TDR = st_rcv_byte;
}

void Echo_ShellPrint_Char(uint8_t *data, uint16_t len)
{
	uint16_t i;

	if (len == 0)
		return;

	for (i = 0; i < len; i++)
	{
		ECHO_SHELL_PRINT(("%c", data[i]));
	}

	ECHO_SHELL_PRINT(("\n"));
}

void Echo_ShellPrint_Hex(uint8_t *data, uint16_t len)
{
	uint16_t i;

	if (len == 0)
		return;

	for (i = 0; i < len; i++)
	{
		ECHO_SHELL_PRINT(("%02x ", data[i]));
	}

	ECHO_SHELL_PRINT(("\n"));
}

/*
 * PIRNT MANUAL
 */
void Echo_Print_Manual()
{
	ECHO_SHELL_PRINT(("%s\n",help_manual));
}
/**********************/

/*
 * PIRNT VERSION
 */
void Echo_Print_Version()
{
	char res_msg[40] =
	{ '\0', };

	sprintf(res_msg, "\r\nVERSION INFO: %s \r\n", ECHO_FW_VER);
	ECHO_SHELL_PRINT(("%s\n",res_msg));
}
/**********************/

/*
 * 1. UART RX Interrupt
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
	}
	else if (huart->Instance == USART2)
	{
		Echo_ShellCMD_Read();
	}
	HAL_UART_Receive_IT(&ECHO_H_SHELL, &uart2_input_data, 1);
}
/**********************/

/*
 *  2. DATA >> BUFFER
 */
void Echo_ShellCMD_Read()
{
	/* NULL */
	if (uart2_buff_pos == 0 && uart2_input_data == '\n')
	{
		uart2_buff_pos = 0;
		uart2_input_data = '\0';
	}
	else
	{
		uart2_buff[uart2_buff_pos] = uart2_input_data;
		uart2_buff_pos++;
	}
}
/**********************/

/*
 *  INPUT DATA PRINT
 */
void Echo_Shell_Input_Print()
{
	if (uart2_input_data != 0)
	{
		HAL_UART_Transmit(&ECHO_H_SHELL, &uart2_input_data, 1, 10);
		uart2_input_data = 0;
	}
}
/**********************/

/*
 *  SHELL COMMAND EXE
 */
void Echo_Shell_CMD_EXE()
{
	/* ENTER KEY CHECK */
	if (uart2_buff[uart2_buff_pos - 1] == 13)
	{
		if (strncmp((const char*) uart2_buff, (const char*) "#set", 4) == 0
				|| strncmp((const char*) uart2_buff, (const char*) "#get", 4)
						== 0)
		{
			Echo_ParameterCMD_Check(uart2_buff, uart2_buff_pos);
		}
		else
		{
			Echo_AdminCMD_Check(uart2_buff, uart2_buff_pos);
		}
		memset(uart2_buff, '\0', uart2_buff_pos);
		uart2_buff_pos = 0;
	}
}
/**********************/

/*
 * CMD EXE >> ADMIN COMMAND CHECK
 */
void Echo_AdminCMD_Check(uint8_t *data, uint16_t len)
{
	uint8_t admin_cmd_cnt;

	for (admin_cmd_cnt = 0; admin_cmd_cnt < admin_cmd_max; admin_cmd_cnt++)
	{
		if (strncmp((const char*) data,
				(const char*) admin_cmd_str_table[admin_cmd_cnt].str,
				admin_cmd_str_table[admin_cmd_cnt].len) == 0)
		{
			break;
		}
	}
	switch (admin_cmd_cnt)
	{
	case START:
		Echo_Set_FSM_State_Start();
		break;

	case STOP:
		Echo_Set_FSM_State_Stop();
		break;

	case SAVE:
		Echo_Flash_Write();
		break;

	case FACTORYRESET:
		Echo_Factory_Reset();
		break;

	case HELP:
		Echo_Print_Manual();
		break;

	case VERSION:
		Echo_Print_Version();
		break;

	default:
		break;
	}
}
/**********************/

/*
 * CMD EXE >> PARAMETER COMMAND CHECK
 */
void Echo_ParameterCMD_Check(uint8_t *data, uint16_t len)
{
	uint8_t param_cmd_cnt = 0;

	for (param_cmd_cnt = 0; param_cmd_cnt < parameter_cmd_max; param_cmd_cnt++)
	{
		if (strncmp((const char*) data,
				(const char*) parameter_cmd_str_table[param_cmd_cnt].str,
				parameter_cmd_str_table[param_cmd_cnt].len) == 0)
		{
			break;
		}
	}

	switch (param_cmd_cnt)
	{
	case SET_DEADTIME:
		Echo_Set_DT(data, len);
		break;

	case SET_PULSEWIDTH:
		Echo_Set_PW(data, len);
		break;

	case SET_PERIOD:
		Echo_Set_FSM_State_Stop();
		Echo_Set_HZ(data, len);
		break;

	case SET_VOLTAGE_PW:
		Echo_Set_V_PW(data, len);
		break;

	case GET_DEADTIME:
	case GET_PULSEWIDTH:
	case GET_FREQUENCY:
	case GET_VOLTAGE_PW:
	case GET_ALLPRM:
		Echo_Get_Res_Data(param_cmd_cnt - 4);
		break;

	default:
		break;
	}
}

/* IN main.c UART RX Interrupt START Command */
void Echo_Shell_Start()
{
	HAL_UART_Receive_IT(&huart2, &uart2_input_data, 1);
}

#endif	/*  AUL_DEBUG_ENABLED */

