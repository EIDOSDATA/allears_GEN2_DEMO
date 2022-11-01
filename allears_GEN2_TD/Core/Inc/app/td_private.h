#ifndef INC_APP_TD_PRIVATE_H_
#define INC_APP_TD_PRIVATE_H_

#include "stm32l4xx_hal.h"

#ifdef DEBUG
#define TD_FW_VER										"v1.00_DEBUG"

#else
#define TD_FW_VER										"v1.00_RELEASE"
#endif

#define TD_DELAY_50MS									50			/* 50ms */
#define TD_DELAY_100MS									100			/* 100ms */
#define TD_DELAY_200MS									200			/* 200ms */
#define TD_DELAY_300MS									300			/* 300ms */
#define TD_DELAY_500MS									500			/* 500ms */
#define TD_DELAY_1SEC									1000		/* 1000ms */
#define TD_DELAY_2SEC									2000		/* 2000ms */
#define TD_DELAY_3SEC									3000		/* 3000ms */
#endif /* AUL_PRIVATE_H */

