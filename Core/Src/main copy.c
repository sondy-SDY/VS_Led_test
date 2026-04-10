/*#include "stm32f1xx_hal.h"

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

// ============ 全局变量 ============
TIM_HandleTypeDef htim3;

float error = 0, prev_error = 0;
float integral = 0;
float pid_val = 0;

// ============ 函数声明 ============
void SystemClock_Config(void);
void GPIO_Init(void);
void TIM3_PWM_Init(void);
void motor_set(int16_t left, int16_t right);
float trace_get_error(void);
void calc_pid(void);

int main(void) {
    HAL_Init();
    SystemClock_Config();
    GPIO_Init();
    TIM3_PWM_Init();
    
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
    
    while (1) {
        error = trace_get_error();
        calc_pid();
        
        int16_t left  = (int16_t)(BASE_SPEED + pid_val);
        int16_t right = (int16_t)(BASE_SPEED - pid_val);
        motor_set(left, right);
        
        HAL_Delay(10);
    }
}

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

void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9; // 8MHz * 9 = 72MHz
    HAL_RCC_OscConfig(&RCC_OscInitStruct);
    
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                   RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;  // 36MHz
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;  // 72MHz
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);
}

void GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    __HAL_RCC_AFIO_CLK_ENABLE();      // 复用功能时钟（必须开启）
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    // 循迹传感器输入 (PA8, PA9)
    GPIO_InitStruct.Pin = S5_PIN | S6_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // 循迹传感器输入 (PB12-PB15)
    GPIO_InitStruct.Pin = S1_PIN | S2_PIN | S3_PIN | S4_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    // 电机方向控制输出 (PB0, PB1, PB10, PB11)
    GPIO_InitStruct.Pin = MOTOR_L_IN1_PIN | MOTOR_L_IN2_PIN | 
                          MOTOR_R_IN1_PIN | MOTOR_R_IN2_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    // PWM引脚 (PA6=TIM3_CH1, PA7=TIM3_CH2) - 复用推挽输出
    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // STM32F103 默认 TIM3 的 CH1/CH2 就在 PA6/PA7，无需重映射
}

void TIM3_PWM_Init(void) {
    __HAL_RCC_TIM3_CLK_ENABLE();
    
    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 71;        // 72MHz / 72 = 1MHz
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = PWM_MAX;      // 1MHz / 1000 = 1kHz PWM
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    HAL_TIM_PWM_Init(&htim3);
    
    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;                      // 初始占空比为0
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1);
    HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2);
}*/