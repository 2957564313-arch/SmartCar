#include "stm32f10x.h"                  
#include "Motor.h"
#include <stdlib.h>

/**
  * @brief  电机初始化 - 提高PWM频率
  */
void Motor_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    
    // 开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    
    // 初始化电机控制引脚
    GPIO_InitStructure.GPIO_Pin = LEFT_MOTOR_AIN1_PIN | LEFT_MOTOR_AIN2_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LEFT_MOTOR_AIN1_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = RIGHT_MOTOR_BIN1_PIN | RIGHT_MOTOR_BIN2_PIN;
    GPIO_Init(RIGHT_MOTOR_BIN1_PORT, &GPIO_InitStructure);
    
    // 初始化PWM引脚
    GPIO_InitStructure.GPIO_Pin = LEFT_MOTOR_PWMA_PIN | RIGHT_MOTOR_PWMB_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 提高PWM频率到10kHz，提高电机响应
    TIM_TimeBaseStructure.TIM_Period = 100 - 1;        // 100级
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;      // 72MHz/72 = 1MHz, 1MHz/100 = 10kHz
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    
    // PWM模式配置
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    
    // 通道3 - 左电机PWM
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OC3Init(TIM2, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
    
    // 通道4 - 右电机PWM
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OC4Init(TIM2, &TIM_OCInitStructure);
    TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);
    
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
}

/**
  * @brief  设置电机速度 - 优化响应
  */
void motor(int left_speed, int right_speed)
{
    // 限制速度范围
    if(left_speed > 100) left_speed = 100;
    if(left_speed < -100) left_speed = -100;
    if(right_speed > 100) right_speed = 100;
    if(right_speed < -100) right_speed = -100;
    
    // 转换为PWM值 (0-99)
    int left_pwm = abs(left_speed);
    int right_pwm = abs(right_speed);
    
    // 左电机控制
    if(left_speed >= 0) {
        GPIO_SetBits(LEFT_MOTOR_AIN1_PORT, LEFT_MOTOR_AIN1_PIN);
        GPIO_ResetBits(LEFT_MOTOR_AIN2_PORT, LEFT_MOTOR_AIN2_PIN);
        TIM_SetCompare3(TIM2, left_pwm);
    } else {
        GPIO_ResetBits(LEFT_MOTOR_AIN1_PORT, LEFT_MOTOR_AIN1_PIN);
        GPIO_SetBits(LEFT_MOTOR_AIN2_PORT, LEFT_MOTOR_AIN2_PIN);
        TIM_SetCompare3(TIM2, left_pwm);
    }
    
    // 右电机控制
    if(right_speed >= 0) {
        GPIO_SetBits(RIGHT_MOTOR_BIN1_PORT, RIGHT_MOTOR_BIN2_PIN);
        GPIO_ResetBits(RIGHT_MOTOR_BIN2_PORT, RIGHT_MOTOR_BIN1_PIN);
        TIM_SetCompare4(TIM2, right_pwm);
    } else {
        GPIO_ResetBits(RIGHT_MOTOR_BIN1_PORT, RIGHT_MOTOR_BIN2_PIN);
        GPIO_SetBits(RIGHT_MOTOR_BIN2_PORT, RIGHT_MOTOR_BIN1_PIN);
        TIM_SetCompare4(TIM2, right_pwm);
    }
}

void Motor_Stop(void)
{
    GPIO_ResetBits(LEFT_MOTOR_AIN1_PORT, LEFT_MOTOR_AIN1_PIN);
    GPIO_ResetBits(LEFT_MOTOR_AIN2_PORT, LEFT_MOTOR_AIN2_PIN);
    GPIO_ResetBits(RIGHT_MOTOR_BIN1_PORT, RIGHT_MOTOR_BIN1_PIN);
    GPIO_ResetBits(RIGHT_MOTOR_BIN2_PORT, RIGHT_MOTOR_BIN2_PIN);
    TIM_SetCompare3(TIM2, 0);
    TIM_SetCompare4(TIM2, 0);
}
