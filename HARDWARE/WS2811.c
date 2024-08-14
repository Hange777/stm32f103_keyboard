#include "WS2811.h"
#include "timer.h"
#include "math.h"
#define PI 3.14159265
uint16_t PixelPointer = 0;
uint16_t PixelBuffer2[1024] = {0};// led * 24 +20
u32 colour_data[7]={red,orange,yellow,green,light_blue,blue,pink};
u32 now_color=0;
u8 max_light=255;//人为限定的最大值
float now_light = 1;//实际的亮度

void LED_SPI2_LowLevel_Init(void)//RGB
{
    uint16_t i = 0;

    GPIO_InitTypeDef  GPIO_InitStructure;
    SPI_InitTypeDef   SPI_InitStructure;
    DMA_InitTypeDef   DMA_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2 , ENABLE);

    DMA_DeInit(DMA1_Channel5);
    DMA_InitStructure.DMA_BufferSize = 0;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (SPI2->DR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)PixelBuffer2;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_Init(DMA1_Channel5, &DMA_InitStructure); /* DMA1 CH3 = MEM -> DR */

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);


    SPI_I2S_DeInit(SPI2);

    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	/* spi1就用3就用SPI_BaudRatePrescaler_8，  spi2,3就用SPI_BaudRatePrescaler_4 
		因为SPI 1在APB2总线,速度是72MHz的 而SPI 2 3在APB1总线,速度是36MHz 差2倍
	*/
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4; 
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI2, &SPI_InitStructure);

    SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);

    SPI_Cmd(SPI2, ENABLE);

		
    for (i = 0; i < 1024; i++)
			{PixelBuffer2[i] = 0;}
    PixelPointer = 0;
			
		Tim2_Init(499,7199);//等效于delay
}

void LED_SPI_WriteByte(uint16_t Data,u8 spi)
{
    /* Wait until the transmit buffer is empty */
    /*
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
    {
    }
    */
   if(spi == 2)
	{
		PixelBuffer2[PixelPointer] = Data;
	}
    PixelPointer++;

    /* Send the byte */
    /* SPI_I2S_SendData16(SPI1, Data); */
}

void LED_SPI_SendBits(uint8_t bits,u8 spi)
{
    int zero = 0x7000;  //1111 1000 0000 0000
    int one = 0x7f00;  //1111 1111 1110 0000
    int i = 0x00;

    for (i = 0x80; i >= 0x01; i >>= 1)
    {
        LED_SPI_WriteByte((bits & i) ? one : zero,spi);
    }
}
void LED_SPI_SendPixel(uint32_t color,u8 spi)
{
    /*
     r7,r6,r5,r4,r3,r2,r1,r0,g7,g6,g5,g4,g3,g2,g1,g0,b7,b6,b5,b4,b3,b2,b1,b0
     \_____________________________________________________________________/
                               |      _________________...
                               |     /   __________________...
                               |    /   /   ___________________...
                               |   /   /   /
                              RGB,RGB,RGB,RGB,...,STOP
    */

    /*
    	BUG Fix : Actual is GRB,datasheet is something wrong.
    */
	  uint8_t Red, Green, Blue;  // 三原色
		float true_light;
	
		true_light=now_light*(max_light/255.0);
	
	
		// 绿 红 蓝 三原色分解
	  Red   = (color>>16);
		Red*=true_light;
	  Green = (color>>8);
		Green*=true_light;
	  Blue  = color;
		Blue*=true_light;
		
		
    LED_SPI_SendBits(Green,spi);
    LED_SPI_SendBits(Red,spi);
    LED_SPI_SendBits(Blue,spi);
}

void LED_SPI_Update(u32 buffer[], uint32_t length,u8 spi)
{
    uint8_t i = 0;
	if(spi == 2) 
	{
		if(DMA_GetCurrDataCounter(DMA1_Channel5) == 0)
		{
			for (i = 0; i < length; i++)
			{
				LED_SPI_SendPixel(buffer[i],spi);
			}
			
			PixelPointer = 0;

			DMA_SetCurrDataCounter(DMA1_Channel5, 24*length);
			DMA_Cmd(DMA1_Channel5, ENABLE);
			while(!DMA_GetFlagStatus(DMA1_FLAG_TC5)) ; 	// wait until transfer complete
			DMA_Cmd(DMA1_Channel5, DISABLE); 			// disable DMA channel 6
			DMA_ClearFlag(DMA1_FLAG_TC5); 				// clear DMA1 Channel 6 transfer complete flag
		}
	}
}

//呼吸灯
void breath_light(u32 buffer[],uint32_t length,u8 spi)
{
	static uint8_t i = 3;
		if(i>180)
	{
		i=0;
	}
	now_light=sin(i*PI/180);
	LED_SPI_Update(buffer,led_num,2);
	i+=3;
}

void change_color(u32 buffer[],uint32_t length,u32 color)
{
	u8 i=0;
	static u8 R,G,B;
	static u8 choice=1;
	R   = (color>>16);
	G = (color>>8);
	B  = color;
	
	OLED_ShowNum(10,0,R,3,16);
	OLED_ShowNum(10,2,G,3,16);
	OLED_ShowNum(10,4,B,3,16);
	if(send_data[2]==keypad_ENTER)
	{
		if(choice==3)choice=0;
		choice++;
		delay_ms(1000);
	}
	else if(send_data[2]==keypad_jia)
	{
		if(choice==1)R++;
		else if(choice==2)G++;
		else if(choice==3)B++;
	}
	else if(send_data[2]==keypad_jian)
	{
		if(choice==1)R--;
		else if(choice==2)G--;
		else if(choice==3)B--;
	}
	
	now_color = R<<16 | G<<8 | B;
	for(i=0;i<length;i++)
	{
		buffer[i]=now_color;
	}
}
