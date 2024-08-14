#include "led.h"
   
//LED IO初始化
void LED_Init(void)
{
    GPIO_InitTypeDef GPIOC13;
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		GPIOC13.GPIO_Pin = GPIO_Pin_8;				 //端口配置
    GPIOC13.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIOC13.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOA, &GPIOC13);					 //根据设定参数初始化PC13
    GPIO_SetBits(GPIOA, GPIO_Pin_8);						    //PC13低
}
