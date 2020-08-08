/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "FreeRTOS.h"
#include "task.h"
#include "nokia5110_LCD.h"
#include "MAX31865.h"
#include "KIMaip.h"
#include "string.h"
#include "stdio.h"
#include "types.h"
#include "logic.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c2;
IWDG_HandleTypeDef hiwdg;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
    .name = "defaultTask",
    .priority = (osPriority_t) osPriorityNormal,
    .stack_size = 128 * 4
};

struct context_t {
    input_values_t* input_values;
    output_values_t* output_values;
    KIMaip_ctx* kimaip_ctx;
};


// KNX communication objects
#define MAX_KNX_OBJECTS 8
CommunicationObject lux_value = {KIM_TYPE_FLOAT, 0};
CommunicationObject water_temp = {KIM_TYPE_FLOAT, 1};
CommunicationObject timer = {KIM_TYPE_BOOL, 2};
CommunicationObject alarm_temp_low = {KIM_TYPE_BOOL, 3};
CommunicationObject alarm_temp_high = {KIM_TYPE_BOOL, 4};
CommunicationObject alarm_230v = {KIM_TYPE_BOOL, 5};
CommunicationObject alarm_24v = {KIM_TYPE_BOOL, 6};
CommunicationObject pump_status = {KIM_TYPE_INT, 7};
CommunicationObject *knx_objects[MAX_KNX_OBJECTS] = {
    &lux_value,
    &water_temp,
    &timer,
    &alarm_temp_low,
    &alarm_temp_high,
    &alarm_230v,
    &alarm_24v,
    &pump_status,
};
KIMaip_ctx kimaip_ctx = {&hi2c2, MAX_KNX_OBJECTS, knx_objects};

input_values_t input_values;
output_values_t output_values;
struct context_t context = {
    .input_values = &input_values,
    .output_values = &output_values,
    .kimaip_ctx = &kimaip_ctx,
};

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C2_Init(void);
static void MX_IWDG_Init(void);
void StartDefaultTask(void *argument);

/* USER CODE BEGIN PFP */

int8_t init_lcd(void)
{
    LCD_setRST(LCD_RST_GPIO_Port, LCD_RST_Pin);
    LCD_setDC(LCD_DC_GPIO_Port, LCD_DC_Pin);
    LCD_setCE(LCD_CE_GPIO_Port, LCD_CE_Pin);
    LCD_setCLK(LCD_CLK_GPIO_Port, LCD_CLK_Pin);
    LCD_setDATA(LCD_DATA_GPIO_Port, LCD_DATA_Pin);
    LCD_init();

    return 0;
}

void lcdThread(void* data)
{
    struct context_t* context = (struct context_t*) data;

    char buffer[30] = "";
    while(1)
    {
        LCD_clear();

        snprintf(buffer, 30, "Tw: %.2f", context->input_values->water_temp);
        LCD_print(buffer, 0, 0);

        snprintf(buffer, 30, "To: %.2f", context->input_values->outside_temp);
        LCD_print(buffer, 0, 1);

        snprintf(buffer, 30, "Lux: %ld", context->input_values->lux);
        LCD_print(buffer, 0, 2);

        snprintf(buffer, 30, "IN : %d %d / %d", context->input_values->power_230v, context->input_values->power_24v, context->input_values->pump_active);
        LCD_print(buffer, 0, 4);

        snprintf(buffer, 30, "OUT: %d %d %d %d", context->output_values->pump_low, context->output_values->pump_high, context->output_values->tracing, context->output_values->cover_open);
        LCD_print(buffer, 0, 5);

        osDelay(500);
    }
}

void read_inputs(input_values_t* input_values, MAX31865_GPIO* sens_outside, MAX31865_GPIO* sens_water, KIMaip_ctx* kimaip)
{
    // Read digital inputs
    input_values->power_230v = HAL_GPIO_ReadPin(BIN_0_GPIO_Port, BIN_0_Pin) ? 0 : 1;
    input_values->power_24v = HAL_GPIO_ReadPin(BIN_1_GPIO_Port, BIN_1_Pin) ? 0 : 1;
    input_values->pump_active = HAL_GPIO_ReadPin(BIN_3_GPIO_Port, BIN_3_Pin) ? 0 : 1;

    // Read temperature
    input_values->water_temp = MAX31865_readTemp(sens_water);
    osDelay(50);
    input_values->outside_temp = MAX31865_readTemp(sens_outside);

    // Read KNX values
    input_values->lux = kimaip->objects[0]->Float;
    input_values->timer = kimaip->objects[2]->Bool;
}

