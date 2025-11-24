#ifndef __LINE_TRACKING_H
#define __LINE_TRACKING_H

#include "stm32f10x.h"
#include "pid.h"

// 循迹参数配置
#define STRAIGHT_SPEED       50   
#define MAX_SPEED           75   
#define MIN_SPEED           20   
#define CROSS_DELAY         250  // 十字路口直行时间

// 传感器权重定义
#define WEIGHT_L2           -5    // 最左侧传感器权重
#define WEIGHT_L1           -3    // 左侧传感器权重  
#define WEIGHT_M            0     // 中间传感器权重
#define WEIGHT_R1           3     // 右侧传感器权重
#define WEIGHT_R2           5     // 最右侧传感器权重

// 全局变量声明
extern unsigned char lukou_num;

// 函数声明
void Track_Init(void);
void Advanced_Tracking(void);
uint8_t Line_Protection_Check(void);

#endif
