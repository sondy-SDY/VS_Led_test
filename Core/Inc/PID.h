#ifndef __PID_H
#define __PID_H

#include <stdint.h>

#define KP    80.0f
#define KI    0.0f
#define KD    60.0f
#define MAX_I 80.0f
#define PID_OUTPUT_LIMIT 620.0f

// D项低通滤波系数 (0~1, 越小滤波越强)
#define PID_D_FILTER_ALPHA 0.4f

void calc_pid(float error, uint8_t line_lost);
float get_pid_output(void);
void pid_reset(void);

#endif /* __PID_H */
