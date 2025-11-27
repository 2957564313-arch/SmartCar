#ifndef __LINE_TRACKING_H
#define __LINE_TRACKING_H

#include "stm32f10x.h"
#include "PID.h"

// ================== 速度参数（可根据实际微调） ==================
#define MAX_SPEED            100   // motor() 的最大速度（对应 PWM 100）
#define MIN_SPEED            0     // 最低速度（弯道内侧轮可降到 0）


// ================== 零点偏置 ==================
// 如果跑直线时总是在“黑线右边”那条白带里，可以把这个数调成负的（例如 -0.5f）

#define POSITION_OFFSET      0.0f

// ================== 传感器权重（对称分布） ==================
#define WEIGHT_L2           -4.0f    // 最左侧传感器权重
#define WEIGHT_L1           -2.0f    // 左侧传感器权重  
#define WEIGHT_M             0.0f    // 中间传感器权重
#define WEIGHT_R1            2.0f    // 右侧传感器权重
#define WEIGHT_R2            4.0f    // 最右侧传感器权重


// 十字路口计数
extern unsigned char lukou_num;
extern int STRAIGHT_SPEED;
extern int CORNER_SPEED_MIN;
extern float SPEED_K;

// 函数声明
void Track_Init(void);
void Advanced_Tracking(void);

#endif
