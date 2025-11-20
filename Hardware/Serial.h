#ifndef __SERIAL_H
#define __SERIAL_H


// 函数声明
void USART1_Init(void);
void USART1_SendByte(uint8_t byte);
void USART1_SendString(char *str);

#endif
