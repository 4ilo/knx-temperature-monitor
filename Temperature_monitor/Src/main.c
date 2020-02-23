/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "nokia5110_LCD.h"
#include "MAX31865.h"
#include "KIMaip.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

#define MAX_INPUTS 5
#define MAX_OUTPUTS 5
typedef struct {
    uint8_t size;
    uint16_t pins[MAX_INPUTS];
    GPIO_TypeDef *ports[MAX_INPUTS];
    uint16_t objectNrs[MAX_INPUTS];

    uint8_t data[MAX_INPUTS];
} binaryInData;

typedef struct {
    uint8_t size;
    uint16_t pins[MAX_OUTPUTS];
    GPIO_TypeDef *ports[MAX_OUTPUTS];
    CommunicationObject *communicationObjects[MAX_OUTPUTS];
} binaryOutData;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define BIN_INPUTS 4
#define BIN_OUTPUTS 4

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

IWDG_HandleTypeDef hiwdg;

/* USER CODE BEGIN PV */

// KIMaip communication objects
CommunicationObject b_out0 = {KIM_TYPE_BOOL, 0};
CommunicationObject b_out1 = {KIM_TYPE_BOOL, 1};
CommunicationObject b_out2 = {KIM_TYPE_BOOL, 2};
CommunicationObject b_out3 = {KIM_TYPE_BOOL, 3};
CommunicationObject illumination = {KIM_TYPE_FLOAT, 9};
CommunicationObject *objects[5] = {&b_out0, &b_out1, &b_out2, &b_out3, &illumination};
uint8_t object_count = 5;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_IWDG_Init(void);
/* USER CODE BEGIN PFP */

void init_outputs(binaryOutData *b_out);
void validate_pool_temp(float temperature);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_IWDG_Init();
  /* USER CODE BEGIN 2 */

    // Init LCD
    LCD_setRST(LCD_RST_GPIO_Port, LCD_RST_Pin);
    LCD_setDC(LCD_DC_GPIO_Port, LCD_DC_Pin);
    LCD_setCE(LCD_CE_GPIO_Port, LCD_CE_Pin);
    LCD_setCLK(LCD_CLK_GPIO_Port, LCD_CLK_Pin);
    LCD_setDATA(LCD_DATA_GPIO_Port, LCD_DATA_Pin);
    LCD_init();

    // Init MAX31865
    MAX31865_GPIO max_gpio;
    max_gpio.MISO_PIN = MISO_1_Pin;
    max_gpio.MISO_PORT = MISO_1_GPIO_Port;
    max_gpio.MOSI_PIN = MOSI_1_Pin;
    max_gpio.MOSI_PORT = MOSI_1_GPIO_Port;
    max_gpio.CLK_PIN = CLK_1_Pin;
    max_gpio.CLK_PORT = CLK_1_GPIO_Port;
    max_gpio.CE_PIN = SS_1_Pin;
    max_gpio.CE_PORT = SS_1_GPIO_Port;
    MAX31865_init(&max_gpio, 3);

    float temperature, avg_temp = 0;

    binaryInData b_in = {
            BIN_INPUTS,                                                                 // Len
            {BIN_0_Pin, BIN_1_Pin, BIN_2_Pin, BIN_3_Pin},                               // GPIO pins
            {BIN_0_GPIO_Port, BIN_1_GPIO_Port, BIN_2_GPIO_Port, BIN_3_GPIO_Port},       // GPIO ports
            {4, 5, 6, 7}                                                                // Communication object numbers
    };

    binaryOutData b_out = {
            BIN_OUTPUTS,                                                                 // Len
            {OUT_0_Pin, OUT_1_Pin, OUT_2_Pin, OUT_3_Pin},                               // GPIO pins
            {OUT_0_GPIO_Port, OUT_1_GPIO_Port, OUT_2_GPIO_Port, OUT_3_GPIO_Port},       // GPIO ports
            {&b_out0, &b_out1, &b_out2, &b_out3}                                        // Communication objects
    };

    uint8_t send_temp_counter = 0;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while (1)
    {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

        // Read temperature and show on screen, and send to bus
        temperature = MAX31865_readTemp();


        // Onlu send temp every 10 seconds
        if(send_temp_counter >= 10)
        {
            avg_temp = avg_temp / 10;
            KIMaip_Send_Float(avg_temp, 8);
            LCD_print_float(avg_temp, 2, 9, FONT_BIG);

            // Logic functions
            validate_pool_temp(avg_temp);

            send_temp_counter = 0;
            avg_temp = 0;
        }
        else
        {
            avg_temp += temperature;
            send_temp_counter++;
        }

        // Update output status
        for(uint8_t i = 0; i < b_out.size; i++)
        {
            HAL_GPIO_WritePin(b_out.ports[i], b_out.pins[i], b_out.communicationObjects[i]->Bool);
        }


        // Read binary inputs, and send to bus
        uint8_t temp;
        for(uint8_t i = 0; i < b_in.size; i++)
        {
            temp = !HAL_GPIO_ReadPin(b_in.ports[i], b_in.pins[i]);
            if (temp != b_in.data[i])
            {
                b_in.data[i] = temp;
                KIMaip_Send_Bool(b_in.data[i], b_in.objectNrs[i]);
            }
        }

        HAL_IWDG_Refresh(&hiwdg);
        HAL_Delay(1000);

    }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00101D7C;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter 
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter 
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief IWDG Initialization Function
  * @param None
  * @retval None
  */
