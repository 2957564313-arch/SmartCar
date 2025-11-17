#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>

uint8_t Serial_RxData;
uint8_t Serial_RxFlag;

void Serial_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruture;
	GPIO_InitStruture.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruture.GPIO_Pin =GPIO_Pin_9 ;
	GPIO_InitStruture.GPIO_Speed =GPIO_Speed_50MHz ;
	GPIO_Init(GPIOA,&GPIO_InitStruture);
	
	GPIO_InitStruture.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruture.GPIO_Pin =GPIO_Pin_10 ;
	GPIO_InitStruture.GPIO_Speed =GPIO_Speed_50MHz ;
	GPIO_Init(GPIOA,&GPIO_InitStruture);
	
	USART_InitTypeDef USART_InitStruture;
	USART_InitStruture.USART_BaudRate =9600 ;
	USART_InitStruture.USART_HardwareFlowControl =USART_HardwareFlowControl_None ;
	USART_InitStruture.USART_Mode =USART_Mode_Tx|USART_Mode_Rx ;
	USART_InitStruture.USART_Parity = USART_Parity_No;
	USART_InitStruture.USART_StopBits = USART_StopBits_1;
	USART_InitStruture.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1,&USART_InitStruture);
	
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStruture;
	NVIC_InitStruture.NVIC_IRQChannel =USART1_IRQn ;
	NVIC_InitStruture.NVIC_IRQChannelCmd =ENABLE ;
	NVIC_InitStruture.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruture.NVIC_IRQChannelSubPriority =1 ;
	NVIC_Init(&NVIC_InitStruture);
	
	USART_Cmd(USART1,ENABLE);
	
}

uint8_t Serial_GetRxFlag(void)
{
	if(Serial_RxFlag == 1)
	{
		Serial_RxFlag = 0;
		return 1;
	}
	return 0;
}

uint8_t Serial_GetRxData(void)
{
	return Serial_RxData;
}


void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET )
	{
		Serial_RxData = USART_ReceiveData(USART1);
		Serial_RxFlag = 1;
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}
}

void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART1,Byte);	//发送以后还要等待一下标志位
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);		//这里的标志位无需手动清零,会自动清零
	
}

int fputc(int ch,FILE *f)		//参数照这样写就好了，不用管那么多
{
	Serial_SendByte(ch);
	return ch;
}
