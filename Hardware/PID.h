#ifndef __PID_H
#define __PID_H

// 简化版PID结构体
typedef struct {
    float kp;           // 比例系数
    float ki;           // 积分系数  
    float kd;           // 微分系数
    float integral;     // 积分项
    float last_error;   // 上次误差
} PID_TypeDef;

// 全局PID实例
extern PID_TypeDef line_pid;

// 函数声明
float PID_Calculate(PID_TypeDef *pid, float setpoint, float current_value);
void PID_Reset(PID_TypeDef *pid);

#endif
