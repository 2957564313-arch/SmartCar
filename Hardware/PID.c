#include "stm32f10x.h"

static float Target,Actual,Out;		//目标值，实际值，输出值
static float kp = 0.15f,ki=0.0f,kd = 1.0f;		//比例项，积分项，微分项
static float Error0,Error1,ErrorInt;			//本次误差，上次误差，误差积分

int16_t Position_PID_Calculate(int32_t target, int32_t actual)
{
   Actual = ;
	
	Error1 = Error0;
	Error0 = Target - Actual;
	
	ErrorInt += Error0;
	
	Out = kp*Error0 + ki*ErrorInt + kd*(Error0 - Error1);
	
	if(Out> ) Out = ;
	if (Out < ) Out = ;
	
	return (int16_t)Out;
}

void Position_PID_SetParams(float p, float i, float d)
{
    kp = p;
	ki = i;
	kd = d;
   
}
