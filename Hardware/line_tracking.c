#include "line_tracking.h"
#include "sensor.h"
#include "motor.h"
#include "delay.h"
#include "Serial.h"
#include "pid.h"

// 全局变量定义
unsigned char lukou_num = 0;
unsigned char last_line_status = 5; // 初始状态设为居中

/**
  * @brief  循迹系统初始化
  */
void Track_Init(void)
{
    lukou_num = 0;
    last_line_status = 5;
    PID_Reset(&line_pid); // 重置PID
}

/**
  * @brief  处理十字路口
  */
void Handle_Crossroad(void)
{
    // 直行通过十字路口
    motor(STRAIGHT_SPEED, STRAIGHT_SPEED);
    Delay_ms(CROSS_DELAY);
    
    // 更新路口计数
    lukou_num++;
    
    // 重置PID控制器
    PID_Reset(&line_pid);

}

/**
  * @brief  处理急弯
  */
void Handle_Sharp_Turn(void)
{
    // 检测急弯条件：只有最外侧传感器检测到黑线
    if((L2 == 1 && L1 == 0 && M == 0 && R1 == 0 && R2 == 0) || 
       (L2 == 0 && L1 == 0 && M == 0 && R1 == 0 && R2 == 1)) {
        
        if(L2 == 1) { // 左急弯
            motor(25, 70);
            Delay_ms(SHARP_TURN_DELAY);
        } else { // 右急弯
            motor(70, 25);
            Delay_ms(SHARP_TURN_DELAY);
        }
    }
}

/**
  * @brief  直线循迹 (状态机方法)
  */
void Track_Straight_Line(void)
{
    unsigned char line_status = Get_Line_Status();
    
    // 首先检查特殊状况
    if(Detect_Crossroad()) {
        Handle_Crossroad();
        return;
    }
    
    Handle_Sharp_Turn(); // 检查急弯
    
    // 状态机控制
    switch(line_status) {
        case 1: // 严重偏左 (10000)
            motor(30, 75);
            break;
        case 2: // 偏左 (11000)
            motor(40, 70);
            break;
        case 3: // 轻微偏左 (01000)
            motor(45, 65);
            break;
        case 4: // 正常偏左 (01100)
            motor(50, 60);
            break;
        case 5: // 居中 (00100)
            motor(STRAIGHT_SPEED, STRAIGHT_SPEED);
            break;
        case 6: // 正常偏右 (00110)
            motor(60, 50);
            break;
        case 7: // 轻微偏右 (00010)
            motor(65, 45);
            break;
        case 8: // 偏右 (00011)
            motor(70, 40);
            break;
        case 9: // 严重偏右 (00001)
            motor(75, 30);
            break;
        case 10: // 丢失路线 (00000)
            // 基于历史状态的智能恢复
            if(last_line_status <= 4) {
                motor(35, 60); // 上次在左侧，向右找线
            } else {
                motor(60, 35); // 上次在右侧，向左找线
            }
            break;
        case 11: // 全黑线 (11111)
            motor(0, 0);
            Delay_ms(2000);
            break;
        default:
            motor(50, 50);
            break;
    }
    
    last_line_status = line_status;
}

/**
  * @brief  PID循迹控制
  */
void Track_With_PID(void)
{
    unsigned char line_status = Get_Line_Status();
    float error = 0;
    
    // 首先检查特殊状况
    if(Detect_Crossroad()) {
        Handle_Crossroad();
        return;
    }
    
    Handle_Sharp_Turn(); // 检查急弯
    
    // 根据循线状态计算偏差
    switch(line_status) {
        case 1: error = -4.0f; break;  // 严重偏左
        case 2: error = -3.0f; break;  // 偏左
        case 3: error = -2.0f; break;  // 轻微偏左
        case 4: error = -1.0f; break;  // 正常偏左
        case 5: error = 0.0f; break;   // 居中
        case 6: error = 1.0f; break;   // 正常偏右
        case 7: error = 2.0f; break;   // 轻微偏右
        case 8: error = 3.0f; break;   // 偏右
        case 9: error = 4.0f; break;   // 严重偏右
        case 10: // 丢失路线
            error = (last_line_status <= 5) ? -3.0f : 3.0f;
            break;
        default: error = 0.0f; break;
    }
    
    // 使用PID控制器计算调整量（目标偏差为0）
    float adjust = PID_Calculate(&line_pid, 0.0f, error);
    
    // 计算左右轮速度
    int left_speed = STRAIGHT_SPEED + (int)adjust;
    int right_speed = STRAIGHT_SPEED - (int)adjust;
    
    // 速度限制
    if(left_speed > 100) left_speed = 100;
    if(right_speed > 100) right_speed = 100;
    if(left_speed < 30) left_speed = 30;
    if(right_speed < 30) right_speed = 30;
    
    motor(left_speed, right_speed);
    last_line_status = line_status;
    
}

/**
  * @brief  高级循迹函数
  */
void Advanced_Tracking(void)
{
    unsigned char line_status = Get_Line_Status();
    
    // 优先处理特殊状况
    if(Detect_Crossroad()) {
        Handle_Crossroad();
        return;
    }
    
    Handle_Sharp_Turn(); // 检查急弯
    
    // 对于小偏差使用PID，大偏差使用状态机
    if(line_status >= 3 && line_status <= 7) {
        Track_With_PID(); // PID精细调整
    } else {
        Track_Straight_Line(); // 状态机大范围调整
    }
}
