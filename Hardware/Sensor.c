#include "stm32f10x.h"                  // Device header
#include "sensor.h"
#include "Delay.h"

// 传感器状态全局变量
int L2 = 0, L1 = 0, M = 0, R1 = 0, R2 = 0;

/**
  * @brief  传感器初始化
  */
void Sensor_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 开启GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // 配置传感器引脚为输入模式
    GPIO_InitStructure.GPIO_Pin = L2_PIN | L1_PIN | M_PIN | R1_PIN | R2_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 上拉输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/**
  * @brief  读取传感器状态
  */
void Sensor_Read(void)
{
    // 读取传感器状态（0:检测到黑线，1:检测到白色）
    L2 = GPIO_ReadInputDataBit(L2_PORT, L2_PIN);
    L1 = GPIO_ReadInputDataBit(L1_PORT, L1_PIN);
    M = GPIO_ReadInputDataBit(M_PORT, M_PIN);
    R1 = GPIO_ReadInputDataBit(R1_PORT, R1_PIN);
    R2 = GPIO_ReadInputDataBit(R2_PORT, R2_PIN);
}

/**
  * @brief  获取循线状态
  * @return 循线状态码
  */
unsigned char Get_Line_Status(void)
{
    // 针对18mm窄线的状态判断
    if(L2 == 0 && L1 == 1 && M == 1 && R1 == 1 && R2 == 1) return 1;      // 00011 - 严重偏左
    else if(L2 == 0 && L1 == 0 && M == 1 && R1 == 1 && R2 == 1) return 2; // 00111 - 偏左
    else if(L2 == 1 && L1 == 0 && M == 1 && R1 == 1 && R2 == 1) return 3; // 01011 - 轻微偏左
    else if(L2 == 1 && L1 == 0 && M == 0 && R1 == 1 && R2 == 1) return 4; // 01101 - 正常偏左
    else if(L2 == 1 && L1 == 1 && M == 0 && R1 == 1 && R2 == 1) return 5; // 10101 - 居中
    else if(L2 == 1 && L1 == 1 && M == 0 && R1 == 0 && R2 == 1) return 6; // 11001 - 正常偏右
    else if(L2 == 1 && L1 == 1 && M == 1 && R1 == 0 && R2 == 1) return 7; // 11010 - 轻微偏右
    else if(L2 == 1 && L1 == 1 && M == 1 && R1 == 0 && R2 == 0) return 8; // 11100 - 偏右
    else if(L2 == 1 && L1 == 1 && M == 1 && R1 == 1 && R2 == 0) return 9; // 11100 - 严重偏右
    else if(L2 == 1 && L1 == 1 && M == 1 && R1 == 1 && R2 == 1) return 10; // 11111 - 丢失路线
    else if(L2 == 0 && L1 == 0 && M == 0 && R1 == 0 && R2 == 0) return 11; // 00000 - 全在线(十字路口)
    else return 0; // 未知状态
}

/**
  * @brief  检测十字路口
  * @return 1-十字路口, 0-非十字路口
  */
unsigned char Detect_Crossroad(void)
{
    unsigned char sensor_count = 0;
    
    // 计算检测到黑线的传感器数量
    if(L2 == 0) sensor_count++;
    if(L1 == 0) sensor_count++;
    if(M == 0) sensor_count++;
    if(R1 == 0) sensor_count++;
    if(R2 == 0) sensor_count++;
    
    // 对于18mm窄线，3个以上传感器同时检测到线认为是十字路口
    if(sensor_count >= 3) {
        return 1;
    }
    
    return 0;
}