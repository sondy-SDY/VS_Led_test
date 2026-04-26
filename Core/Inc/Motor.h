#ifndef __MOTOR_H
#define __MOTOR_H

#include "main.h"
#include <stdint.h>

// 左电机 (TIM3_CH1 = PA6)
#define MOTOR_L_IN1_PIN  GPIO_PIN_0   // PB0
#define MOTOR_L_IN2_PIN  GPIO_PIN_1   // PB1
#define MOTOR_L_PORT     GPIOB

// 右电机 (TIM3_CH2 = PA7)
#define MOTOR_R_IN1_PIN  GPIO_PIN_10  // PB10
#define MOTOR_R_IN2_PIN  GPIO_PIN_11  // PB11
#define MOTOR_R_PORT     GPIOB

// PWM参数
#define PWM_MAX    999   // 对应100%占空比
#define BASE_SPEED 400   // 基础速度 (0-999)

void motor_set(int16_t left, int16_t right);

#endif /* __MOTOR_H */
