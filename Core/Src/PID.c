#include "PID.h"

static float prev_error = 0.0f;
static float integral = 0.0f;
static float pid_val = 0.0f;
static float d_filtered = 0.0f;

void calc_pid(float error, uint8_t line_lost) {
    if (line_lost) {
        integral = 0.0f;
        pid_val = 0.0f;
        return;
    }

    integral += error;
    if (integral >  MAX_I) integral =  MAX_I;
    if (integral < -MAX_I) integral = -MAX_I;

    float d_raw = error - prev_error;
    prev_error = error;

    // 一阶低通滤波抑制D项高频噪声
    d_filtered = PID_D_FILTER_ALPHA * d_raw + (1.0f - PID_D_FILTER_ALPHA) * d_filtered;

    pid_val = KP * error + KI * integral + KD * d_filtered;
    if (pid_val > PID_OUTPUT_LIMIT) pid_val = PID_OUTPUT_LIMIT;
    if (pid_val < -PID_OUTPUT_LIMIT) pid_val = -PID_OUTPUT_LIMIT;
}

float get_pid_output(void) {
    return pid_val;
}

void pid_reset(void) {
    prev_error = 0.0f;
    integral = 0.0f;
    pid_val = 0.0f;
    d_filtered = 0.0f;
}
