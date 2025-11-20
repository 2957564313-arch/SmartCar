#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "Key.h"
#include "PID.h"
#include "Menu.h"

// 全局变量
SystemState_t system_state = SYSTEM_MENU;
uint8_t car_started = 0;
uint8_t launch_confirmed = 0;  // 发车确认标志
uint8_t menu_state = 0;	//0=主菜单,1=发车功能,2=PID调试
uint8_t cursor_pos = 1;	//光标位置
uint8_t edit_mode = 0;	//0=浏览，1=编辑

void Show_Main_Menu(void)
{
	OLED_Clear();
	OLED_ShowString(1,2,"LAUNCH");
	OLED_ShowString(2,2,"PID");
	
	if(cursor_pos == 1)
		OLED_ShowChar(1,1,'>');
	if(cursor_pos == 2)
		OLED_ShowChar(2,1,'>');
}

void Show_PID_Menu(void)
{
	OLED_Clear();
	OLED_ShowString(1,1,"PID");
	OLED_ShowString(2,2,"kp");
	OLED_ShowString(3,2,"ki");
	OLED_ShowString(4,2,"kd");
	OLED_ShowFloat(2,13,line_pid.kp,1,1);
	OLED_ShowFloat(3,13,line_pid.ki,1,1);
	OLED_ShowFloat(4,13,line_pid.kd,1,1);
	
	if(edit_mode)
		OLED_ShowChar(1,15,'E');
	
	if(cursor_pos == 2)
		OLED_ShowChar(2,1,'>');
	if(cursor_pos == 3)
		OLED_ShowChar(3,1,'>');
	if(cursor_pos == 4)
		OLED_ShowChar(4,1,'>');
}

void Handle_Key(uint8_t key)
{
	if(key == 0)return;
	
	if(menu_state == 0)
	{
		if(key == 1||key == 2)		//按下上键或者上键
			cursor_pos = cursor_pos==1 ? 2 : 1;		
		else if(key == 3)		//按下确认键
		{
			if(cursor_pos == 1)
			{
				//这里写发车功能//
				system_state = SYSTEM_RUNNING; //系统状态为发车状态
				menu_state = 1;
				OLED_Clear();
				
			}
			if(cursor_pos == 2)
			{
				menu_state = 2;		//进入pid调试界面
				cursor_pos = 2;
			}
			
		}	
			
	}
			else if(menu_state == 2)		//pid调试界面
			{
				if(edit_mode == 0)	//浏览模式
				{
					if(key == 1)
						cursor_pos = (cursor_pos == 2) ? 4 : (cursor_pos - 1);
					else if(key == 2)
						cursor_pos = (cursor_pos-1) % 3 + 2;
					else if(key == 3)
					{
						edit_mode = 1;
					}
					else if(key == 4)
					{
						menu_state = 0;
						cursor_pos = 1;
					}
					
				}
				else	//编辑模式
				{
					if(key == 3)
					{
						edit_mode = 0;
					}
					else if(key == 1)
					{
						switch (cursor_pos)
						{
							case 2:line_pid.kp+=0.1;break;
							case 3:line_pid.ki+=0.1;break;
							case 4:line_pid.kd+=0.1;break;
						}
					}
					else if(key == 2)
					{
						switch (cursor_pos)
						{
							case 2:line_pid.kp-=0.1;break;
							case 3:line_pid.ki-=0.1;break;
							case 4:line_pid.kd-=0.1;break;
						}
					}
				}
		}
		
	switch (menu_state)
	{
	case 0:
	Show_Main_Menu();
	break;
	case 2:
	Show_PID_Menu();
	break;
    }
}
