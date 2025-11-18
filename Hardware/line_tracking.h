#ifndef __LINE_TRACKING_H
#define __LINE_TRACKING_H

// 循迹参数配置
#define STRAIGHT_SPEED       60   // 直道速度
#define TURN_SPEED          45   // 弯道速度
#define CROSS_DELAY         300  // 十字路口直行时间(ms)
#define SHARP_TURN_DELAY    150  // 急弯转向时间(ms)

// 全局变量声明
extern unsigned char lukou_num;
extern unsigned char last_line_status;
extern unsigned int straight_count;

// 函数声明
void Track_Init(void);
void Track_Straight_Line(void);
void Track_With_PID(int base_speed, float kp, float ki, float kd);
void Handle_Crossroad(void);
void Handle_Sharp_Turn(void);
void Advanced_Tracking(void);

#endif