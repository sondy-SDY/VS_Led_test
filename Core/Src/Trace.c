#include "Trace.h"

static TraceState state = {0};
static float last_error = 0.0f;
static uint8_t filter_level[TRACE_SENSOR_COUNT] = {0};
static uint8_t stable_mask = 0;
static uint8_t lost_hold_count = TRACE_LOST_HOLD_CYCLES;

static uint8_t read_raw_mask(void)
{
    uint8_t mask = 0;

    if (HAL_GPIO_ReadPin(S1_PORT, S1_PIN) == GPIO_PIN_SET) mask |= (uint8_t)(1U << 0);
    if (HAL_GPIO_ReadPin(S2_PORT, S2_PIN) == GPIO_PIN_SET) mask |= (uint8_t)(1U << 1);
    if (HAL_GPIO_ReadPin(S3_PORT, S3_PIN) == GPIO_PIN_SET) mask |= (uint8_t)(1U << 2);
    if (HAL_GPIO_ReadPin(S4_PORT, S4_PIN) == GPIO_PIN_SET) mask |= (uint8_t)(1U << 3);
    if (HAL_GPIO_ReadPin(S5_PORT, S5_PIN) == GPIO_PIN_SET) mask |= (uint8_t)(1U << 4);
    if (HAL_GPIO_ReadPin(S6_PORT, S6_PIN) == GPIO_PIN_SET) mask |= (uint8_t)(1U << 5);

    return mask;
}

static uint8_t filter_raw_mask(uint8_t raw_mask)
{
    uint8_t mask = stable_mask;

    for (uint8_t i = 0; i < TRACE_SENSOR_COUNT; i++) {
        uint8_t bit = (uint8_t)(1U << i);

        if ((raw_mask & bit) != 0U) {
            if (filter_level[i] < TRACE_FILTER_MAX) {
                filter_level[i]++;
            }
        } else if (filter_level[i] > 0U) {
            filter_level[i]--;
        }

        if (filter_level[i] >= TRACE_FILTER_ON_LEVEL) {
            mask |= bit;
        } else if (filter_level[i] <= TRACE_FILTER_OFF_LEVEL) {
            mask &= (uint8_t)~bit;
        }
    }

    stable_mask = mask;
    return stable_mask;
}

static uint8_t count_black_groups(uint8_t mask)
{
    uint8_t groups = 0;
    uint8_t in_group = 0;

    for (uint8_t i = 0; i < TRACE_SENSOR_COUNT; i++) {
        if ((mask & (uint8_t)(1U << i)) != 0U) {
            if (!in_group) {
                groups++;
                in_group = 1;
            }
        } else {
            in_group = 0;
        }
    }

    return groups;
}

static int pick_error_from_mask(uint8_t mask, float reference_error)
{
    const int weights[TRACE_SENSOR_COUNT] = {-5, -3, -1, 1, 3, 5};
    int best_weight = 0;
    float best_distance = 100.0f;

    for (uint8_t i = 0; i < TRACE_SENSOR_COUNT; i++) {
        if ((mask & (uint8_t)(1U << i)) != 0U) {
            float distance = (float)weights[i] - reference_error;
            if (distance < 0.0f) {
                distance = -distance;
            }
            if (distance < best_distance) {
                best_distance = distance;
                best_weight = weights[i];
            }
        }
    }

    return best_weight;
}

// 读取6路传感器，返回误差值 (权重: 左负右正)
float trace_get_error(void) {
    const int weights[TRACE_SENSOR_COUNT] = {-5, -3, -1, 1, 3, 5};
    int sum = 0;
    uint8_t count = 0;
    uint8_t raw_mask = read_raw_mask();
    uint8_t mask = filter_raw_mask(raw_mask);

    for (uint8_t i = 0; i < TRACE_SENSOR_COUNT; i++) {
        if ((mask & (uint8_t)(1U << i)) != 0U) {
            sum += weights[i];
            count++;
        }
    }

    state.raw_mask = raw_mask;
    state.mask = mask;
    state.count = count;
    state.wide = 0;
    state.split = 0;

    if (count == 0) {
        if (lost_hold_count < TRACE_LOST_HOLD_CYCLES) {
            lost_hold_count++;
        }
        state.line_lost = (lost_hold_count >= TRACE_LOST_HOLD_CYCLES) ? 1U : 0U;
        state.error = last_error;
        return last_error;
    }

    lost_hold_count = 0;
    state.line_lost = 0;

    uint8_t groups = count_black_groups(mask);
    state.split = (groups >= 2U) ? 1U : 0U;
    state.wide = (count >= 4U) ? 1U : 0U;

    if (state.split) {
        last_error = (float)pick_error_from_mask(mask, last_error);
    } else {
        last_error = (float)sum / count;
    }

    state.error = last_error;
    return last_error;
}

uint8_t trace_is_line_lost(void) {
    return state.line_lost;
}

float trace_get_last_error(void) {
    return last_error;
}

uint8_t trace_get_raw_mask(void) {
    return state.raw_mask;
}

uint8_t trace_get_mask(void) {
    return state.mask;
}

uint8_t trace_get_count(void) {
    return state.count;
}

const TraceState *trace_get_state(void) {
    return &state;
}
