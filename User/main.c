#include "stm32f10x.h"
#include "OLED.h"
#include "PID.h"
#include "Key.h"
#include "Encoder.h"
#include "Sensor.h"
#include "Menu.h"
#include "Motor.h"

int main(void)
{
    OLED_Init();
    Encoder_Init();
    PWM_Init();
    
    while(1)
    {
       
    }
}
