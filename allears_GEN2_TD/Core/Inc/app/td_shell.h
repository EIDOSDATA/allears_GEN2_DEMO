#ifndef INC_APP_TD_SHELL_H_
#define INC_APP_TD_SHELL_H_

#define TD_SHELL_UART											USART2

#ifdef	TD_SHELL_ENABLED
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <td_private.h>

typedef enum
{
	start = 0x00, stop, save, factoryreset, help, version, admin_cmd_max
} admin_command_t;

typedef enum
{
	set_stim_deadtime = 0x00,
	set_stim_pulse_width,
	set_stim_frequency,
	set_voltage_pulse_width,
	set_target_voltage_value,
	set_current_strength,

	get_stim_deadtime,
	get_stim_pulse_width,
	get_stim_frequency,
	get_voltage_pulse_width,
	get_target_voltage_value,
	get_current_strength,
	get_allprm,

	parameter_cmd_max
} parameter_command_t;

typedef enum
{
	res_stim_deadtime = 0x00,
	res_stim_pulse_width,
	res_stim_frequency,
	res_voltage_pulse_width,
	res_target_voltage_value,
	res_current_strength,
	res_allprm,

	res_prm_cmd_max
} response_prm_command_t;

/* Enable debug print as each module in here */
#define TD_SHELL_INIT(void)										td_Shell_Init()
#define TD_SHELL_DEINIT(void)									td_Shell_DeInit()
#define TD_SHELL_PRINT(x)										printf x

#define TD_SHELL_PRINT_CHAR(data, len)							td_ShellPrint_Char(data, len)
#define TD_SHELL_PRINT_HEX(data, len)							td_ShellPrint_Hex(data, len)
#define TD_SHELL_RCV_POLL()										td_Shell_RxPoll()

#define TD_SHELL_MSG_RCV_FLUSH_TIMEOUT				2000
#define TD_SHELL_MSG_RCV_HANDSHAKING_TIMEOUT		(TD_DELAY_1SEC * 10)					/* 10s */

#define SHELL_MSG_RCV_BUF_SIZE						256
#define SHELL_MSG_RCV_POS							td_shell_msg_state.pos
#define SHELL_MSG_RCV_BUF							td_shell_msg_state.buf
#define SHELL_MSG_RCV_FLUSH_CHK_TIME				td_shell_msg_state.flush_chk_time
#define SHELL_MSG_RCV_HANDSHAKING_TIME				td_shell_msg_state.handshaking_time

void td_Shell_RxPoll(void);
void td_ShellPrint_Char(uint8_t *data, uint16_t len);
void td_ShellPrint_Hex(uint8_t *data, uint16_t len);

int td_Shell_printf(const char *format, ...);

void td_Print_Manual(void);
void td_Print_Version(void);
void td_Shell_Input_Print(void);

void td_Shell_Init(void);
void td_Shell_CMD_Handle(void);

void td_AdminCMD_Check(uint8_t *data, uint16_t len);
void td_ParameterCMD_Check(uint8_t *data, uint16_t len);

#else

#define	TD_SHELL_INIT()
#define	TD_SHELL_DEINIT()
#define	TD_SHELL_PRINT(x)
#define	TD_SHELL_PRINT_CHAR(data, len)
#define	TD_SHELL_PRINT_HEX(data, len)
#define TD_SHELL_RCV_POLL()

#endif	/* TD_SHELL_ENABLED */

#endif	/* TD_SHELL_H */
