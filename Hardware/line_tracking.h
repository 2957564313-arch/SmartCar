#ifndef __LINE_TRACKING_H
#define __LINE_TRACKING_H

#include "stm32f10x.h"
#include "pid.h"

// 循迹参数配置 - 激进参数
#define STRAIGHT_SPEED       40   // 进一步降低速度确保控制
#define MAX_SPEED           60   
#define MIN_SPEED           10   // 允许更低的转弯速度
#define CROSS_DELAY         200  // 缩短十字路口时间

// 全局变量声明
extern unsigned char lukou_num;
extern unsigned char last_line_status;

// 函数声明
void Track_Init(void);
void Advanced_Tracking(void);
uint8_t Line_Protection_Check(void);

#endif
