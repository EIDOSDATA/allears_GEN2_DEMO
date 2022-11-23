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

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define START_BTN_Pin GPIO_PIN_13
#define START_BTN_GPIO_Port GPIOC
#define MCO_Pin GPIO_PIN_0
#define MCO_GPIO_Port GPIOH
#define STIM_ANODE_Pin GPIO_PIN_1
#define STIM_ANODE_GPIO_Port GPIOA
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define SMPS_EN_Pin GPIO_PIN_4
#define SMPS_EN_GPIO_Port GPIOA
#define SMPS_V1_Pin GPIO_PIN_5
#define SMPS_V1_GPIO_Port GPIOA
#define VDDH_FEEDBACK_Pin GPIO_PIN_6
#define VDDH_FEEDBACK_GPIO_Port GPIOA
#define PEAK_DETECTION_Pin GPIO_PIN_7
#define PEAK_DETECTION_GPIO_Port GPIOA
#define QCC_CTRL0_Pin GPIO_PIN_0
#define QCC_CTRL0_GPIO_Port GPIOB
#define QCC_CTRL1_Pin GPIO_PIN_1
#define QCC_CTRL1_GPIO_Port GPIOB
#define STIM_CATHODE_Pin GPIO_PIN_11
#define STIM_CATHODE_GPIO_Port GPIOB
#define LED_Pin GPIO_PIN_13
#define LED_GPIO_Port GPIOB
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
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define STIM_CURRENT_CTRL_Pin GPIO_PIN_15
#define STIM_CURRENT_CTRL_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define BIPHASIC_SW_Pin GPIO_PIN_4
#define BIPHASIC_SW_GPIO_Port GPIOB
#define PEAK_DISCHG_SW_Pin GPIO_PIN_5
#define PEAK_DISCHG_SW_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
