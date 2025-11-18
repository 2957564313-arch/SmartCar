#include "stm32f10x.h"                  // Device header
#include "Encoder.h"
#include "Serial.h"
#include "Delay.h"

typedef struct {
    int32_t count;          // 编码器计数值
    int32_t last_count;     // 上一次计数值
    int32_t total_count;    // 总计数（用于里程计算）
    int16_t speed;          // 速度值（脉冲/采样周期）
    uint8_t direction;      // 方向
} Encoder_TypeDef;

// 编码器全局变量
Encoder_TypeDef left_encoder = {0, 0, 0, 0, 0};
Encoder_TypeDef right_encoder = {0, 0, 0, 0, 0};


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
    // 左电机编码器 - TIM3 (PA6, PA7)
    GPIO_InitStructure.GPIO_Pin = LEFT_ENCODER_A_PIN | LEFT_ENCODER_B_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LEFT_ENCODER_A_PORT, &GPIO_InitStructure);
    
    // 右电机编码器 - TIM4 (PB6, PB7)
    GPIO_InitStructure.GPIO_Pin = RIGHT_ENCODER_A_PIN | RIGHT_ENCODER_B_PIN;
    GPIO_Init(RIGHT_ENCODER_A_PORT, &GPIO_InitStructure);
    
    // 配置TIM3为编码器模式
    TIM_TimeBaseStructure.TIM_Period = 65535;          // 最大计数值
    TIM_TimeBaseStructure.TIM_Prescaler = 0;           // 不分频
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 10;             // 滤波器
    TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    TIM_ICInit(TIM3, &TIM_ICInitStructure);
    
    // 配置TIM4为编码器模式
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    TIM_ICInit(TIM4, &TIM_ICInitStructure);
	
	TIM_Cmd(TIM3, ENABLE);
    TIM_Cmd(TIM4, ENABLE);
    
    // 清除计数器
    TIM_SetCounter(TIM3, 0);
    TIM_SetCounter(TIM4, 0);
    
    // 初始化编码器变量
    left_encoder.count = 0;
    left_encoder.last_count = 0;
    left_encoder.total_count = 0;
    left_encoder.speed = 0;
    left_encoder.direction = 0;
    
    right_encoder.count = 0;
    right_encoder.last_count = 0;
    right_encoder.total_count = 0;
    right_encoder.speed = 0;
    right_encoder.direction = 0;

}

/**
  * @brief  获取左编码器计数值
  * @retval 左编码器计数值
  */
int32_t Encoder_Get_Left_Count(void)
{
    int16_t current_count = TIM_GetCounter(TIM3);
    
    // 处理计数器溢出
    if((current_count < 0 && left_encoder.last_count > 30000) || 
       (current_count > 30000 && left_encoder.last_count < 0)) {
        // 溢出处理
        if(current_count < 0) {
            left_encoder.count += (65535 - left_encoder.last_count) + current_count;
        } else {
            left_encoder.count += current_count + (65535 - left_encoder.last_count);
        }
    } else {
        left_encoder.count += (current_count - left_encoder.last_count);
    }
    
    left_encoder.last_count = current_count;
    left_encoder.total_count = left_encoder.count;
    
    return left_encoder.count;
}

/**
  * @brief  获取右编码器计数值
  * @retval 右编码器计数值
  */
int32_t Encoder_Get_Right_Count(void)
{
    int16_t current_count = TIM_GetCounter(TIM4);
    
    // 处理计数器溢出
    if((current_count < 0 && right_encoder.last_count > 30000) || 
       (current_count > 30000 && right_encoder.last_count < 0)) {
        // 溢出处理
        if(current_count < 0) {
            right_encoder.count += (65535 - right_encoder.last_count) + current_count;
        } else {
            right_encoder.count += current_count + (65535 - right_encoder.last_count);
        }
    } else {
        right_encoder.count += (current_count - right_encoder.last_count);
    }
    
    right_encoder.last_count = current_count;
    right_encoder.total_count = right_encoder.count;
    
    return right_encoder.count;
}

/**
  * @brief  重置左编码器
  * @retval 无
  */
void Encoder_Reset_Left(void)
{
    TIM_SetCounter(TIM3, 0);
    left_encoder.count = 0;
    left_encoder.last_count = 0;
    left_encoder.total_count = 0;
    left_encoder.speed = 0;
}

/**
  * @brief  重置右编码器
  * @retval 无
  */
void Encoder_Reset_Right(void)
{
    TIM_SetCounter(TIM4, 0);
    right_encoder.count = 0;
    right_encoder.last_count = 0;
    right_encoder.total_count = 0;
    right_encoder.speed = 0;
}

/**
  * @brief  更新编码器速度
  * @retval 无
  */
void Encoder_Update_Speed(void)
{
    static uint32_t last_update_time = 0;
    uint32_t current_time = 0;
    
    // 获取当前时间（假设有系统时钟）
    // 这里需要根据实际的系统时钟来获取时间
    
    // 计算速度（脉冲/时间）
    left_encoder.speed = Encoder_Get_Left_Count();
    right_encoder.speed = Encoder_Get_Right_Count();
    
    // 重置计数用于下一次速度计算
    Encoder_Reset_Left();
    Encoder_Reset_Right();
}

/**
  * @brief  获取左编码器速度
  * @retval 左编码器速度
  */
int16_t Encoder_Get_Left_Speed(void)
{
    return left_encoder.speed;
}

/**
  * @brief  获取右编码器速度
  * @retval 右编码器速度
  */
int16_t Encoder_Get_Right_Speed(void)
{
    return right_encoder.speed;
}