#ifndef __TRACE_H
#define __TRACE_H

#include "main.h"

// 6路循迹传感器引脚定义 (高电平=检测到黑线)
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

#define TRACE_SENSOR_COUNT 6U

typedef struct {
    uint8_t mask;
    uint8_t raw_mask;
    uint8_t count;
    uint8_t line_lost;
    uint8_t wide;
    uint8_t split;
    float error;
} TraceState;

// 无灯红外传感器需要更深的滤波深度
#define TRACE_FILTER_MAX       8U
#define TRACE_FILTER_ON_LEVEL  6U
#define TRACE_FILTER_OFF_LEVEL 2U
#define TRACE_LOST_HOLD_CYCLES 5U

// 单次调用内多次采样取多数 (抗瞬态干扰)
#define TRACE_OVERSAMPLE       3U

float trace_get_error(void);
uint8_t trace_is_line_lost(void);
float trace_get_last_error(void);
uint8_t trace_get_raw_mask(void);
uint8_t trace_get_mask(void);
uint8_t trace_get_count(void);
const TraceState *trace_get_state(void);

#endif /* __TRACE_H */
