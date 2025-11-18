#ifndef __ENCODER_H
#define __ENCODER_H

// 编码器引脚定义
// 左电机编码器 - TIM3
#define LEFT_ENCODER_A_PIN     GPIO_Pin_6
#define LEFT_ENCODER_A_PORT    GPIOA
#define LEFT_ENCODER_B_PIN     GPIO_Pin_7
#define LEFT_ENCODER_B_PORT    GPIOA

// 右电机编码器 - TIM4
#define RIGHT_ENCODER_A_PIN    GPIO_Pin_6
#define RIGHT_ENCODER_A_PORT   GPIOB
#define RIGHT_ENCODER_B_PIN    GPIO_Pin_7
#define RIGHT_ENCODER_B_PORT   GPIOB

// 全局变量声明
extern Encoder_TypeDef left_encoder;
extern Encoder_TypeDef right_encoder;


// 函数声明
void Encoder_Init(void);
int32_t Encoder_Get_Left_Count(void);
int32_t Encoder_Get_Right_Count(void);
void Encoder_Reset_Left(void);
void Encoder_Reset_Right(void);
void Encoder_Update_Speed(void);
int16_t Encoder_Get_Left_Speed(void);
int16_t Encoder_Get_Right_Speed(void);

#endif