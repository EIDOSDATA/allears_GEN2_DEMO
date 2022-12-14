#include <td_btn.h>
#include <td_flash_memory.h>
#include <td_led.h>
#include <td_private.h>
#include <td_schedule.h>
#include <td_shell.h>
#include <td_stim_setting.h>
#include <td_sys_common.h>
#include <td_sys_fsm_state.h>
#include <td_uart.h>
#include "main.h"

#ifdef	TD_SHELL_ENABLED

extern pwm_pulse_param_t ex_pwm_param;
extern uint8_t ex_td_uart1_rcv_byte;
extern uint8_t ex_td_uart2_rcv_byte;

typedef struct
{
	uint8_t pos;
	uint8_t buf[SHELL_MSG_RCV_BUF_SIZE];
	uint32_t flush_chk_time;
	uint32_t handshaking_time;
} td_shell_msg_rcv_state_t;

td_shell_msg_rcv_state_t td_shell_msg_state;

const char *help_manual = "----------   MANUAL   ----------\r\n"
		"********  ADMIN COMMAND  ********\r\n"
		"#start		:	START PWM\r\n"
		"#stop		:	STOP PWM\r\n"
		"#save		:	SAVE PWM PARAMETER\r\n"
		"#factoryreset	:	FACTORY RESET\r\n"
		"#help		:	PRINT HELP MANUAL\r\n"
		"#version	:	PRINT VERSION INFO\r\n\r\n"

		"********  PARAMETER COMMAND  ********\r\n"
		"#setDT		:	Dead Time Setting\r\n"
		"#setPW		:	Pulse Width Setting\r\n"
		"#setHZ		:	Pulse Frequency Setting\r\n"
		"#setVPW		:	Directly sets the voltage-related pulse width\r\n"
		"#setVOL		:	Set the voltage value to output\r\n"
		"#setDAC		:	Adjust the strength of the current.\r\n\r\n"

		"#getDT		:	Get Dead Time\r\n"
		"#getPW		:	Get Pulse Width\r\n"
		"#getHZ		:	Get Pulse Frequency\r\n"
		"#getVPW		:	Outputs the voltage-related pulse width\r\n"
		"#getVOL		:	Get the voltage value to output\r\n"
		"#getDAC		:	Check the strength of the current.\r\n"
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
{ (unsigned char*) "#setVOL", 7 },
{ (unsigned char*) "#setDAC", 7 },

{ (unsigned char*) "#getDT", 6 },
{ (unsigned char*) "#getPW", 6 },
{ (unsigned char*) "#getHZ", 6 },
{ (unsigned char*) "#getVPW", 7 },
{ (unsigned char*) "#getVOL", 7 },
{ (unsigned char*) "#getDAC", 7 },
{ (unsigned char*) "#getALLPRM", 10 } };

void td_Shell_RxPoll(void)
{
	static uint8_t st_rcv_byte;

	while (!(TD_SHELL_UART->ISR & UART_FLAG_RXNE))
		;

	st_rcv_byte = TD_SHELL_UART->RDR;

	while (!(TD_SHELL_UART->ISR & UART_FLAG_TXE))
		;

	TD_SHELL_UART->TDR = st_rcv_byte;
}

void td_ShellPrint_Char(uint8_t *data, uint16_t len)
{
	uint16_t i;

	if (len == 0)
		return;

	for (i = 0; i < len; i++)
	{
		TD_SHELL_PRINT(("%c", data[i]));
	}
	TD_SHELL_PRINT(("\n"));
}

void td_ShellPrint_Hex(uint8_t *data, uint16_t len)
{
	uint16_t i;

	if (len == 0)
		return;

	for (i = 0; i < len; i++)
	{
		TD_SHELL_PRINT(("%02x ", data[i]));
	}
	TD_SHELL_PRINT(("\n"));
}

/*
 * PIRNT MANUAL
 */
void td_Print_Manual()
{
	TD_SHELL_PRINT(("%s\n",help_manual));
}
/**********************/

/*
 * PIRNT VERSION
 */
void td_Print_Version()
{
	char res_msg[60] =
	{ '\0', };

	sprintf(res_msg, "VERSION INFO: %s\r\n"
			"Build Date: %s\r\n", TD_FW_VER, __DATE__);
	TD_SHELL_PRINT(("%s\n",res_msg));
}
/**********************/

/*
 *  INPUT DATA PRINT
 */
void td_Shell_Input_Print()
{
	if (ex_td_uart2_rcv_byte != 0)
	{
		TD_USART2_MUTEX_LOCK;

		HAL_UART_Transmit(&TD_USART2_HANDLE, &ex_td_uart2_rcv_byte, 1, 10);

		ex_td_uart2_rcv_byte = 0;
		TD_USART2_MUTEX_UNLOCK;
	}

	if (ex_td_uart1_rcv_byte != 0)
	{
		TD_USART1_MUTEX_LOCK;

		HAL_UART_Transmit(&TD_USART1_HANDLE, &ex_td_uart1_rcv_byte, 1, 10);

		ex_td_uart1_rcv_byte = 0;
		TD_USART1_MUTEX_UNLOCK;
	}
}
/**********************/