static void MX_IWDG_Init(void)
{

  /* USER CODE BEGIN IWDG_Init 0 */

  /* USER CODE END IWDG_Init 0 */

  /* USER CODE BEGIN IWDG_Init 1 */

  /* USER CODE END IWDG_Init 1 */
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_256;
  hiwdg.Init.Window = 4000;
  hiwdg.Init.Reload = 4000;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN IWDG_Init 2 */

  /* USER CODE END IWDG_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, SS_1_Pin|CLK_1_Pin|MOSI_1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, OUT_0_Pin|OUT_1_Pin|OUT_2_Pin|LCD_CLK_Pin 
                          |LCD_RST_Pin|LCD_CE_Pin|LCD_DC_Pin|LCD_DATA_Pin 
                          |OUT_3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : BIN_0_Pin BIN_1_Pin BIN_2_Pin BIN_3_Pin 
                           MISO_1_Pin */
  GPIO_InitStruct.Pin = BIN_0_Pin|BIN_1_Pin|BIN_2_Pin|BIN_3_Pin 
                          |MISO_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : SS_1_Pin CLK_1_Pin MOSI_1_Pin */
  GPIO_InitStruct.Pin = SS_1_Pin|CLK_1_Pin|MOSI_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : OUT_0_Pin OUT_1_Pin OUT_2_Pin LCD_CLK_Pin 
                           LCD_RST_Pin LCD_CE_Pin LCD_DC_Pin LCD_DATA_Pin 
                           OUT_3_Pin */
  GPIO_InitStruct.Pin = OUT_0_Pin|OUT_1_Pin|OUT_2_Pin|LCD_CLK_Pin 
                          |LCD_RST_Pin|LCD_CE_Pin|LCD_DC_Pin|LCD_DATA_Pin 
                          |OUT_3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : DRDY_Pin */
  GPIO_InitStruct.Pin = DRDY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DRDY_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);

}

/* USER CODE BEGIN 4 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == DRDY_Pin)
    {
        KIMaip_Handle_Interrupt();
    }
}

void validate_pool_temp(float temperature)
{
    float temp_limit = 30.0;
    static uint8_t pool_too_hot = 0;

    if(temperature >= temp_limit && !pool_too_hot)
    {
        pool_too_hot = 1;
        KIMaip_Send_Bool(1, 10);
    }
    else if(temperature < (temp_limit-1) && pool_too_hot)
    {
        pool_too_hot = 0;
        KIMaip_Send_Bool(0, 10);
    }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
