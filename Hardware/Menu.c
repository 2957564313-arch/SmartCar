#include "stm32f10x.h"                  
#include "OLED.h"
#include "Key.h"
#include "PID.h"
#include "Menu.h"
#include "line_tracking.h"

uint8_t menu_state = 0;		//0=主菜单 1=发车 2=PID 3=直线速度 4=弯道速度
uint8_t cursor_pos = 1;		//光标位置
uint8_t edit_mode = 0;		//0=浏览模式 1=编辑模式

/**
  * @brief  显示主菜单
  */
void Show_Main_Menu(void)
{
    OLED_Clear();
    
    // 检查是否已经发车过
    if(!Can_Launch())
    {
        return;
    }
    
    OLED_ShowString(1, 2, "LAUNCH");
    OLED_ShowString(2, 2, "PID");
	OLED_ShowString(3, 2, "STRAIGHT");
	OLED_ShowString(4, 2, "CORNER");
    
    if(cursor_pos == 1)
        OLED_ShowChar(1, 1, '>');
    if(cursor_pos == 2)
        OLED_ShowChar(2, 1, '>');
	if(cursor_pos == 3)
        OLED_ShowChar(3, 1, '>');
    if(cursor_pos == 4)
        OLED_ShowChar(4, 1, '>');
}

/**
  * @brief  显示PID菜单
  */
void Show_PID_Menu(void)
{
    OLED_Clear();
    OLED_ShowString(1, 1, "PID Setting");
    OLED_ShowString(2, 2, "Kp:");
    OLED_ShowString(3, 2, "Ki:");
    OLED_ShowString(4, 2, "Kd:");
    OLED_ShowFloat(2, 10, line_pid.kp, 1, 1);
    OLED_ShowFloat(3, 10, line_pid.ki, 1, 2);
    OLED_ShowFloat(4, 10, line_pid.kd, 1, 1);
    
    if(edit_mode)
        OLED_ShowChar(1, 15, 'E');
	if(cursor_pos == 1)
        OLED_ShowChar(1, 1, '>');
    if(cursor_pos == 2)
        OLED_ShowChar(2, 1, '>');
	if(cursor_pos == 3)
        OLED_ShowChar(3, 1, '>');
    if(cursor_pos == 4)
        OLED_ShowChar(4, 1, '>');

}

/**
  * @brief  显示直线速度菜单
  */
void Show_STRAIGHT_SPEED_Menu(void)
{
    OLED_Clear();
    OLED_ShowString(1, 1, "STRAIGHT SPEED");
    OLED_ShowNum(2,2,STRAIGHT_SPEED,3);
    
    if(edit_mode)
        OLED_ShowChar(1, 15, 'E');
    
    if(cursor_pos == 2)
        OLED_ShowChar(2, 1, '>');
}

/**
  * @brief  显示弯道速度最低基础速度菜单
  */
void Show_CORNER_SPEED_Menu(void)
{
    OLED_Clear();
    OLED_ShowString(1, 1, "CORNER SPEED");
    OLED_ShowNum(2,2,CORNER_SPEED_MIN,2);
	
    if(edit_mode)
        OLED_ShowChar(1, 15, 'E');
    
    if(cursor_pos == 2)
        OLED_ShowChar(2, 1, '>');
}

