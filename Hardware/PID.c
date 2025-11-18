#include "stm32f10x.h"
#include "PID.h"

// 初始化PID参数
PID_TypeDef line_pid = {3.0f, 0.05f, 0.8f, 0.0f, 0.0f};

/**
  * @brief  PID计算
  * @param  pid: PID结构体指针
  * @param  setpoint: 目标值
  * @param  current_value: 当前值
  * @retval PID输出值
  */
float PID_Calculate(PID_TypeDef *pid, float setpoint, float current_value)
{
    float error = setpoint - current_value;
    
    // 比例项
    float proportional = pid->kp * error;
    
    // 积分项（简单限幅）
    pid->integral += error;
    if(pid->integral > 100.0f) pid->integral = 100.0f;
    if(pid->integral < -100.0f) pid->integral = -100.0f;
    float integral = pid->ki * pid->integral;
    
    // 微分项
    float derivative = pid->kd * (error - pid->last_error);
    
    // 计算输出
    float output = proportional + integral + derivative;
    
    // 输出限幅
    if(output > 50.0f) output = 50.0f;
    if(output < -50.0f) output = -50.0f;
    
    pid->last_error = error;
    
    return output;
}

/**
  * @brief  重置PID控制器
  * @param  pid: PID结构体指针
  * @retval 无
  */
void PID_Reset(PID_TypeDef *pid)
{
    pid->integral = 0;
    pid->last_error = 0;
}
