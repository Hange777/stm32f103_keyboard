#include "led.h"
   
//LED IO��ʼ��
void LED_Init(void)
{
    GPIO_InitTypeDef GPIOC13;
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		GPIOC13.GPIO_Pin = GPIO_Pin_8;				 //�˿�����
    GPIOC13.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    GPIOC13.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOA, &GPIOC13);					 //�����趨������ʼ��PC13
    GPIO_SetBits(GPIOA, GPIO_Pin_8);						    //PC13��
}
