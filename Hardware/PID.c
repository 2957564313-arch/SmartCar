#include "stm32f10x.h"                  // Device header
#include "PID.h"
#include "Serial.h"

/**
  * @brief  PID初始化
  * @param  pid: PID结构体指针
  * @param  kp: 比例系数
  * @param  ki: 积分系数
  * @param  kd: 微分系数
  * @param  max: 输出上限
  * @param  min: 输出下限
  * @param  integral_max: 积分限幅
  * @retval 无
  */
void PID_Init(PID_TypeDef *pid, float kp, float ki, float kd, float max, float min, float integral_max)
{
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->target = 0;
    pid->integral = 0;
    pid->last_error = 0;
    pid->output = 0;
    pid->output_max = max;
    pid->output_min = min;
    pid->integral_max = integral_max;
}

/**
  * @brief  PID计算
  * @param  pid: PID结构体指针
  * @param  current_value: 当前值
  * @retval PID输出值
  */
float PID_Calculate(PID_TypeDef *pid, float current_value)
{
    float error = pid->target - current_value;
    
    // 比例项
    float proportional = pid->kp * error;
    
    // 积分项（带限幅）
    pid->integral += error;
    if(pid->integral > pid->integral_max) pid->integral = pid->integral_max;
    if(pid->integral < -pid->integral_max) pid->integral = -pid->integral_max;
    float integral = pid->ki * pid->integral;
    
    // 微分项
    float derivative = pid->kd * (error - pid->last_error);
    
    // 计算输出
    pid->output = proportional + integral + derivative;
    
    // 输出限幅
    if(pid->output > pid->output_max)
        pid->output = pid->output_max;
    else if(pid->output < pid->output_min)
        pid->output = pid->output_min;
    
    pid->last_error = error;
    
    return pid->output;
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
    pid->output = 0;
}

/**
  * @brief  设置PID参数
  * @param  pid: PID结构体指针
  * @param  kp: 比例系数
  * @param  ki: 积分系数
  * @param  kd: 微分系数
  * @retval 无
  */
void PID_Set_Parameters(PID_TypeDef *pid, float kp, float ki, float kd)
{
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    
}
