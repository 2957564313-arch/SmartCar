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

int main(void)
{
    Motor_Init();
    Sensor_Init();
    Encoder_Init();      // 新增编码器初始化
    Time_Key_Init();
    OLED_Init();
    while(1)
    {
       uint8_t key = KEY_GetKey();	//获取按键
		
		if(key != 0)
			Handle_Key(key);	//处理按键
    }
}
