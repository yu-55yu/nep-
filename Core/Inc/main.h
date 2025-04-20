/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32h7xx_hal.h"

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
#define PF1_Pin GPIO_PIN_12
#define PF1_GPIO_Port GPIOB
#define PF2_Pin GPIO_PIN_13
#define PF2_GPIO_Port GPIOB
#define OSK_Pin GPIO_PIN_8
#define OSK_GPIO_Port GPIOD
#define PF0_Pin GPIO_PIN_9
#define PF0_GPIO_Port GPIOD
#define DRC_Pin GPIO_PIN_10
#define DRC_GPIO_Port GPIOD
#define DRH_Pin GPIO_PIN_14
#define DRH_GPIO_Port GPIOD
#define IUP_Pin GPIO_PIN_15
#define IUP_GPIO_Port GPIOD
#define SDI_Pin GPIO_PIN_2
#define SDI_GPIO_Port GPIOG
#define SCK_Pin GPIO_PIN_3
#define SCK_GPIO_Port GPIOG
#define CSN_Pin GPIO_PIN_4
#define CSN_GPIO_Port GPIOG
#define MRT_Pin GPIO_PIN_5
#define MRT_GPIO_Port GPIOG
#define SCL_Pin GPIO_PIN_6
#define SCL_GPIO_Port GPIOB
#define SDA_Pin GPIO_PIN_7
#define SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
