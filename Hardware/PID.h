#ifndef __PID_H
#define __PID_H

// PID结构体
typedef struct {
    float kp;           // 比例系数
    float ki;           // 积分系数  
    float kd;           // 微分系数
    float target;     // 目标值
    float integral;     // 积分项
    float last_error;   // 上次误差
    float output;       // 输出值
    float output_max;   // 输出上限
    float output_min;   // 输出下限
    float integral_max; // 积分限幅
} PID_TypeDef;

// 函数声明
void PID_Init(PID_TypeDef *pid, float kp, float ki, float kd, float max, float min, float integral_max);
float PID_Calculate(PID_TypeDef *pid, float current_value);
void PID_Reset(PID_TypeDef *pid);
void PID_Set_Parameters(PID_TypeDef *pid, float kp, float ki, float kd);

#endif
