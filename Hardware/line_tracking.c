#include "line_tracking.h"
#include "Sensor.h"
#include "Motor.h"
#include "PID.h"

// 十字路口计数
unsigned char lukou_num = 0;

// 上一帧线的位置，用于丢线后“朝上一次方向找线”
static float last_position = 0.0f;

// 全黑计数（>3 秒停车）
static uint16_t all_black_cnt = 0;

// 上一次是否在十字（防止每帧都 ++）
static uint8_t last_is_cross = 0;

// 全黑保护时间阈值（按主循环 5ms 一次算：600 * 5ms ≈ 3s）
#define ALL_BLACK_LIMIT   600    

/**
  * @brief  通过权重计算线的位置
  * @return 线的“相对位置” (-4 ~ +4 左负右正，0 居中)
  */
static float Calculate_Line_Position(void)
{
    float weight_sum = 0.0f;
    uint8_t active_cnt = 0;

    // 传感器在黑线上为 1，在白底为 0
    if(L2) { weight_sum += WEIGHT_L2; active_cnt++; }
    if(L1) { weight_sum += WEIGHT_L1; active_cnt++; }
    if(M)  { weight_sum += WEIGHT_M;  active_cnt++; }
    if(R1) { weight_sum += WEIGHT_R1; active_cnt++; }
    if(R2) { weight_sum += WEIGHT_R2; active_cnt++; }

    // 有传感器在黑线上，返回加权平均位置
    if(active_cnt > 0) {
        last_position = weight_sum / active_cnt;  // -4 ~ +4 左负右正
        return last_position;
    }

    // ========= 全白（丢线）：根据上一帧方向找线 =========
    if(last_position > 0.0f) {
        // 上一次线在右边，往右找
        return WEIGHT_R2 + 1.0f;
    } else if(last_position < 0.0f) {
        // 上一次线在左边，往左找
        return WEIGHT_L2 - 1.0f;
    } else {
        // 实在不知道，就先当居中
        return 0.0f;
    }
}

/**
  * @brief  循迹系统初始化
  */
void Track_Init(void)
{
    lukou_num      = 0;
    last_position  = 0.0f;
    all_black_cnt  = 0;
    last_is_cross  = 0;
    PID_Reset(&line_pid);
}

/**
  * @brief  高级循迹函数 - 权重 + PID，带十字计数 + 全黑保护
  */
void Advanced_Tracking(void)
{
    // 读取传感器
    Sensor_Read();

    uint8_t sL2 = L2;
    uint8_t sL1 = L1;
    uint8_t sM  = M;
    uint8_t sR1 = R1;
    uint8_t sR2 = R2;

    uint8_t sensor_sum = sL2 + sL1 + sM + sR1 + sR2;
    uint8_t all_black  = (sensor_sum == 5);

    /* ---------- 全黑保护（>3 秒停车） ---------- */
    if(all_black) {
        if(all_black_cnt < 0xFFFF) all_black_cnt++;
    } else {
        all_black_cnt = 0;
    }

    if(all_black_cnt > ALL_BLACK_LIMIT) {
        Motor_Stop();
        return;
    }

    /* ---------- 十字路口检测（保留功能，但不强行改速度） ---------- */
    if(sensor_sum >= 3) {
        // 从“非十字”进入“十字”的瞬间，判定为通过一个十字
        if(!last_is_cross) {
            lukou_num++;
            // 通过十字后重置 PID，防止残留误差
            PID_Reset(&line_pid);
        }
        last_is_cross = 1;
    } else {
        last_is_cross = 0;
    }

    /* ---------- 正常 权重 + PID 循迹 ---------- */
    // 计算线位置，并减去零点偏置
    float position = Calculate_Line_Position() - POSITION_OFFSET;

    // 这里把 setpoint 和 current 对调，保证 error = position
    // 这样 position > 0 时 adjust > 0，车会朝右拐回到线附近
    float adjust = PID_Calculate(&line_pid, position, 0.0f);

    // 根据偏差大小动态降速：弯道减速，直线全速
    float abs_pos = position;
    if(abs_pos < 0) abs_pos = -abs_pos;

    int base_speed = STRAIGHT_SPEED - (int)(SPEED_K * abs_pos);
    if(base_speed < CORNER_SPEED_MIN) base_speed = CORNER_SPEED_MIN;
    if(base_speed > STRAIGHT_SPEED)  base_speed = STRAIGHT_SPEED;

    // 计算左右轮速度
    int left_speed  = base_speed + (int)adjust;
    int right_speed = base_speed - (int)adjust;

    // 速度限幅（允许到 0，不允许反转）
    if(left_speed > MAX_SPEED)   left_speed = MAX_SPEED;
    if(right_speed > MAX_SPEED)  right_speed = MAX_SPEED;
    if(left_speed < MIN_SPEED)   left_speed = MIN_SPEED;
    if(right_speed < MIN_SPEED)  right_speed = MIN_SPEED;

    // 设置电机速度
    motor(left_speed, right_speed);
}
