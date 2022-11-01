/*
 * td_led.h
 *
 *  Created on: Oct 24, 2022
 *      Author: ECHO
 */

#ifndef INC_APP_TD_LED_H_
#define INC_APP_TD_LED_H_

#include <stdbool.h>

typedef enum
{
	/* State LED */
	td_led_state_none = 0x00,
	td_led_idle,
	td_led_run,
	td_led_state_max
} td_led_sate_t;

typedef enum
{
	td_led_color_none = 0x00,
	td_led_red = (1 << 0),
	td_led_green = (1 << 1),
	td_led_blue = (1 << 2)
} td_led_color_t;

/*
 __STATIC_INLINE void td_LED_Green_Off(void);
 __STATIC_INLINE void td_LED_Green_On(void);
 __STATIC_INLINE void td_LED_Off_All(void);
 __STATIC_INLINE void td_LED_State_Reset(void);
 */
#define TD_LED_STEADY_OFF_TIME							0xFFFF
#define TD_LED_STEADY_ON_TIME							0

#define TD_LED_IND_TIMEOUT								0xFFFF // ((TD_DELAY_3SEC * 10) / TD_SCHED_HANDLE_PERIOD) 	/* 10s */
#define TD_LED_COLOR_ALL								(td_led_red | te_led_green | td_led_blue)

#define TD_LED_STATE_IS_EVENT(param)					(td_led_low_batt <= param && param < td_led_state_max)

#define TD_LED_CUR_STATE								td_led_state.led_state
#define TD_LED_ON										td_led_state.led_on
#define TD_LED_TIME_TICK 								td_led_state.led_tick
#define TD_LED_TIMEOUT_TICK 							td_led_state.led_timeout_tick
#define TD_LED_TIMED_OUT	 							td_led_state.led_timed_out

#define TD_LED_STATE_IND_COLORS_GET(param)				td_led_indication_table[param].led_colors
#define TD_LED_STATE_IND_OFF_TIME_GET(param)			td_led_indication_table[param].off_time
#define TD_LED_STATE_IND_ON_TIME_GET(param)				td_led_indication_table[param].on_time

#define TD_LED_STATE_IND_TIMEOUT_APPLIED(param)			(param)

#define TD_LED_STATE_IND_TIMEOUT_RESET()				{TD_LED_TIMEOUT_TICK = 0; TD_LED_TIMED_OUT = false;}

#define TD_LED_IND_DISABLED()							(TD_LED_TIMED_OUT == true)

void td_LED_Init(void);
void td_LED_Enable(void);
bool td_LED_Indication_Disable(void);
void td_Set_LED_State(td_led_sate_t led_state);
void td_LED_State_Refresh(void);
void td_LED_Handle(void);
#endif /* INC_APP_TD_LED_H_ */