void set_outputs(output_values_t* output_values)
{
    HAL_GPIO_WritePin(OUT_0_GPIO_Port, OUT_0_Pin, output_values->pump_low ? 0 : 1);
    HAL_GPIO_WritePin(OUT_1_GPIO_Port, OUT_1_Pin, output_values->pump_high ? 0 : 1);
    HAL_GPIO_WritePin(OUT_2_GPIO_Port, OUT_2_Pin, output_values->tracing ? 0 : 1);
    HAL_GPIO_WritePin(OUT_3_GPIO_Port, OUT_3_Pin, output_values->cover_open ? 0 : 1);
}

void send_knx(input_values_t* input_values, output_values_t* output_values, KIMaip_ctx* kimaip_ctx)
{
    static uint8_t counter = 0;
    uint8_t pump_status = 0;

    //#define MAX_COM 5
    //static uint8_t last_values[MAX_COM] = {0};

    if (counter >= 10) {
        pump_status =  (output_values->pump_low ? 1 : 0) + output_values->pump_high;
        if (pump_status && input_values->pump_active == 0) {
            // Pump is driven, but motor drive did not activate
            pump_status = 3;
        }

        KIMaip_Send_Int(kimaip_ctx, output_values->alarm_temp_low, 3);
        KIMaip_Send_Int(kimaip_ctx, output_values->alarm_temp_high, 4);
        KIMaip_Send_Int(kimaip_ctx, output_values->alarm_230v, 5);
        KIMaip_Send_Int(kimaip_ctx, output_values->alarm_24v, 6);
        KIMaip_Send_Int(kimaip_ctx, pump_status, 7);

        // Send temp
        KIMaip_Send_Float(kimaip_ctx, input_values->water_temp, 1);

        counter = 0;
    }

    counter++;
}

void StartDefaultTask(void *argument)
{
    struct context_t* context = (struct context_t*) argument;

    // Set both chip select lines high
    HAL_GPIO_WritePin(SS_1_GPIO_Port, SS_1_Pin, 1);
    HAL_GPIO_WritePin(SS_2_GPIO_Port, SS_2_Pin, 1);

    // Init outside temp sens
    MAX31865_GPIO sens_outside = {
        .CE_PORT = SS_2_GPIO_Port,
        .CE_PIN = SS_2_Pin,
        .CLK_PORT = CLK_1_GPIO_Port,
        .CLK_PIN = CLK_1_Pin,
        .MOSI_PORT = MOSI_1_GPIO_Port,
        .MOSI_PIN = MOSI_1_Pin,
        .MISO_PORT = MISO_1_GPIO_Port,
        .MISO_PIN = MISO_1_Pin,
        .counter = 0,
        .moving_average = 0,
        .value = 20,
    };
    MAX31865_init(&sens_outside, 2);

    // Init water temp sens
    MAX31865_GPIO sens_water = {
        .CE_PORT = SS_1_GPIO_Port,
        .CE_PIN = SS_1_Pin,
        .CLK_PORT = CLK_1_GPIO_Port,
        .CLK_PIN = CLK_1_Pin,
        .MOSI_PORT = MOSI_1_GPIO_Port,
        .MOSI_PIN = MOSI_1_Pin,
        .MISO_PORT = MISO_1_GPIO_Port,
        .MISO_PIN = MISO_1_Pin,
        .counter = 0,
        .moving_average = 0,
        .value = 20,
    };
    MAX31865_init(&sens_water, 2);


    while(1)
    {
        // Read inputs
        read_inputs(context->input_values, &sens_outside, &sens_water, context->kimaip_ctx);

        // Logic
        run_logic(context->input_values, context->output_values);

        // Set outputs
        set_outputs(context->output_values);

        // Send to knx
        send_knx(context->input_values, context->output_values, context->kimaip_ctx);

        HAL_IWDG_Refresh(&hiwdg);
        osDelay(1000);
    }
}

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
    MX_I2C2_Init();
    MX_IWDG_Init();
    /* USER CODE BEGIN 2 */

    init_lcd();
    LCD_print("Boot", 0, 0);
    HAL_Delay(1000);

    /* USER CODE END 2 */

    /* Init scheduler */
    osKernelInitialize();

    /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
    /* USER CODE END RTOS_MUTEX */

    /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
    /* USER CODE END RTOS_SEMAPHORES */

    /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
    /* USER CODE END RTOS_TIMERS */

    /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
    /* USER CODE END RTOS_QUEUES */
    /* Create the thread(s) */
    /* creation of defaultTask */
    defaultTaskHandle = osThreadNew(StartDefaultTask, (void*)&context, &defaultTask_attributes);
    TaskHandle_t lcdThread_handle = NULL;
    xTaskCreate(lcdThread, "LCD", 128*4, (void*)&context, 1, &lcdThread_handle);

    /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
    /* USER CODE END RTOS_THREADS */

    /* Start scheduler */
    osKernelStart();

    /* We should never get here as control is now taken by the scheduler */
    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
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

    /** Initializes the CPU, AHB and APB busses clocks 
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
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
}

/**
 * @brief I2C2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2C2_Init(void)
{

    /* USER CODE BEGIN I2C2_Init 0 */

    /* USER CODE END I2C2_Init 0 */

    /* USER CODE BEGIN I2C2_Init 1 */

    /* USER CODE END I2C2_Init 1 */
    hi2c2.Instance = I2C2;
    hi2c2.Init.Timing = 0x00101D7C;
    hi2c2.Init.OwnAddress1 = 0;
    hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c2.Init.OwnAddress2 = 0;
    hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c2) != HAL_OK)
    {
        Error_Handler();
    }
    /** Configure Analogue filter 
     */
    if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
    {
        Error_Handler();
    }
    /** Configure Digital filter 
     */
    if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN I2C2_Init 2 */

    /* USER CODE END I2C2_Init 2 */

}

