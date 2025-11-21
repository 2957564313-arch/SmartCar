#ifndef __SENSOR_H
#define __SENSOR_H

// 红外传感器引脚定义
#define L2_PIN    GPIO_Pin_0   // 最左边传感器
#define L2_PORT   GPIOA
#define L1_PIN    GPIO_Pin_1
#define L1_PORT   GPIOA  
#define M_PIN     GPIO_Pin_4   // 中间传感器
#define M_PORT    GPIOA
#define R1_PIN    GPIO_Pin_5
#define R1_PORT   GPIOA
#define R2_PIN    GPIO_Pin_15  // 最右边传感器（修改为PA15）
#define R2_PORT   GPIOA

// 传感器数量
#define SENSOR_NUM 5

// 全局变量声明
extern int L2, L1, M, R1, R2;

void Sensor_Init(void);
void Sensor_Read(void);
unsigned char Get_Line_Status(void);
unsigned char Detect_Crossroad(void);

#endif
