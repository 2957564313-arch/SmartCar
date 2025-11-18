#ifndef __ENCODER_H
#define __ENCODER_H

#include "stm32f10x.h"                  // Device header

// 编码器结构体
typedef struct {
    int32_t count;      // 编码器计数值
    int16_t speed;      // 速度值
} Encoder_TypeDef;

// 编码器引脚定义
#define LEFT_ENCODER_A_PIN     GPIO_Pin_6
#define LEFT_ENCODER_A_PORT    GPIOA
#define LEFT_ENCODER_B_PIN     GPIO_Pin_7
#define LEFT_ENCODER_B_PORT    GPIOA

#define RIGHT_ENCODER_A_PIN    GPIO_Pin_6
#define RIGHT_ENCODER_A_PORT   GPIOB
#define RIGHT_ENCODER_B_PIN    GPIO_Pin_7
#define RIGHT_ENCODER_B_PORT   GPIOB

// 全局编码器实例
extern Encoder_TypeDef left_encoder;
extern Encoder_TypeDef right_encoder;

// 函数声明
void Encoder_Init(void);
int32_t Encoder_Get_Left_Count(void);
int32_t Encoder_Get_Right_Count(void);
void Encoder_Reset_Both(void);

#endif