void Handle_Key(uint8_t key)
{
    if(key == 0) return;
    
    // 如果已经发车过，不允许任何菜单操作
    if(!Can_Launch())
    {
        Show_Main_Menu();
        return;
    }
    
    if(menu_state == 0)  // 主菜单
    {
        if(key == 1)  // 上键
        {
            cursor_pos = cursor_pos + 3 ;
			while(cursor_pos > 4)
			{
				cursor_pos-=4;
			}
			Show_Main_Menu();
        }
		else if(key == 2)  // 下键
        {
            cursor_pos = cursor_pos + 1;
			while(cursor_pos > 4)
			{
				cursor_pos-=4;
			}
			Show_Main_Menu();
        }
        else if(key == 3)  // 确认键
        {
            if(cursor_pos == 1)  // LAUNCH - 直接发车，无需确认
            {
                // 只能发车一次
                if(Can_Launch())
                {
                    Start_Line_Tracking();
                }
            }
            else if(cursor_pos == 2)  // PID
            {
                menu_state = 2;  // 进PID调试界面
                cursor_pos = 2;
                Show_PID_Menu();
            }
			else if(cursor_pos == 3)  // 直线速度
            {
                menu_state = 3;  // 进入直线速度调试界面
                cursor_pos = 2;
                Show_STRAIGHT_SPEED_Menu();
            }
			else if(cursor_pos == 4)  // 弯道速度
            {
                menu_state = 4;  // 进入弯道速度调试界面
                cursor_pos = 2;
                Show_CORNER_SPEED_Menu();
            }
        }
    }
	
    else if(menu_state == 2)  // PID调试界面
    {
        if(edit_mode == 0)  // 浏览模式
        {
            if(key == 1)  // 上键
            {
                cursor_pos = (cursor_pos == 2) ? 4 : (cursor_pos - 1);
                Show_PID_Menu();
            }
            else if(key == 2)  // 下键
            {
                cursor_pos = (cursor_pos == 4) ? 2 : (cursor_pos + 1);
                Show_PID_Menu();
            }
            else if(key == 3)  // 确认键 - 进入编辑
            {
                edit_mode = 1;
                Show_PID_Menu();
            }
            else if(key == 4)  // BACK键 - 返回主菜单
            {
                menu_state = 0;
                cursor_pos = 1;
                Show_Main_Menu();
            }
        }
        else  // 编辑模式
        {
            if(key == 3)  // 确认键 - 退出编辑
            {
                edit_mode = 0;
                Show_PID_Menu();
            }
            else if(key == 1)  // 上键 - 增加数值
            {
                switch(cursor_pos)
                {
                    case 2: line_pid.kp += 0.1f; break;
                    case 3: line_pid.ki += 0.01f; break;  // Ki调整步长改为0.01
                    case 4: line_pid.kd += 0.1f; break;
                }
                // 限制范围
                if(line_pid.kp > 10.0f) line_pid.kp = 10.0f;
                if(line_pid.ki > 1.0f) line_pid.ki = 1.0f;
                if(line_pid.kd > 10.0f) line_pid.kd = 10.0f;
                
                Show_PID_Menu();
 
            }
            else if(key == 2)  // 下键 - 减少数值
            {
                switch(cursor_pos)
                {
                    case 2: line_pid.kp -= 0.1f; break;
                    case 3: line_pid.ki -= 0.01f; break;  // Ki调整步长改为0.01
                    case 4: line_pid.kd -= 0.1f; break;
                }
                // 限制最小值
                if(line_pid.kp < 0.0f) line_pid.kp = 0.0f;
                if(line_pid.ki < 0.0f) line_pid.ki = 0.0f;
                if(line_pid.kd < 0.0f) line_pid.kd = 0.0f;
                
                Show_PID_Menu();
               
            }
        }
    }
	
	else if(menu_state == 3)  // 直线速度调试界面
    {
        if(edit_mode == 0)  // 浏览模式
        {
            
            if(key == 3)  // 确认键 - 进入编辑
            {
                edit_mode = 1;
                Show_STRAIGHT_SPEED_Menu();
            }
            else if(key == 4)  // BACK键 - 返回主菜单
            {
                menu_state = 0;
                cursor_pos = 1;
                Show_Main_Menu();
            }
        }
        else  // 编辑模式
        {
            if(key == 3)  // 确认键 - 退出编辑
            {
                edit_mode = 0;
                Show_STRAIGHT_SPEED_Menu();
            }
            else if(key == 1)  // 上键 - 增加数值
            {
                STRAIGHT_SPEED+=5;
                // 限制范围
                if(STRAIGHT_SPEED > 100)STRAIGHT_SPEED  = 100;
				if(STRAIGHT_SPEED < 0)STRAIGHT_SPEED  = 0;				
                Show_STRAIGHT_SPEED_Menu();
            }
            else if(key == 2)  // 下键 - 减少数值
            {
                STRAIGHT_SPEED-=5;
                // 限制范围
				if(STRAIGHT_SPEED > 100)STRAIGHT_SPEED  = 100;
                if(STRAIGHT_SPEED < 0)STRAIGHT_SPEED  = 0;  
                Show_STRAIGHT_SPEED_Menu();
            }
        }
    }
	
	else if(menu_state == 4)  // 弯道速度调试界面
    {
        if(edit_mode == 0)  // 浏览模式
        {
            if(key == 3)  // 确认键 - 进入编辑
            {
                edit_mode = 1;
                Show_CORNER_SPEED_Menu();
            }
            else if(key == 4)  // BACK键 - 返回主菜单
            {
                menu_state = 0;
                cursor_pos = 1;
                Show_Main_Menu();
            }
        }
        else  // 编辑模式
        {
            if(key == 3)  // 确认键 - 退出编辑
            {
                edit_mode = 0;
                Show_CORNER_SPEED_Menu();
            }
            else if(key == 1)  // 上键 - 增加数值
            {
                CORNER_SPEED_MIN+=1;
                // 限制范围
                if(CORNER_SPEED_MIN > 100)CORNER_SPEED_MIN  = 100;
				if(CORNER_SPEED_MIN < 0)CORNER_SPEED_MIN  = 0;				
                Show_CORNER_SPEED_Menu();
            }
            else if(key == 2)  // 下键 - 减少数值
            {
                CORNER_SPEED_MIN-=1;
                // 限制范围
				if(CORNER_SPEED_MIN > 100)CORNER_SPEED_MIN  = 100;
                if(CORNER_SPEED_MIN < 0)CORNER_SPEED_MIN  = 0;  
                Show_CORNER_SPEED_Menu();
            }
        }
    }
}
