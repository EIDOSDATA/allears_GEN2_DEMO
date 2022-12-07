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
	td_led_state_none = 0x00, td_led_idle, td_led_run, td_led_state_max
} td_led_sate_t;

/*
 __STATIC_INLINE void td_LED_Green_Off(void);
 __STATIC_INLINE void td_LED_Green_On(void);
 __STATIC_INLINE void td_LED_Off_All(void);
 __STATIC_INLINE void td_LED_State_Reset(void);
 */

void td_LED_Init(void);
void td_LED_Enable(void);
bool td_LED_Indication_Disable(void);
void td_Set_LED_State(td_led_sate_t led_state);
void td_LED_State_Refresh(void);
void td_LED_Handle(void);
#endif /* INC_APP_TD_LED_H_ */
