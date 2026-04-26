#include "PID.h"

static float prev_error = 0;
static float integral = 0;
static float pid_val = 0;

void calc_pid(float error, uint8_t line_lost) {
    float P = error;

    if (!line_lost) {
        integral += error;
        if (integral >  MAX_I) integral =  MAX_I;
        if (integral < -MAX_I) integral = -MAX_I;
    }

    float D = error - prev_error;
    prev_error = error;
    pid_val = KP * P + KI * integral + KD * D;
}

float get_pid_output(void) {
    return pid_val;
}
