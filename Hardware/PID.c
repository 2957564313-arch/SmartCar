#include "stm32f10x.h"
#include "PID.h"

// 初始 PID 参数
PID_TypeDef line_pid = {8.0f, 0.1f, 3.0f, 0.0f, 0.0f};

/**
  * @brief  PID计算
  */
float PID_Calculate(PID_TypeDef *pid, float setpoint, float current_value)
{
    float error = setpoint - current_value;

    // 比例项
    float proportional = pid->kp * error;

    // 积分项
    pid->integral += error;
    if(pid->integral > 100.0f)  pid->integral = 100.0f;
    if(pid->integral < -100.0f) pid->integral = -100.0f;
    float integral = pid->ki * pid->integral;

    // 微分项
    float derivative = pid->kd * (error - pid->last_error);

    float output = proportional + integral + derivative;

    // 输出限幅：避免一次性拐太狠导致甩出线
    if(output > 35.0f)  output = 35.0f;
    if(output < -35.0f) output = -35.0f;

    pid->last_error = error;

    return output;
}

void PID_Reset(PID_TypeDef *pid)
{
    pid->integral = 0;
    pid->last_error = 0;
}
