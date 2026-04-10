/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

// ============ 引脚定义 ============
// 6路循迹传感器 (低电平=检测到黑线)
#define S1_PIN  GPIO_PIN_12  // PB12 最左
#define S2_PIN  GPIO_PIN_13  // PB13
#define S3_PIN  GPIO_PIN_14  // PB14
#define S4_PIN  GPIO_PIN_15  // PB15
#define S5_PIN  GPIO_PIN_8   // PA8
#define S6_PIN  GPIO_PIN_9   // PA9 最右

#define S1_PORT GPIOB
#define S2_PORT GPIOB
#define S3_PORT GPIOB
#define S4_PORT GPIOB
#define S5_PORT GPIOA
#define S6_PORT GPIOA

// 左电机 (TIM3_CH1 = PA6)
#define MOTOR_L_IN1_PIN  GPIO_PIN_0   // PB0
#define MOTOR_L_IN2_PIN  GPIO_PIN_1   // PB1
#define MOTOR_L_PORT     GPIOB

// 右电机 (TIM3_CH2 = PA7)
#define MOTOR_R_IN1_PIN  GPIO_PIN_10  // PB10
#define MOTOR_R_IN2_PIN  GPIO_PIN_11  // PB11
#define MOTOR_R_PORT     GPIOB

// PWM参数
#define PWM_MAX  999   // 对应100%占空比
#define BASE_SPEED 400 // 基础速度 (0-999)

// PID参数
#define KP  80.0f
#define KI  0.1f
#define KD  200.0f
#define MAX_I 100.0f

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim3;

/* USER CODE BEGIN PV */

float error = 0, prev_error = 0;
float integral = 0;
float pid_val = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */

void motor_set(int16_t left, int16_t right);
float trace_get_error(void);
void calc_pid(void);

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
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */

  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    error = trace_get_error();
    calc_pid();
    
    int16_t left  = (int16_t)(BASE_SPEED + pid_val);
    int16_t right = (int16_t)(BASE_SPEED - pid_val);
    motor_set(left, right);
    
    HAL_Delay(10);
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 71;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 999;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_AFIO_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Led_GPIO_Port, Led_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : Led_Pin */
  GPIO_InitStruct.Pin = Led_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(Led_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  // --- 循迹传感器输入 (PA8, PA9) ---
  GPIO_InitStruct.Pin = S5_PIN | S6_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // --- 循迹传感器输入 (PB12-PB15) ---
  GPIO_InitStruct.Pin = S1_PIN | S2_PIN | S3_PIN | S4_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  // --- 电机方向控制输出 (PB0, PB1, PB10, PB11) ---
  GPIO_InitStruct.Pin = MOTOR_L_IN1_PIN | MOTOR_L_IN2_PIN | 
                        MOTOR_R_IN1_PIN | MOTOR_R_IN2_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  // --- PWM引脚 (PA6=TIM3_CH1, PA7=TIM3_CH2) ---
  GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

// 读取6路传感器，返回误差值
float trace_get_error(void) {
    // 低电平=检测到黑线=1
    int s[6];
    s[0] = (HAL_GPIO_ReadPin(S1_PORT, S1_PIN) == GPIO_PIN_RESET) ? 1 : 0;
    s[1] = (HAL_GPIO_ReadPin(S2_PORT, S2_PIN) == GPIO_PIN_RESET) ? 1 : 0;
    s[2] = (HAL_GPIO_ReadPin(S3_PORT, S3_PIN) == GPIO_PIN_RESET) ? 1 : 0;
    s[3] = (HAL_GPIO_ReadPin(S4_PORT, S4_PIN) == GPIO_PIN_RESET) ? 1 : 0;
    s[4] = (HAL_GPIO_ReadPin(S5_PORT, S5_PIN) == GPIO_PIN_RESET) ? 1 : 0;
    s[5] = (HAL_GPIO_ReadPin(S6_PORT, S6_PIN) == GPIO_PIN_RESET) ? 1 : 0;

    // 权重: 左负右正
    const int weights[6] = {-5, -3, -1, 1, 3, 5};
    int sum = 0, count = 0;
    for (int i = 0; i < 6; i++) {
        if (s[i]) {
            sum += weights[i];
            count++;
        }
    }

    if (count == 0) return prev_error; // 全部丢线，保持上次误差
    return (float)sum / count;
}

void calc_pid(void) {
    float P = error;
    integral += error;
    if (integral > MAX_I)  integral = MAX_I;
    if (integral < -MAX_I) integral = -MAX_I;
    float D = error - prev_error;
    prev_error = error;
    pid_val = KP * P + KI * integral + KD * D;
}

// 设置电机速度 (-PWM_MAX ~ +PWM_MAX)
void motor_set(int16_t left, int16_t right) {
    // 限幅
    if (left  >  PWM_MAX) left  =  PWM_MAX;
    if (left  < -PWM_MAX) left  = -PWM_MAX;
    if (right >  PWM_MAX) right =  PWM_MAX;
    if (right < -PWM_MAX) right = -PWM_MAX;

    // 左电机
    if (left >= 0) {
        HAL_GPIO_WritePin(MOTOR_L_PORT, MOTOR_L_IN1_PIN, GPIO_PIN_SET);
        HAL_GPIO_WritePin(MOTOR_L_PORT, MOTOR_L_IN2_PIN, GPIO_PIN_RESET);
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, left);
    } else {
        HAL_GPIO_WritePin(MOTOR_L_PORT, MOTOR_L_IN1_PIN, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MOTOR_L_PORT, MOTOR_L_IN2_PIN, GPIO_PIN_SET);
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, -left);
    }

    // 右电机
    if (right >= 0) {
        HAL_GPIO_WritePin(MOTOR_R_PORT, MOTOR_R_IN1_PIN, GPIO_PIN_SET);
        HAL_GPIO_WritePin(MOTOR_R_PORT, MOTOR_R_IN2_PIN, GPIO_PIN_RESET);
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, right);
    } else {
        HAL_GPIO_WritePin(MOTOR_R_PORT, MOTOR_R_IN1_PIN, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MOTOR_R_PORT, MOTOR_R_IN2_PIN, GPIO_PIN_SET);
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, -right);
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
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
