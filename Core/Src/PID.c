#include "PID.h"

extern float error;
extern float prev_error;
extern float integral;
extern float pid_val;

void calc_pid(void) {
    float P = error;
    integral += error;
    if (integral >  MAX_I) integral =  MAX_I;
    if (integral < -MAX_I) integral = -MAX_I;
    float D = error - prev_error;
    prev_error = error;
    pid_val = KP * P + KI * integral + KD * D;
}
