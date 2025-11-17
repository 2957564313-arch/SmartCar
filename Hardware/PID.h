#ifndef __PID_H
#define __PID_H

void Position_PID_SetParams(float p, float i, float d);
int16_t Position_PID_Calculate(int32_t target, int32_t actual);


#endif
