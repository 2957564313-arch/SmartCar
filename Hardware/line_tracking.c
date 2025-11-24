#include "line_tracking.h"
#include "sensor.h"
#include "motor.h"
#include "delay.h"
#include "Serial.h"
#include "pid.h"

// 全局变量定义
unsigned char lukou_num = 0;

// 保护变量
static uint32_t lost_line_timer = 0;
static uint32_t all_black_timer = 0;
static uint8_t protection_enabled = 1;
static uint8_t protection_triggered = 0;

/**
  * @brief  计算传感器加权误差
  * @return 加权误差值
  */
static float Calculate_Weighted_Error(void)
{
    float weighted_error = 0.0f;
    uint8_t sensor_count = 0;
    
    // 计算加权误差
    if(L2 == 1) {
        weighted_error += WEIGHT_L2;
        sensor_count++;
    }
    if(L1 == 1) {
        weighted_error += WEIGHT_L1;
        sensor_count++;
    }
    if(M == 1) {
        weighted_error += WEIGHT_M;
        sensor_count++;
    }
    if(R1 == 1) {
        weighted_error += WEIGHT_R1;
        sensor_count++;
    }
    if(R2 == 1) {
        weighted_error += WEIGHT_R2;
        sensor_count++;
    }
    
    // 如果有传感器检测到线，返回加权误差
    if(sensor_count > 0) {
        return weighted_error;
    }
    
    // 没有传感器检测到线（全白），返回最大误差值
    return 10.0f; // 正值表示向右找线
}

/**
  * @brief  检测并处理十字路口
  * @return 1:处理了十字路口, 0:不是十字路口
  */
static uint8_t Handle_Crossroad_Detection(void)
{
    // 检测十字路口（多个传感器同时检测到黑线）
    if((L2 + L1 + M + R1 + R2) >= 3) {  // 至少3个传感器检测到黑线
        // 直行通过十字路口
        motor(STRAIGHT_SPEED, STRAIGHT_SPEED);
        Delay_ms(CROSS_DELAY);
        lukou_num++;
        PID_Reset(&line_pid);  // 重置PID
        return 1;
    }
    return 0;
}

/**
  * @brief  循迹系统初始化
  */
void Track_Init(void)
{
    lukou_num = 0;
    lost_line_timer = 0;
    all_black_timer = 0;
    protection_triggered = 0;
    PID_Reset(&line_pid);
}

/**
  * @brief  线路保护检查
  */
uint8_t Line_Protection_Check(void)
{
    if(!protection_enabled || protection_triggered) return 1;
    
    // 检查全白（丢失线路）
    if(L2 == 0 && L1 == 0 && M == 0 && R1 == 0 && R2 == 0) {
        lost_line_timer++;
        if(lost_line_timer > 300) {  // 3秒
            protection_triggered = 1;
            return 1;
        }
    } else {
        lost_line_timer = 0;
    }
    
    // 检查全黑（冲出赛道）
    if(L2 == 1 && L1 == 1 && M == 1 && R1 == 1 && R2 == 1) {
        all_black_timer++;
        if(all_black_timer > 300) {  // 3秒
            protection_triggered = 1;
            return 1;
        }
    } else {
        all_black_timer = 0;
    }
    
    return 0;
}

/**
  * @brief  高级循迹函数 - 权重+PID算法
  */
void Advanced_Tracking(void)
{
    // 保护检查
    if(protection_triggered || Line_Protection_Check()) {
        Motor_Stop();
        return;
    }
    
    // 读取传感器
    Sensor_Read();
    
    // 优先处理十字路口
    if(Handle_Crossroad_Detection()) {
        return;
    }
    
    // 计算加权误差
    float weighted_error = Calculate_Weighted_Error();
    
    // 使用PID控制器计算调整量
    float adjust = PID_Calculate(&line_pid, 0.0f, weighted_error);
    
    // 计算左右轮速度
    int base_speed = STRAIGHT_SPEED;
    int left_speed = base_speed + (int)adjust;
    int right_speed = base_speed - (int)adjust;
    
    // 速度限制
    if(left_speed > MAX_SPEED) left_speed = MAX_SPEED;
    if(right_speed > MAX_SPEED) right_speed = MAX_SPEED;
    if(left_speed < MIN_SPEED) left_speed = MIN_SPEED;
    if(right_speed < MIN_SPEED) right_speed = MIN_SPEED;
    
    // 设置电机速度
    motor(left_speed, right_speed);
}
