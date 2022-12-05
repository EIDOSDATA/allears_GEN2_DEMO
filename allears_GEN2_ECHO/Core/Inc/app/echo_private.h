#ifndef INC_APP_ECHO_PRIVATE_H_
#define INC_APP_ECHO_PRIVATE_H_

#include "stm32l4xx_hal.h"

#ifdef DEBUG
#define ECHO_FW_VER										"v1.00_DEBUG"

#else
#define ECHO_FW_VER										"v1.00_RELEASE"
#endif

#define ECHO_DELAY_50MS									50			/* 50ms */
#define ECHO_DELAY_100MS									100			/* 100ms */
#define ECHO_DELAY_200MS									200			/* 200ms */
#define ECHO_DELAY_300MS									300			/* 300ms */
#define ECHO_DELAY_500MS									500			/* 500ms */
#define ECHO_DELAY_1SEC									1000		/* 1000ms */
#define ECHO_DELAY_2SEC									2000		/* 2000ms */
#define ECHO_DELAY_3SEC									3000		/* 3000ms */
#endif /* AUL_PRIVATE_H */

