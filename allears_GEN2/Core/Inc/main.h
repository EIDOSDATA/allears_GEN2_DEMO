/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "echo_private.h"
#include "echo_shell.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void HAL_TIM_OC_MspInit(TIM_HandleTypeDef *htim);
void HAL_TIM_OC_MspDeInit(TIM_HandleTypeDef *htim);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define STIM_ANODE_Pin GPIO_PIN_1
#define STIM_ANODE_GPIO_Port GPIOA
#define VCP_TX_Pin GPIO_PIN_2
#define VCP_TX_GPIO_Port GPIOA
#define STIM_CATHODE_Pin GPIO_PIN_3
#define STIM_CATHODE_GPIO_Port GPIOA
#define START_BTN_Pin GPIO_PIN_4
#define START_BTN_GPIO_Port GPIOA
#define STIM_CURRENT_CTRL_Pin GPIO_PIN_5
#define STIM_CURRENT_CTRL_GPIO_Port GPIOA
#define VDDH_FEEDBACK_Pin GPIO_PIN_6
#define VDDH_FEEDBACK_GPIO_Port GPIOA
#define PEAK_DETECTION_Pin GPIO_PIN_7
#define PEAK_DETECTION_GPIO_Port GPIOA
#define QCC_CRTL0_Pin GPIO_PIN_0
#define QCC_CRTL0_GPIO_Port GPIOB
#define QCC_CRTL1_Pin GPIO_PIN_1
#define QCC_CRTL1_GPIO_Port GPIOB
#define STEP_UP_SW_Pin GPIO_PIN_8
#define STEP_UP_SW_GPIO_Port GPIOA
#define DAC0_Pin GPIO_PIN_9
#define DAC0_GPIO_Port GPIOA
#define DAC1_Pin GPIO_PIN_10
#define DAC1_GPIO_Port GPIOA
#define DAC2_Pin GPIO_PIN_11
#define DAC2_GPIO_Port GPIOA
#define DAC3_Pin GPIO_PIN_12
#define DAC3_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define VCP_RX_Pin GPIO_PIN_15
#define VCP_RX_GPIO_Port GPIOA
#define LED_Pin GPIO_PIN_3
#define LED_GPIO_Port GPIOB
#define BIPHASIC_SW_Pin GPIO_PIN_4
#define BIPHASIC_SW_GPIO_Port GPIOB
#define PEAK_DISCHG_SW_Pin GPIO_PIN_5
#define PEAK_DISCHG_SW_GPIO_Port GPIOB
#define QCC_UART_TX_Pin GPIO_PIN_6
#define QCC_UART_TX_GPIO_Port GPIOB
#define QCC_UART_RX_Pin GPIO_PIN_7
#define QCC_UART_RX_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
