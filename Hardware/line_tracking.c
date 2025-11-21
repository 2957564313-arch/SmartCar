#include "line_tracking.h"
#include "sensor.h"
#include "motor.h"
#include "delay.h"
#include "Serial.h"
#include "pid.h"

// 全局变量定义
unsigned char lukou_num = 0;
unsigned char last_line_status = 5;

// 保护变量
static uint32_t lost_line_timer = 0;
static uint32_t all_black_timer = 0;
static uint8_t protection_enabled = 1;
static uint8_t protection_triggered = 0;

// 新增：传感器权重计算
static int Calculate_Sensor_Weight(void)
{
    // 给每个传感器分配权重，中间为0，两边对称
    // L2(-2) L1(-1) M(0) R1(1) R2(2)
    int weight = 0;
    
    if(L2 == 1) weight -= 2;  // 最左侧传感器权重-2
    if(L1 == 1) weight -= 1;  // 左侧传感器权重-1
    if(R1 == 1) weight += 1;  // 右侧传感器权重+1
    if(R2 == 1) weight += 2;  // 最右侧传感器权重+2
    
    return weight;
}

/**
  * @brief  循迹系统初始化
  */
void Track_Init(void)
{
    lukou_num = 0;
    last_line_status = 5;
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
    
    unsigned char line_status = Get_Line_Status();
    
    if(line_status == 10)  // 全白
    {
        lost_line_timer++;
        all_black_timer = 0;
        
        if(lost_line_timer > 300)  // 3秒
        {
            protection_triggered = 1;
            return 1;
        }
    }
    else if(L2 == 1 && L1 == 1 && M == 1 && R1 == 1 && R2 == 1)  // 全黑
    {
        all_black_timer++;
        lost_line_timer = 0;
        
        if(all_black_timer > 300)  // 3秒
        {
            protection_triggered = 1;
            return 1;
        }
    }
    else
    {
        lost_line_timer = 0;
        all_black_timer = 0;
    }
    
    return 0;
}

/**
  * @brief  处理十字路口
  */
void Handle_Crossroad(void)
{
    motor(STRAIGHT_SPEED, STRAIGHT_SPEED);
    Delay_ms(CROSS_DELAY);
    lukou_num++;
    PID_Reset(&line_pid);
}

/**
  * @brief  激进循迹算法 - 基于传感器权重
  */
void Aggressive_Tracking(void)
{
    int weight = Calculate_Sensor_Weight();
    
    // 根据权重直接计算电机速度
    int base_speed = STRAIGHT_SPEED;
    
    // 权重范围：-4到+4，映射到速度差
    int speed_diff = weight * 12;  // 每个权重点对应12的速度差
    
    int left_speed = base_speed - speed_diff;
    int right_speed = base_speed + speed_diff;
    
    // 限制速度范围
    if(left_speed > MAX_SPEED) left_speed = MAX_SPEED;
    if(right_speed > MAX_SPEED) right_speed = MAX_SPEED;
    if(left_speed < MIN_SPEED) left_speed = MIN_SPEED;
    if(right_speed < MIN_SPEED) right_speed = MIN_SPEED;
    
    motor(left_speed, right_speed);
}

/**
  * @brief  处理极端情况 - 强力转弯
  */
void Handle_Extreme_Cases(void)
{
    // 检测只有最外侧传感器看到线的情况（急弯）
    if(L2 == 1 && L1 == 0 && M == 0 && R1 == 0 && R2 == 0) {
        // 左急弯 - 强力右转
        motor(8, 50);
        return;
    }
    
    if(L2 == 0 && L1 == 0 && M == 0 && R1 == 0 && R2 == 1) {
        // 右急弯 - 强力左转
        motor(50, 8);
        return;
    }
    
    // 检测只有两个外侧传感器看到线的情况（中急弯）
    if(L2 == 1 && L1 == 1 && M == 0 && R1 == 0 && R2 == 0) {
        // 左中急弯
        motor(15, 45);
        return;
    }
    
    if(L2 == 0 && L1 == 0 && M == 0 && R1 == 1 && R2 == 1) {
        // 右中急弯
        motor(45, 15);
        return;
    }
    
    // 检测丢失线路的情况
    if(L2 == 0 && L1 == 0 && M == 0 && R1 == 0 && R2 == 0) {
        // 基于历史状态恢复
        if(last_line_status <= 4) {
            motor(12, 40); // 上次偏左，向右找线
        } else {
            motor(40, 12); // 上次偏右，向左找线
        }
        return;
    }
}

/**
  * @brief  高级循迹函数 - 简化但更激进
  */
void Advanced_Tracking(void)
{
    // 保护检查
    if(protection_triggered || Line_Protection_Check())
    {
        Motor_Stop();
        protection_triggered = 1;
        return;
    }
    
    // 十字路口处理
    if(Detect_Crossroad()) {
        Handle_Crossroad();
        return;
    }
    
    // 读取传感器
    Sensor_Read();
    
    // 先处理极端情况（急弯、丢失线路等）
    if((L2 == 1 && L1 == 0 && M == 0 && R1 == 0 && R2 == 0) ||  // 左急弯
       (L2 == 0 && L1 == 0 && M == 0 && R1 == 0 && R2 == 1) ||  // 右急弯
       (L2 == 1 && L1 == 1 && M == 0 && R1 == 0 && R2 == 0) ||  // 左中急弯
       (L2 == 0 && L1 == 0 && M == 0 && R1 == 1 && R2 == 1) ||  // 右中急弯
       (L2 == 0 && L1 == 0 && M == 0 && R1 == 0 && R2 == 0))    // 丢失线路
    {
        Handle_Extreme_Cases();
    }
    else
    {
        // 正常情况使用权重算法
        Aggressive_Tracking();
    }
    
    // 更新最后状态
    last_line_status = Get_Line_Status();
}
