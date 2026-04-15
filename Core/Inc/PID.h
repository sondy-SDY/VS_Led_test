#ifndef __PID_H
#define __PID_H

// PID参数
#define KP    80.0f
#define KI    0.1f
#define KD    200.0f
#define MAX_I 100.0f

void calc_pid(void);

#endif /* __PID_H */