void td_Shell_Init(void)
{
	memset(SHELL_MSG_RCV_BUF, '\0', SHELL_MSG_RCV_BUF_SIZE);
	SHELL_MSG_RCV_POS = 0;
}

/*
 *  SHELL COMMAND EXE
 */
void td_Shell_CMD_Handle()
{
	static uint8_t st_byte;
	while ((td_Uart1_Get_RCV_Q(&st_byte) == true
			|| td_Uart2_Get_RCV_Q(&st_byte) == true)
			&& SHELL_MSG_RCV_POS < SHELL_MSG_RCV_BUF_SIZE)
	{
		SHELL_MSG_RCV_FLUSH_CHK_TIME = HAL_GetTick();
		if ((st_byte == '#') || SHELL_MSG_RCV_POS != 0)
		{
			SHELL_MSG_RCV_BUF[SHELL_MSG_RCV_POS] = st_byte;
			SHELL_MSG_RCV_POS++;

			/* ENTER KEY CHECK */
			if (st_byte == 13)
			{
				if (strncmp((const char*) SHELL_MSG_RCV_BUF,
						(const char*) "#set", 4) == 0
						|| strncmp((const char*) SHELL_MSG_RCV_BUF,
								(const char*) "#get", 4) == 0)
				{
					td_ParameterCMD_Check(SHELL_MSG_RCV_BUF,
					SHELL_MSG_RCV_POS);
				}
				else
				{
					td_AdminCMD_Check(SHELL_MSG_RCV_BUF, SHELL_MSG_RCV_POS);
				}
				td_Shell_Init();
				SHELL_MSG_RCV_HANDSHAKING_TIME = HAL_GetTick();
			}
		}
	}

	if (SHELL_MSG_RCV_POS > 0)
	{
		/* Check flush timeout */
		if (HAL_GetTick() - SHELL_MSG_RCV_FLUSH_CHK_TIME
				> TD_SHELL_MSG_RCV_FLUSH_TIMEOUT)
		{
#if 0
			TD_SHELL_PRINT(("BT_MSG: Flush timeout\n"));
			TD_SHELL_PRINT_CHAR(SHELL_MSG_RCV_BUF, SHELL_MSG_RCV_POS);
#endif
			td_Shell_Init();
		}
	}
	if (td_Get_Sys_FSM_State() == td_sys_state_run)
	{
		/* Check handshaking timeout */
		if (HAL_GetTick() - SHELL_MSG_RCV_HANDSHAKING_TIME
				> TD_SHELL_MSG_RCV_HANDSHAKING_TIMEOUT)
		{
#if 0
			TD_SHELL_PRINT(("SHELL MSG: Handshaking timeout\n"));
			aulStimul_forceStop();
#endif
			/* Reset time */
			SHELL_MSG_RCV_HANDSHAKING_TIME = HAL_GetTick();
		}
	}
}
/**********************/

/*
 * CMD EXE >> ADMIN COMMAND CHECK
 */
void td_AdminCMD_Check(uint8_t *data, uint16_t len)
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
	case start:
		td_Set_Sys_FSM_State_Start();
		break;

	case stop:
		td_Set_Sys_FSM_State_Stop();
		break;

	case save:
		td_Flash_Write();
		break;

	case factoryreset:
		td_Factory_Reset();
		break;

	case help:
		td_Print_Manual();
		break;

	case version:
		td_Print_Version();
		break;

	default:
		break;
	}
}
/**********************/

/*
 * CMD EXE >> PARAMETER COMMAND CHECK
 */
void td_ParameterCMD_Check(uint8_t *data, uint16_t len)
{
	int param_cmd_cnt = 0;

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
	case set_stim_deadtime:
		td_Set_DT(data, len);
		break;

	case set_stim_pulse_width:
		td_Set_PW(data, len);
		break;

	case set_stim_frequency:
		td_Set_Sys_FSM_State_Stop();
		td_Set_HZ(data, len);
		break;

	case set_voltage_pulse_width:
		td_Set_V_PW(data, len);
		break;

	case set_target_voltage_value:
		td_Set_Voltage_Output(data, len);
		break;

	case set_current_strength:
		td_Set_Current_Strength(data, len);
		break;

	case get_stim_deadtime:
	case get_stim_pulse_width:
	case get_stim_frequency:
	case get_voltage_pulse_width:
	case get_target_voltage_value:
	case get_current_strength:
	case get_allprm:
		td_Get_Res_Data(param_cmd_cnt - get_stim_deadtime);
		break;

	default:
		break;
	}
}

#endif	/*  AUL_DEBUG_ENABLED */

