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
    uint8_t mask;       // bit0=S1 ... bit5=S6, 1=black line detected
    uint8_t raw_mask;   // unfiltered GPIO sample
    uint8_t count;      // active sensor count
    uint8_t line_lost;  // no sensor sees the line
    uint8_t wide;       // crossing or very wide line
    uint8_t split;      // non-contiguous black areas
    float error;        // left negative, right positive
} TraceState;

#define TRACE_FILTER_MAX       5U
#define TRACE_FILTER_ON_LEVEL  4U
#define TRACE_FILTER_OFF_LEVEL 1U
#define TRACE_LOST_HOLD_CYCLES 3U

float trace_get_error(void);
uint8_t trace_is_line_lost(void);
float trace_get_last_error(void);
uint8_t trace_get_raw_mask(void);
uint8_t trace_get_mask(void);
uint8_t trace_get_count(void);
const TraceState *trace_get_state(void);

#endif /* __TRACE_H */
