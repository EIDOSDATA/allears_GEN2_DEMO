#ifndef INC_APP_ECHO_SHELL_H_
#define INC_APP_ECHO_SHELL_H_

#define ECHO_SHELL_UART											USART2

#ifdef	ECHO_SHELL_ENABLED
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "echo_private.h"

typedef enum
{
	START = 0x00, STOP, SAVE, FACTORYRESET, HELP, VERSION, admin_cmd_max
} admin_command_t;

typedef enum
{
	SET_DEADTIME = 0x00,
	SET_PULSEWIDTH,
	SET_PERIOD,
	SET_VOLTAGE_PW,

	GET_DEADTIME,
	GET_PULSEWIDTH,
	GET_FREQUENCY,
	GET_VOLTAGE_PW,
	GET_ALLPRM,
	parameter_cmd_max
} parameter_command_t;

typedef enum
{
	RESPONSE_DEADTIME = 0x00,
	RESPONSE_PULSEWIDTH,
	RESPONSE_FREQUENCY,
	RESPONSE_VOLTAGE_PW,
	RESPONSE_ALLPRM,
	response_prm_cmd_max
} response_prm_command_t;

/* Enable debug print as each module in here */
#define ECHO_SHELL_INIT(void)										Echo_Shell_Init()
#define ECHO_SHELL_DEINIT(void)										Echo_Shell_DeInit()
#define ECHO_SHELL_PRINT(x)											printf x

#define ECHO_SHELL_PRINT_CHAR(data, len)						Echo_ShellPrint_Char(data, len)
#define ECHO_SHELL_PRINT_HEX(data, len)							Echo_ShellPrint_Hex(data, len)
#define ECHO_SHELL_RCV_POLL()									Echo_Shell_RxPoll()

void Echo_Shell_RxPoll(void);
void Echo_ShellPrint_Char(uint8_t *data, uint16_t len);
void Echo_ShellPrint_Hex(uint8_t *data, uint16_t len);

int Echo_Shell_printf(const char *format, ...);

void Echo_Print_Manual(void);
void Echo_Print_Version(void);
void Echo_Shell_Input_Print(void);

void Echo_Shell_Init(void);
void Echo_Shell_CMD_Handle(void);

void Echo_AdminCMD_Check(uint8_t *data, uint16_t len);
void Echo_ParameterCMD_Check(uint8_t *data, uint16_t len);

#else

#define	ECHO_SHELL_INIT()
#define	ECHO_SHELL_DEINIT()
#define	ECHO_SHELL_PRINT(x)
#define	ECHO_SHELL_PRINT_CHAR(data, len)
#define	ECHO_SHELL_PRINT__HEX(data, len)
#define ECHO_SHELL_RCV_POLL()

#endif	/* ECHO_SHELL_ENABLED */

#endif	/* ECHO_SHELL_H */

