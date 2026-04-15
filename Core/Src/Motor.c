#include "Motor.h"

extern TIM_HandleTypeDef htim3;

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
