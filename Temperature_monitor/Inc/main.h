/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stm32f0xx_hal.h"

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
#define BIN_0_Pin GPIO_PIN_0
#define BIN_0_GPIO_Port GPIOA
#define BIN_1_Pin GPIO_PIN_1
#define BIN_1_GPIO_Port GPIOA
#define BIN_2_Pin GPIO_PIN_2
#define BIN_2_GPIO_Port GPIOA
#define BIN_3_Pin GPIO_PIN_3
#define BIN_3_GPIO_Port GPIOA
#define SS_1_Pin GPIO_PIN_4
#define SS_1_GPIO_Port GPIOA
#define CLK_1_Pin GPIO_PIN_5
#define CLK_1_GPIO_Port GPIOA
#define MISO_1_Pin GPIO_PIN_6
#define MISO_1_GPIO_Port GPIOA
#define MOSI_1_Pin GPIO_PIN_7
#define MOSI_1_GPIO_Port GPIOA
#define OUT_0_Pin GPIO_PIN_0
#define OUT_0_GPIO_Port GPIOB
#define OUT_1_Pin GPIO_PIN_1
#define OUT_1_GPIO_Port GPIOB
#define OUT_2_Pin GPIO_PIN_2
#define OUT_2_GPIO_Port GPIOB
#define LCD_CLK_Pin GPIO_PIN_10
#define LCD_CLK_GPIO_Port GPIOB
#define LCD_RST_Pin GPIO_PIN_11
#define LCD_RST_GPIO_Port GPIOB
#define LCD_CE_Pin GPIO_PIN_12
#define LCD_CE_GPIO_Port GPIOB
#define LCD_DC_Pin GPIO_PIN_13
#define LCD_DC_GPIO_Port GPIOB
#define LCD_DATA_Pin GPIO_PIN_15
#define LCD_DATA_GPIO_Port GPIOB
#define DRDY_Pin GPIO_PIN_11
#define DRDY_GPIO_Port GPIOA
#define DRDY_EXTI_IRQn EXTI4_15_IRQn
#define OUT_3_Pin GPIO_PIN_3
#define OUT_3_GPIO_Port GPIOB
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
