/*
 * echo_led.h
 *
 *  Created on: Oct 24, 2022
 *      Author: ECHO
 */

#ifndef INC_APP_ECHO_LED_H_
#define INC_APP_ECHO_LED_H_

#include <stdbool.h>

typedef enum
{
	/* State LED */
	echo_led_state_none = 0x00, echo_led_idle, echo_led_run, echo_led_state_max
} echo_led_sate_t;

typedef enum
{
	echo_led_color_none = 0x00, echo_led_red = (1 << 0), echo_led_green = (1
			<< 1), echo_led_blue = (1 << 2)
//ECHO_LED_WHITE = (aul_led_red| aul_led_green | aul_led_blue)
} echo_led_color_t;

/*
 __STATIC_INLINE void Echo_LED_Green_Off(void);
 __STATIC_INLINE void Echo_LED_Green_On(void);
 __STATIC_INLINE void Echo_LED_Off_All(void);
 __STATIC_INLINE void Echo_LED_State_Reset(void);
 */
#define ECHO_LED_STEADY_OFF_TIME						0xFFFF
#define ECHO_LED_STEADY_ON_TIME							0

#define ECHO_LED_IND_TIMEOUT							0xFFFF // ((TD_DELAY_3SEC * 10) / TD_SCHED_HANDLE_PERIOD) 	/* 10s */
#define ECHO_LED_COLOR_ALL								(echo_led_red | echo_led_green | echo_led_blue)

#define ECHO_LED_STATE_IS_EVENT(param)					(echo_led_low_batt <= param && param < echo_led_state_max)

#define ECHO_LED_CUR_STATE								echo_led_state.led_state
#define ECHO_LED_ON										echo_led_state.led_on
#define ECHO_LED_TIME_TICK 								echo_led_state.led_tick
#define ECHO_LED_TIMEOUT_TICK 							echo_led_state.led_timeout_tick
#define ECHO_LED_TIMED_OUT	 							echo_led_state.led_timed_out

#define ECHO_LED_STATE_IND_COLORS_GET(param)			echo_led_indication_table[param].led_colors
#define ECHO_LED_STATE_IND_OFF_TIME_GET(param)			echo_led_indication_table[param].off_time
#define ECHO_LED_STATE_IND_ON_TIME_GET(param)			echo_led_indication_table[param].on_time

#define ECHO_LED_STATE_IND_TIMEOUT_APPLIED(param)		(param)

#define ECHO_LED_STATE_IND_TIMEOUT_RESET()				{ECHO_LED_TIMEOUT_TICK = 0; ECHO_LED_TIMED_OUT = false;}

#define ECHO_LED_IND_DISABLED()							(ECHO_LED_TIMED_OUT == true)

void Echo_LED_Init(void);
void Echo_LED_Enable(void);
bool Echo_LED_Indication_Disable(void);
void Echo_Set_LED_State(echo_led_sate_t led_state);
void Echo_LED_State_Refresh(void);
void Echo_LED_Handle(void);
#endif /* INC_APP_ECHO_LED_H_ */
