#include "stm32f10x.h"
#include "stdlib.h"

static float pos_kp = 0.15f,pos_ki=0.0f,pos_kd = 1.0f;

int16_t Position_PID_Calculate(int32_t target, int32_t actual)
{
    
}

void Position_PID_SetParams(float p, float i, float d)
{
    pos_kp = p;
	pos_ki = i;
	pos_kd = d;
   
}
