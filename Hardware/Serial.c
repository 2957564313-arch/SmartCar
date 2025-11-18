#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>
#include "Serial.h"

void USART1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    
    // 开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
    
    // 配置TX引脚 (PA9) - 复用推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 配置RX引脚 (PA10) - 浮空输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 串口配置
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);
    
    // 启动串口
    USART_Cmd(USART1, ENABLE);
}

/**
  * @brief  发送一个字节
  * @param  byte: 要发送的字节
  */
void USART1_SendByte(uint8_t byte)
{
    USART_SendData(USART1, byte);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

/**
  * @brief  发送字符串
  * @param  str: 要发送的字符串
  */
void USART1_SendString(char *str)
{
    while(*str)
    {
        USART1_SendByte(*str++);
    }
}

/**
  * @brief  重定向printf到串口
  */
int fputc(int ch, FILE *f)
{
    USART_SendData(USART1, (uint8_t)ch);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    return ch;
}
