#ifndef __PID_H
#define __PID_H

#include <stdint.h>

// PD parameters for line tracking. Start with KI disabled for easier tuning.
#define KP    85.0f
#define KI    0.0f
#define KD    55.0f
#define MAX_I 80.0f
#define PID_OUTPUT_LIMIT 620.0f

void calc_pid(float error, uint8_t line_lost);
float get_pid_output(void);

#endif /* __PID_H */
