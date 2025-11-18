#include "Encoder.h"

Encoder_TypeDef left_encoder = {0, 0};
Encoder_TypeDef right_encoder = {0, 0};

/**
  * @brief  编码器初始化
  * @retval 无
  */
void Encoder_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    
    // 开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM4, ENABLE);
    
    // 初始化编码器引脚
    GPIO_InitStructure.GPIO_Pin = LEFT_ENCODER_A_PIN | LEFT_ENCODER_B_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LEFT_ENCODER_A_PORT, &GPIO_InitStructure);
    
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
    
    // 配置TIM4为编码器模式（右电机）
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    TIM_ICInit(TIM4, &TIM_ICInitStructure);
    
    // 启动编码器
    TIM_Cmd(TIM3, ENABLE);
    TIM_Cmd(TIM4, ENABLE);
}

/**
  * @brief  获取左编码器计数值
  * @retval 左编码器计数值
  */
int32_t Encoder_Get_Left_Count(void)
{
    left_encoder.count = (int32_t)TIM_GetCounter(TIM3);
    return left_encoder.count;
}

/**
  * @brief  获取右编码器计数值
  * @retval 右编码器计数值
  */
int32_t Encoder_Get_Right_Count(void)
{
    right_encoder.count = (int32_t)TIM_GetCounter(TIM4);
    return right_encoder.count;
}

/**
  * @brief  重置两个编码器
  * @retval 无
  */
void Encoder_Reset_Both(void)
{
    TIM_SetCounter(TIM3, 0);
    TIM_SetCounter(TIM4, 0);
    left_encoder.count = 0;
    right_encoder.count = 0;
}
