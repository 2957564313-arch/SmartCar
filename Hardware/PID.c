#include "stm32f10x.h"
#include "PID.h"

// 优化PID参数 - 提高响应速度
PID_TypeDef line_pid = {10.0f, 0.3f, 3.5f, 0.0f, 0.0f};

/**
  * @brief  PID计算 - 优化响应
  */
float PID_Calculate(PID_TypeDef *pid, float setpoint, float current_value)
{
    float error = setpoint - current_value;
    
    // 强比例项
    float proportional = pid->kp * error;
    
    // 积分项 - 限制更严格
    pid->integral += error;
    if(pid->integral > 15.0f) pid->integral = 15.0f;
    if(pid->integral < -15.0f) pid->integral = -15.0f;
    float integral = pid->ki * pid->integral;
    
    // 微分项
    float derivative = pid->kd * (error - pid->last_error);
    
    float output = proportional + integral + derivative;
    
    // 输出限幅 - 放宽限制
    if(output > 40.0f) output = 40.0f;
    if(output < -40.0f) output = -40.0f;
    
    pid->last_error = error;
    
    return output;
}

void PID_Reset(PID_TypeDef *pid)
{
    pid->integral = 0;
    pid->last_error = 0;
}
