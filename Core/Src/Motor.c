#include "Motor.h"

extern TIM_HandleTypeDef htim3;

static int16_t last_left = 0;
static int16_t last_right = 0;

static int16_t limit_step(int16_t target, int16_t current) {
    int16_t delta = target - current;
    if (delta > MOTOR_PWM_STEP) {
        return current + MOTOR_PWM_STEP;
    }
    if (delta < -MOTOR_PWM_STEP) {
        return current - MOTOR_PWM_STEP;
    }
    return target;
}

// 设置电机速度 (-PWM_MAX ~ +PWM_MAX)
void motor_set(int16_t left, int16_t right) {
    left += LEFT_TRIM;
    right += RIGHT_TRIM;

    // 限幅
    if (left  >  PWM_MAX) left  =  PWM_MAX;
    if (left  < -PWM_MAX) left  = -PWM_MAX;
    if (right >  PWM_MAX) right =  PWM_MAX;
    if (right < -PWM_MAX) right = -PWM_MAX;

    left = limit_step(left, last_left);
    right = limit_step(right, last_right);
    last_left = left;
    last_right = right;

    // 左电机
    if (left >= 0) {
        HAL_GPIO_WritePin(MOTOR_L_PORT, MOTOR_L_IN1_PIN, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MOTOR_L_PORT, MOTOR_L_IN2_PIN, GPIO_PIN_SET);
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, left);
    } else {
        HAL_GPIO_WritePin(MOTOR_L_PORT, MOTOR_L_IN1_PIN, GPIO_PIN_SET);
        HAL_GPIO_WritePin(MOTOR_L_PORT, MOTOR_L_IN2_PIN, GPIO_PIN_RESET);
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, -left);
    }

    // 右电机
    if (right >= 0) {
        HAL_GPIO_WritePin(MOTOR_R_PORT, MOTOR_R_IN1_PIN, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MOTOR_R_PORT, MOTOR_R_IN2_PIN, GPIO_PIN_SET);
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, right);
    } else {
        HAL_GPIO_WritePin(MOTOR_R_PORT, MOTOR_R_IN1_PIN, GPIO_PIN_SET);
        HAL_GPIO_WritePin(MOTOR_R_PORT, MOTOR_R_IN2_PIN, GPIO_PIN_RESET);
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, -right);
    }
}

void motor_stop(void) {
    last_left = 0;
    last_right = 0;
    HAL_GPIO_WritePin(MOTOR_L_PORT, MOTOR_L_IN1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_L_PORT, MOTOR_L_IN2_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_R_PORT, MOTOR_R_IN1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_R_PORT, MOTOR_R_IN2_PIN, GPIO_PIN_RESET);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
}
