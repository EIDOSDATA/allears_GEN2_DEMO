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
	ECHO_LED_STATE_NONE = 0x00, ECHO_LED_IDLE, ECHO_LED_RUN,

	/* Event LED */
	/*
	 aul_led_low_batt,
	 aul_led_btn_pairing,
	 aul_led_btn_force_inear_chg,
	 */
	ECHO_LED_STATE_MAX
} echo_led_sate_t;

/*
 __STATIC_INLINE void Echo_LED_Green_Off(void);
 __STATIC_INLINE void Echo_LED_Green_On(void);
 __STATIC_INLINE void Echo_LED_Off_All(void);
 __STATIC_INLINE void Echo_LED_State_Reset(void);
 */

void Echo_LED_Init(void);
void Echo_LED_Enable(void);
bool Echo_LED_Indication_Disable(void);
void Echo_Set_LED_State(echo_led_sate_t led_state);
void Echo_LED_State_Refresh(void);
void Echo_LED_Handle(void);
#endif /* INC_APP_ECHO_LED_H_ */
