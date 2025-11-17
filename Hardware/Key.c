#include "stm32f10x.h" // Device header

extern float pid_param[3];
extern uint8_t menu_state;
extern uint8_t edit_mode;


static uint8_t key_pressed = 0;	//当前按下的按键


void Time_Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0 |GPIO_Pin_2|GPIO_Pin_4 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz ;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period =1000-1 ;//1ms
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72-1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter =0 ;
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStruture;
	NVIC_InitStruture.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStruture.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruture.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruture.NVIC_IRQChannelSubPriority =0 ;
	NVIC_Init(&NVIC_InitStruture);
		
	TIM_Cmd(TIM2,ENABLE);
}

uint8_t KEY_GetKey(void)
{
	uint8_t key = key_pressed;
	key_pressed = 0;	//读取后清零
	return key;
}

void TIM2_IRQHandler(void)
{
    static uint8_t key_count[4] = {0};      // 按键计数器
    static uint8_t last_state[4] = {1,1,1,1}; // 上一次状态
	static uint16_t press_time[4] = {0};	//按键按下时间计数器
	static uint8_t long_press_flag[4] ={0};	//长按标志
	
    uint8_t current_state[4];
	
    
    if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
    {
        // 读取按键状态
        current_state[0] = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4);  // 上键
        current_state[1] = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2);  // 下键
        current_state[2] = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);  // 确认键
        current_state[3] = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14); // 返回键
        
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
        
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}