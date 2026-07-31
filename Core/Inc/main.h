/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BTN_4_GPIO_EXTI6_Pin GPIO_PIN_2
#define BTN_4_GPIO_EXTI6_GPIO_Port GPIOC
#define LED_4_GPIO_Output_Pin GPIO_PIN_3
#define LED_4_GPIO_Output_GPIO_Port GPIOC
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define ERR_LED_GPIO_Output_Pin GPIO_PIN_13
#define ERR_LED_GPIO_Output_GPIO_Port GPIOB
#define ERR_BUZZER_GPIO_Output_Pin GPIO_PIN_14
#define ERR_BUZZER_GPIO_Output_GPIO_Port GPIOB
#define BTN_3_GPIO_EXTI6_Pin GPIO_PIN_6
#define BTN_3_GPIO_EXTI6_GPIO_Port GPIOC
#define BTN_3_GPIO_EXTI6_EXTI_IRQn EXTI9_5_IRQn
#define BTN_2_GPIO_EXTI8_Pin GPIO_PIN_8
#define BTN_2_GPIO_EXTI8_GPIO_Port GPIOC
#define BTN_2_GPIO_EXTI8_EXTI_IRQn EXTI9_5_IRQn
#define BTN_1_GPIO_EXTI9_Pin GPIO_PIN_9
#define BTN_1_GPIO_EXTI9_GPIO_Port GPIOC
#define BTN_1_GPIO_EXTI9_EXTI_IRQn EXTI9_5_IRQn
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define LED_1_GPIO_Output_Pin GPIO_PIN_10
#define LED_1_GPIO_Output_GPIO_Port GPIOC
#define LED_2_GPIO_Output_Pin GPIO_PIN_11
#define LED_2_GPIO_Output_GPIO_Port GPIOC
#define LED_3_GPIO_Output_Pin GPIO_PIN_12
#define LED_3_GPIO_Output_GPIO_Port GPIOC
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
