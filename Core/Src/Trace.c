#include "Trace.h"

extern float prev_error;

// 读取6路传感器，返回误差值 (权重: 左负右正)
float trace_get_error(void) {
    int s[6];
    s[0] = (HAL_GPIO_ReadPin(S1_PORT, S1_PIN) == GPIO_PIN_RESET) ? 1 : 0;
    s[1] = (HAL_GPIO_ReadPin(S2_PORT, S2_PIN) == GPIO_PIN_RESET) ? 1 : 0;
    s[2] = (HAL_GPIO_ReadPin(S3_PORT, S3_PIN) == GPIO_PIN_RESET) ? 1 : 0;
    s[3] = (HAL_GPIO_ReadPin(S4_PORT, S4_PIN) == GPIO_PIN_RESET) ? 1 : 0;
    s[4] = (HAL_GPIO_ReadPin(S5_PORT, S5_PIN) == GPIO_PIN_RESET) ? 1 : 0;
    s[5] = (HAL_GPIO_ReadPin(S6_PORT, S6_PIN) == GPIO_PIN_RESET) ? 1 : 0;

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
