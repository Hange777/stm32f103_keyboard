#include "main.h"



void WS_SetAll(u32 WsDat[], u8 number) //流水灯
{
	unsigned int j;
	unsigned long temp;
	temp=WsDat[0];
	for(j=0;j<number;j++)
	{
		WsDat[j]=WsDat[j+1];
	}
	WsDat[number-1]=temp;
	
}


int main (void) 
{
	u16 i;
	u32 buffer[led_num];
	
	delay_init();
	delay_ms(100);//上电缓一缓,不然oled初始化有几率失败
	
	LED_SPI2_LowLevel_Init();
//	OLED_Init();//初始化OLED  
//	OLED_Clear();

//	myKeyBoard_GPIO_Init();
//	my_usb_init();
	

for(i = 0 ;i < led_num ; i++) //流水灯的颜色
	{
//		buffer[i] = colour_data[i%7]	;
		buffer[i] =0x00FFFF;
	}

//	now_color=0xFFFFFF;
	while(1)
	{
		
		myKeyBoard_ScanKeyAndUpdataATBuffer();//键盘事件更新
		
		if(mode==1)
		{
			if(g_myKeyBoard_DataWaitForUploadFlag == 1)
			{
				g_myKeyBoard_DataWaitForUploadFlag = 0;

				Keyboard_Send(send_data);	
				OLED_ShowChar_HID(0,0,send_data_to_lcd[0],16);
				OLED_ShowChar_HID(8,0,send_data_to_lcd[1],16);
				OLED_ShowChar_HID(16,0,send_data_to_lcd[2],16);
				OLED_ShowChar_HID(24,0,send_data_to_lcd[3],16);
				OLED_ShowChar_HID(32,0,send_data_to_lcd[4],16);
				OLED_ShowChar_HID(40,0,send_data_to_lcd[5],16);
			}
		}
		else if(mode==2)
		{
			if(first_in_flag)
			{
				snak_init();
				first_in_flag=0;
			}
			if(flash_flag)
			{
				snake_flash(direct);
				flash_flag=0;
			}
		}

		
		if(light_flash_flag)
		{
				if(mode==3)
				{
					change_color(buffer,led_num,now_color);
					LED_SPI_Update(buffer,led_num,2);
					breath_light(buffer,led_num,2);
					light_flash_flag=0;	
				}
				else
				{
				breath_light(buffer,led_num,2);
				light_flash_flag=0;	
				}
		}
	}
}
