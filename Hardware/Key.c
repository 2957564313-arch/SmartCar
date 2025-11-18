#include "stm32f10x.h" // Device header
#include "PID.h"
#include "Menu.h"

static uint8_t key_pressed = 0;	//当前按下的按键
static uint8_t key_count[4] = {0};      // 按键计数器
static uint8_t last_state[4] = {1,1,1,1}; // 上一次状态
static uint16_t press_time[4] = {0};	//按键按下时间计数器
static uint8_t long_press_flag[4] ={0};	//长按标志
	
uint8_t current_state[4];

void Time_Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_13 |GPIO_Pin_14|GPIO_Pin_15 ; //PC13-UP PC14-DOWN PC15-OK
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz ;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;			//PB0-BACK
	GPIO_Init(GPIOB,&GPIO_InitStructure);

}

uint8_t KEY_GetKey(void)
{
		// 读取按键状态
        current_state[0] = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13);  // 上键
        current_state[1] = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14);  // 下键
        current_state[2] = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_15);  // 确认键
        current_state[3] = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0); // 返回键
        
        for(int i = 0; i < 4; i++)
        {
			
			// 检测下降沿（从1变0）
            if(last_state[i] == 1 && current_state[i] == 0)
            {
                key_count[i] = 1;  // 开始计数
				press_time[i] = 0;	//重置按下时间
				long_press_flag[i] = 0;	//重置长按标志
            }
            else if(last_state[i] == 0 && current_state[i] == 0)
            {
                // 持续按下，继续计数
                if(key_count[i] < 0xFF) 
					key_count[i]++;
				
				if(key_count[i] >= 5)	//消抖完成后开始计时
				{
					press_time[i]++;
				}
				
            }
            else if(last_state[i] == 0 && current_state[i]  == 1)	//处理短按（按下时间小于1000ms且已消抖）
			{
				if(key_count[i] >=5 &&press_time[i] < 1000 && !long_press_flag[i])
				{
					key_pressed = i + 1;  // 1=上, 2=下, 3=确认, 4=返回
				}
				
				//重置状态
				key_count[i] = 0;
				press_time[i] = 0;
				long_press_flag[i] = 0;
			}
			last_state[i] = current_state[i];
			
          
    
            if(key_count[i] >= 5 &&press_time[i] >= 1000 && !long_press_flag[i])
            {
				long_press_flag[i] = 1;	//进入长按状态
				
				if(menu_state == 2 && edit_mode == 1 && (i == 0 || i == 1))
				{
					key_pressed = i+1;	//触发第一次长按调节
				}
            }
			
			//长按连续调节
			if(long_press_flag[i] && (i == 0 || i == 1) && menu_state == 2 && edit_mode == 1 )
			{
				if(press_time[i] >= 1000 )
				{
					uint16_t continuous_time = press_time[i] - 100;	//长按后的持续时间
					
					if(continuous_time % 100 == 0)
					{
						key_pressed = i + 1;
					}
				}
			}
            
        }
	uint8_t key = key_pressed;
	key_pressed = 0;	//读取后清零
	
	return key;
}
