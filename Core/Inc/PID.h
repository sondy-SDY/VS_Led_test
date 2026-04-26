#ifndef __PID_H
#define __PID_H

#include <stdint.h>

// PID参数 (需根据实际赛道调试)
#define KP    80.0f
#define KI    0.1f
#define KD    40.0f
#define MAX_I 100.0f

void calc_pid(float error, uint8_t line_lost);
float get_pid_output(void);

#endif /* __PID_H */
