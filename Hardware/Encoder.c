#include "Encoder.h"

Encoder_TypeDef left_encoder = {0, 0};
Encoder_TypeDef right_encoder = {0, 0};

/**
  * @brief  编码器初始化
  */
void Encoder_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    
    // 开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);  // TIM1在APB2上
    
    // 初始化左编码器引脚 (PA6, PA7) - TIM3
    GPIO_InitStructure.GPIO_Pin = LEFT_ENCODER_A_PIN | LEFT_ENCODER_B_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LEFT_ENCODER_A_PORT, &GPIO_InitStructure);
    
    // 初始化右编码器引脚 (PA8, PA9) - TIM1
    GPIO_InitStructure.GPIO_Pin = RIGHT_ENCODER_A_PIN | RIGHT_ENCODER_B_PIN;
    GPIO_Init(RIGHT_ENCODER_A_PORT, &GPIO_InitStructure);
    
    // 配置TIM3为编码器模式（左电机）
    TIM_TimeBaseStructure.TIM_Period = 65535;
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 10;
    TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    TIM_ICInit(TIM3, &TIM_ICInitStructure);
    
    // 配置TIM1为编码器模式（右电机）- 使用PA8和PA9
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
    TIM_EncoderInterfaceConfig(TIM1, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    TIM_ICInit(TIM1, &TIM_ICInitStructure);
    
    // 启动编码器
    TIM_Cmd(TIM3, ENABLE);
    TIM_Cmd(TIM1, ENABLE);
}

/**
  * @brief  获取左编码器计数值
  */
int32_t Encoder_Get_Left_Count(void)
{
    left_encoder.count = (int32_t)TIM_GetCounter(TIM3);
    return left_encoder.count;
}

/**
  * @brief  获取右编码器计数值
  */
int32_t Encoder_Get_Right_Count(void)
{
    right_encoder.count = (int32_t)TIM_GetCounter(TIM1);
    return right_encoder.count;
}

/**
  * @brief  重置两个编码器
  */
void Encoder_Reset_Both(void)
{
    TIM_SetCounter(TIM3, 0);
    TIM_SetCounter(TIM1, 0);
    left_encoder.count = 0;
    right_encoder.count = 0;
}
