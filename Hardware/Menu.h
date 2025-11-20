#ifndef __MENU_H
#define __MENU_H

void Show_Main_Menu(void);
void Show_PID_Menu(void);
void Handle_Key(uint8_t key);

// 系统状态定义
typedef enum {
    SYSTEM_MENU = 0,     // 菜单状态
    SYSTEM_RUNNING = 1,  // 运行状态
    SYSTEM_STOP = 2      // 停止状态
} SystemState_t;



extern SystemState_t system_state;
extern uint8_t car_started;
extern uint8_t launch_confirmed;
extern uint8_t menu_state;
extern uint8_t edit_mode;

#endif

