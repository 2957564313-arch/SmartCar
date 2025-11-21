#include "stm32f10x.h"
#include "OLED.h"
#include "PID.h"
#include "Key.h"
#include "Encoder.h"
#include "Sensor.h"
#include "Menu.h"
#include "Motor.h"
#include "line_tracking.h"
#include "Serial.h"
#include "Delay.h"
#include <stdio.h>

/**
  * @brief  启动循迹功能
  */
void Start_Line_Tracking(void)
{
    system_state = SYSTEM_RUNNING;
    car_started = 1;
    launch_confirmed = 1;  // 标记已发车
    
    // 关闭OLED显示以节省CPU
    OLED_Clear();
 
    
    // 重置循迹系统
    Track_Init();
    Encoder_Reset_Both();
    

}

/**
  * @brief  停止循迹功能
  */
void Stop_Line_Tracking(void)
{
    system_state = SYSTEM_STOP;
    car_started = 0;
    
    // 停止电机
    Motor_Stop();
    
    // 注意：停止后不重新打开OLED，保持关闭状态
    // 只能通过断电重启来重新进入菜单
}

/**
  * @brief  检查是否允许发车（只能发车一次）
  * @return 1:允许发车, 0:不允许
  */
uint8_t Can_Launch(void)
{
    // 如果已经发车过，则不允许再次发车
    if(launch_confirmed)
    {
        return 0;
    }
    return 1;
}

int main(void)
{
    Motor_Init();        // 电机初始化
    Sensor_Init();       // 传感器初始化
    Encoder_Init();      // 编码器初始化
    Time_Key_Init();     // 按键初始化
    OLED_Init();         // OLED初始化
    USART1_Init(); 		 // 串口初始化
	Show_Main_Menu();
    
    while(1)
    {
        uint8_t key = KEY_GetKey();  // 获取按键
        
        if(key != 0)
        {
           
            // 根据系统状态处理按键
            switch(system_state)
            {
                case SYSTEM_MENU:
                    // 菜单状态下处理菜单
                    Handle_Key(key);
                    break;
                    
                case SYSTEM_RUNNING:
                    // 运行状态下，BACK键停止
                    if(key == 4)  // BACK键
                    {
                        Stop_Line_Tracking();
                    }
                    break;
                    
                case SYSTEM_STOP:
                    // 停止状态下不响应任何按键
                    // 只能通过断电重启
                    break;
            }
        }
        
        // 根据系统状态执行不同任务
        switch(system_state)
        {
            case SYSTEM_MENU:
                // 菜单状态下不执行循迹
                break;
                
            case SYSTEM_RUNNING:
                // 运行状态下执行循迹
                if(car_started)
                {
					OLED_Clear();
             
					// 读取传感器
                    Sensor_Read();
                    
                    // 执行高级循迹算法
                    Advanced_Tracking();
                    
                    // 发送调试信息到串口（可选）
                    static uint32_t debug_timer = 0;
                    debug_timer++;
                    if(debug_timer >= 100)  // 每100次循环发送一次
                    {
                        printf("L:%d %d %d %d %d Stat:%d\r\n", 
                               L2, L1, M, R1, R2, Get_Line_Status());
                        debug_timer = 0;
                    }
                }
                break;
                
            case SYSTEM_STOP:
                // 停止状态，电机已停止
                break;
        }
        
        Delay_ms(10);  // 10ms延时，控制循环频率
    }
}