static void MX_IWDG_Init(void)
{
    hiwdg.Instance = IWDG;
    hiwdg.Init.Prescaler = IWDG_PRESCALER_256;
    hiwdg.Init.Window = 4000;
    hiwdg.Init.Reload = 4000;
    if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
    {
      Error_Handler();
    }
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
    HAL_GPIO_WritePin(GPIOA, OUT_0_Pin|OUT_1_Pin|OUT_3_Pin|OUT_2_Pin 
            |CLK_1_Pin|SS_1_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOB, SS_2_Pin|MOSI_1_Pin|LCD_CLK_Pin|LCD_DATA_Pin 
            |LCD_DC_Pin|LCD_CE_Pin|LCD_RST_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pins : BIN_0_Pin BIN_1_Pin BIN_2_Pin BIN_3_Pin 
      MISO_1_Pin */
    GPIO_InitStruct.Pin = BIN_0_Pin|BIN_1_Pin|BIN_2_Pin|BIN_3_Pin 
        |MISO_1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pins : OUT_0_Pin OUT_1_Pin OUT_3_Pin OUT_2_Pin 
      CLK_1_Pin SS_1_Pin */
    GPIO_InitStruct.Pin = OUT_0_Pin|OUT_1_Pin|OUT_3_Pin|OUT_2_Pin 
        |CLK_1_Pin|SS_1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pin : DRDY_Pin */
    GPIO_InitStruct.Pin = DRDY_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(DRDY_GPIO_Port, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);

    /*Configure GPIO pins : SS_2_Pin MOSI_1_Pin LCD_CLK_Pin LCD_DATA_Pin 
      LCD_DC_Pin LCD_CE_Pin LCD_RST_Pin */
    GPIO_InitStruct.Pin = SS_2_Pin|MOSI_1_Pin|LCD_CLK_Pin|LCD_DATA_Pin 
        |LCD_DC_Pin|LCD_CE_Pin|LCD_RST_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM1 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    /* USER CODE BEGIN Callback 0 */

    /* USER CODE END Callback 0 */
    if (htim->Instance == TIM1) {
        HAL_IncTick();
    }
    /* USER CODE BEGIN Callback 1 */

    /* USER CODE END Callback 1 */
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == DRDY_Pin)
    {
        KIMaip_Handle_Interrupt(&kimaip_ctx);
    }
}

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
void assert_failed(uint8_t *file, uint32_t line)
{ 
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
