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
#include "echo_uart2.h"

#ifdef	ECHO_SHELL_ENABLED

extern pwm_pulse_param_t pwm_param;

#define ECHO_SHELL_MSG_RCV_FLUSH_TIMEOUT			2000
#define ECHO_SHELL_MSG_RCV_HANDSHAKING_TIMEOUT		(ECHO_DELAY_1SEC * 10)					/* 10s */

#define SHELL_MSG_RCV_BUF_SIZE					256
#define SHELL_MSG_RCV_POS						ehco_shell_msg_state.pos
#define SHELL_MSG_RCV_BUF						ehco_shell_msg_state.buf
#define SHELL_MSG_RCV_FLUSH_CHK_TIME			ehco_shell_msg_state.flush_chk_time
#define SHELL_MSG_RCV_HANDSHAKING_TIME			ehco_shell_msg_state.handshaking_time

extern uint8_t echo_uart2_rcv_byte;
typedef struct
{
	uint8_t pos;
	uint8_t buf[SHELL_MSG_RCV_BUF_SIZE];
	uint32_t flush_chk_time;
	uint32_t handshaking_time;
} echo_shell_msg_rcv_state_t;

echo_shell_msg_rcv_state_t ehco_shell_msg_state;

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
 *  INPUT DATA PRINT
 */
void Echo_Shell_Input_Print()
{
	if (echo_uart2_rcv_byte != 0)
	{
		HAL_UART_Transmit(&ECHO_USART2_HANDLE, &echo_uart2_rcv_byte, 1, 10);
		echo_uart2_rcv_byte = 0;
	}
}
/**********************/

void Echo_Shell_Init(void)
{
	memset(SHELL_MSG_RCV_BUF, '\0', SHELL_MSG_RCV_BUF_SIZE);
	SHELL_MSG_RCV_POS = 0;
}

/*
 *  SHELL COMMAND EXE
 */
void Echo_Shell_CMD_Handle()
{
	static uint8_t st_byte;
	while (Echo_Uart2_Get_RCV_Q(&st_byte) == true
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
					Echo_ParameterCMD_Check(SHELL_MSG_RCV_BUF,
					SHELL_MSG_RCV_POS);
				}
				else
				{
					Echo_AdminCMD_Check(SHELL_MSG_RCV_BUF, SHELL_MSG_RCV_POS);
				}
				Echo_Shell_Init();
				SHELL_MSG_RCV_HANDSHAKING_TIME = HAL_GetTick();
			}
		}
	}

	if (SHELL_MSG_RCV_POS > 0)
	{
		/* Check flush timeout */
		if (HAL_GetTick() - SHELL_MSG_RCV_FLUSH_CHK_TIME
				> ECHO_SHELL_MSG_RCV_FLUSH_TIMEOUT)
		{
#if 1
			ECHO_SHELL_PRINT(("BT_MSG: Flush timeout\n"));
			ECHO_SHELL_PRINT_CHAR(SHELL_MSG_RCV_BUF, SHELL_MSG_RCV_POS);
#endif
			Echo_Shell_Init();
		}
	}
	if (Echo_Get_FSM_State() == ECHO_STATE_RUN)
	{
		/* Check handshaking timeout */
		if (HAL_GetTick() - SHELL_MSG_RCV_HANDSHAKING_TIME
				> ECHO_SHELL_MSG_RCV_HANDSHAKING_TIMEOUT)
		{
#if 0
			ECHO_SHELL_PRINT(("SHELL MSG: Handshaking timeout\n"));
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

#endif	/*  AUL_DEBUG_ENABLED */

