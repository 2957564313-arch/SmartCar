#include "stm32f10x.h"                  
#include "Sensor.h"
#include "Delay.h"

// 传感器状态全局变量
int L2 = 0, L1 = 0, M = 0, R1 = 0, R2 = 0;

/**
  * @brief  传感器初始化
  */
void Sensor_Init(void)
{
    // 开启GPIOA时钟和AFIO时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 禁用JTAG，释放PA15、PB3、PB4作为普通IO
    // GPIO_Remap_SWJ_JTAGDisable: JTAG-DP禁用 + SW-DP使能
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    
    // 配置传感器引脚为输入模式
    GPIO_InitStructure.GPIO_Pin = L2_PIN | L1_PIN | M_PIN | R1_PIN | R2_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 上拉输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
}

/**
  * @brief  读取传感器状态
  * 遇到白色时亮灯输出低电平(0)，遇到黑色时灭灯输出高电平(1)
  */
void Sensor_Read(void)
{
    // 读取传感器状态（0:检测到白色，1:检测到黑色）
    L2 = GPIO_ReadInputDataBit(L2_PORT, L2_PIN);
    L1 = GPIO_ReadInputDataBit(L1_PORT, L1_PIN);
    M = GPIO_ReadInputDataBit(M_PORT, M_PIN);
    R1 = GPIO_ReadInputDataBit(R1_PORT, R1_PIN);
    R2 = GPIO_ReadInputDataBit(R2_PORT, R2_PIN);
}

