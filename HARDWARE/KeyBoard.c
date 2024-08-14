#include "main.h"

u8 mode=1;
extern u8 direct;
// 查HID附录键7盘表格 Keyboard ESCAPE 对应 0x29
// 3A  Keyboard F1
// 控制键为0 CTRL WIN ALT FN SHIFT这几个
//28  Keyboard Return (ENTER) 回车键
//54  Keypad / 
// 这个是真正的键值
// 电路板上的第一行 对应这里的第一列
const unsigned char myKeyBoard_KeyMap_ATValue[myKeyBoard_row_num][myKeyBoard_col_num] =
{
	keyboard_Fn,keypad_chu,keypad_cheng,keypad_jian,
	keypad_num7,keypad_num8,keypad_num9,keypad_jia,
	keypad_num4,keypad_num5,keypad_num6,keypad_jia,
	keypad_num1,keypad_num2,keypad_num3,keypad_ENTER,
	keypad_num0,keypad_num0,keypad_dot,keypad_ENTER,
};

unsigned char send_data[8] = {
							0x00 ,
							0x00 ,
							0x00 ,
							0x00 ,
							0x00 ,
							0x00 ,
							0x00 ,
							0x00 };

							
char unsigned send_data_to_lcd[6] = {
							0x24 ,
							0x00 ,
							0x00 ,
							0x00 ,
							0x00 ,
							0x00 ,
};

							
// 自己看的表 用来对应组合键用的 可以无视
// 电路板上的第一行 对应这里的第一列
const char * myKeyBoard_KeyMap_Name[5][5] = 
{
	"Esc",      "HOME","END","PageUp","PageDown",
	"Backspace","7",   "4",  "1",     "0",
	"/",        "8",   "5",  "2",     "0",
	"*",        "9",   "6",  "3",     ".",
	"-",        "+",   "+",  "Enter", "Enter"
};


const uint16_t key_Col_Pin[myKeyBoard_col_num] = 
{
	myKeyBoard_col0_GPIOPin,
	myKeyBoard_col1_GPIOPin,
	myKeyBoard_col2_GPIOPin,	
	myKeyBoard_col3_GPIOPin,	
//	myKeyBoard_col4_GPIOPin	
		
};

typedef struct 
{
	unsigned char ATKeyData;
	unsigned char rowN;
	unsigned char colN;
}ATKeyDataStructureTypedef;


//普通按键暂存数组
ATKeyDataStructureTypedef ATKeyData[6];
unsigned char ATKeyControlByte5 = 0x00;//状态控制键字节 如Shift Ctrl键等

//6行x16列键盘 状态数组
KeyState_enumTypedef myKeyBoard_KeyState[myKeyBoard_row_num][myKeyBoard_col_num] = {KEYUNPRESSED};//当前按键状态
KeyState_enumTypedef myKeyBoard_KeyState_Ex[myKeyBoard_row_num][myKeyBoard_col_num] = {KEYUNPRESSED};//上次按键状态



unsigned char myKeyBoard_KeyStateChangedFlag = 0;//键盘状态改变标志
unsigned char myKeyBoard_KeyIsControl = 1;//键盘控制按键标志
unsigned char g_myKeyBoard_DataWaitForUploadFlag = 0;//数据等待上传标志


void myKeyBoard_ControlKeyProcess(u8 rowi, u8 colj);
void myKeyBoard_KeyScan_rowScan(GPIO_TypeDef * 		col_GPIOPort, 
								uint8_t 			col_GPIOPin_Index,
								const uint16_t *	p_col_GPIOPin);





//键盘更新
void myKeyBoard_ScanKeyAndUpdataATBuffer()
{
	myKeyBoard_KeyScan();//物理层键盘状态扫描，按键是否被按下 ， 
	myKeyBoard_JudgeKeyStateChange();//判断键盘状态是否有变化，有则置位myKeyBoard_KeyStateChangedFlag标志
	
	if(myKeyBoard_KeyStateChangedFlag)//判断是否有变化
	{
		myKeyBoard_KeyStateChangedFlag = 0;
		myKeyBoard_UpdataATDataPack();//键盘AT数据包更新
		g_myKeyBoard_DataWaitForUploadFlag = 1;//置位数据等待上传标志
	}
}

void myKeyBoard_UpdataATDataPack()
{
	u8 i,j,k;
	u8 AlreadeyExistflag = 0;
	ATKeyControlByte5 = 0x00;//清除控制按键键值

	
	
	//功能待定
	if(myKeyBoard_KeyState[0][0] == KEYPRESSED) // 左上角 Fn 只能用来做组合键 不会输出字符
	{
		
		if(myKeyBoard_KeyState[0][1] == KEYPRESSED) // Fn + “.” 切换蓝牙USB
		{
			if(mode==1)mode=2;
			else mode=1,first_in_flag=1,OLED_Clear();
		}
		else if(myKeyBoard_KeyState[0][2] == KEYPRESSED)
		{
			TIM_Cmd(TIM3, DISABLE);
		}
		else if(myKeyBoard_KeyState[0][3] == KEYPRESSED)
		{
			TIM_Cmd(TIM3, ENABLE);
		}
		else if(myKeyBoard_KeyState[1][0] == KEYPRESSED)
		{		
			if(max_light==255)max_light=0;
			else max_light+=51;
		}
		else if(myKeyBoard_KeyState[1][1] == KEYPRESSED)
		{		
			if(max_light==0)max_light=255;			
			else max_light-=51;
		}
		else if(myKeyBoard_KeyState[1][2] == KEYPRESSED)
		{
			max_light=102;
		}
		else if(myKeyBoard_KeyState[2][0] == KEYPRESSED)
		{
			mode=3;
			OLED_Clear();
			OLED_ShowChar(0,0,50,16);//R
			OLED_ShowChar(0,2,39,16);//G
			OLED_ShowChar(0,4,34,16);//B
		}		
		return ; //防止干扰真正的大键盘区域
	}
	
	
	
	
	for(i = 0; i < 6 ; i++)//判断上次按下的按键现在是否还被按下,若否则将ATKeyData的数据清零
	{
		if(ATKeyData[i].ATKeyData != 0x00)
		{
			if(myKeyBoard_KeyState[ATKeyData[i].rowN][ATKeyData[i].colN] == KEYUNPRESSED)
			{
				ATKeyData[i].ATKeyData = 0x00;
			}
		}
	}
	for(i = 0; i < myKeyBoard_row_num; i++) // 列
	{
		for(j = 0; j < myKeyBoard_col_num; j++) // 对应行
		{
			if(myKeyBoard_KeyState[i][j] == KEYPRESSED)//如果按键被按下则处理键盘数据包
			{
				myKeyBoard_ControlKeyProcess(i,j);//控制按键处理 暂时不要 用下句代替
				if(myKeyBoard_KeyIsControl == 0)
				{
					for(k = 0; k < 5; k++)//判断ATKeyData数据包里是否已经有该键的值了，即是否之前已经被按下了
					{
						if(ATKeyData[k].ATKeyData == myKeyBoard_KeyMap_ATValue[i][j])
						{
							AlreadeyExistflag = 1;
							break;
						}
					}
					if(AlreadeyExistflag == 0)//如果ATKeyData数据包没有该键值则放入
					{
						//循环	只要ATKeyData数据包有空位则插入该键值，并且break。
						//如果六次循环完毕也没插就无效，因为一次只能按下六个按键
						for(k = 0; k < 6; k++)
						{
							if(ATKeyData[k].ATKeyData == 0x00)//将按键AT值放入ATKeyData  Buffer
							{
								ATKeyData[k].ATKeyData = myKeyBoard_KeyMap_ATValue[i][j];
								ATKeyData[k].rowN = i;
								ATKeyData[k].colN = j;
								break;
							}
						}
						
						//贪吃蛇用的
						if(mode==2)
						{
						if(myKeyBoard_KeyMap_ATValue[i][j]==keypad_num8&&direct!=4)direct=3;//上
						if(myKeyBoard_KeyMap_ATValue[i][j]==keypad_num5&&direct!=3)direct=4;//下
						if(myKeyBoard_KeyMap_ATValue[i][j]==keypad_num4&&direct!=1)direct=2;//左
						if(myKeyBoard_KeyMap_ATValue[i][j]==keypad_num6&&direct!=2)direct=1;//右
						}
					}
					else {
						AlreadeyExistflag = 0;}
					myKeyBoard_KeyIsControl = 1;
				}
				
			}
			
		}
	}
	
	//按键键值判断完毕后，将键盘数据包更新
	send_data[0] = ATKeyControlByte5;	//更新控制按键字节
	for(k = 0; k < 6; k++)
	{
		send_data[2 + k] = ATKeyData[k].ATKeyData;//更新普通按键
		send_data_to_lcd[k] = ATKeyData[k].ATKeyData;
	}
}


void myKeyBoard_ControlKeyProcess(u8 rowi, u8 colj)
{
	if(strcmp(myKeyBoard_KeyMap_Name[rowi][colj],"LShift") == 0)
	{
		ATKeyControlByte5 = ATKeyControlByte5 | (0x01 << 1);
	}
	else if(strcmp(myKeyBoard_KeyMap_Name[rowi][colj],"RShift") == 0)
	{
		ATKeyControlByte5 = ATKeyControlByte5 | (0x01 << 5);
	}
	else if(strcmp(myKeyBoard_KeyMap_Name[rowi][colj],"LCtrl") == 0)
	{
		ATKeyControlByte5 = ATKeyControlByte5 | (0x01 << 0);
	}
	else if(strcmp(myKeyBoard_KeyMap_Name[rowi][colj],"RCtrl") == 0)
	{
		ATKeyControlByte5 = ATKeyControlByte5 | (0x01 << 4);
	}
	else if(strcmp(myKeyBoard_KeyMap_Name[rowi][colj],"LAlt") == 0)
	{
		ATKeyControlByte5 = ATKeyControlByte5 | (0x01 << 2);
	}
	else if(strcmp(myKeyBoard_KeyMap_Name[rowi][colj],"RAlt") == 0)
	{
		ATKeyControlByte5 = ATKeyControlByte5 | (0x01 << 6);
	}
	else if(strcmp(myKeyBoard_KeyMap_Name[rowi][colj],"Win") == 0)
	{
		ATKeyControlByte5 = ATKeyControlByte5 | (0x01 << 3);
	}
	else//如果以上都没有判别，则是普通按键，复位控制按键标志
	{
		myKeyBoard_KeyIsControl = 0;
	}
}


void myKeyBoard_JudgeKeyStateChange()
{
	u8 i,j;
	//检查键盘状态是否改变
	for(i = 0; i < myKeyBoard_row_num; i++) //第i列
	{
		for(j = 0; j < myKeyBoard_col_num; j++) //第k行
		{
			if(myKeyBoard_KeyState_Ex[i][j] != myKeyBoard_KeyState[i][j])
			{
				myKeyBoard_KeyStateChangedFlag = 1;
				break;
			}
			
		}
		if(myKeyBoard_KeyStateChangedFlag)
			break;
	}
	
	//将当前的键盘状态传递给上次
	for(i = 0; i < myKeyBoard_row_num; i++)
	{
		for(j = 0; j < myKeyBoard_col_num; j++)
		{
			myKeyBoard_KeyState_Ex[i][j] = myKeyBoard_KeyState[i][j];//将当前的键盘状态传递给上次
		}
	}
	
}

void myKeyBoard_KeyScan()
{
	u8 i;
	for(i = 0 ; i < myKeyBoard_col_num ;i ++)
	{
		myKeyBoard_KeyScan_rowScan(myKeyBoard_col_GPIOPort, i, key_Col_Pin);
		//delay_us(6);
	}
}

void myKeyBoard_KeyScan_rowScan(GPIO_TypeDef * 		col_GPIOPort, 
								uint8_t 			col_GPIOPin_Index,
								const uint16_t *	p_col_GPIOPin)
{
	GPIO_SetBits(col_GPIOPort, *(p_col_GPIOPin + col_GPIOPin_Index));
	delay_us(50);
	if(GPIO_ReadInputDataBit(myKeyBoard_row_GPIOPort, myKeyBoard_row0_GPIOPin)) 
		{myKeyBoard_KeyState[0][col_GPIOPin_Index] = KEYPRESSED;}
	else{myKeyBoard_KeyState[0][col_GPIOPin_Index] = KEYUNPRESSED;}
	delay_us(5);
	if(GPIO_ReadInputDataBit(myKeyBoard_row_GPIOPort, myKeyBoard_row1_GPIOPin)) 
		{myKeyBoard_KeyState[1][col_GPIOPin_Index] = KEYPRESSED;}
	else{myKeyBoard_KeyState[1][col_GPIOPin_Index] = KEYUNPRESSED;}
	delay_us(5);
	if(GPIO_ReadInputDataBit(myKeyBoard_row_GPIOPort, myKeyBoard_row2_GPIOPin)) 
		{myKeyBoard_KeyState[2][col_GPIOPin_Index] = KEYPRESSED;}
	else{myKeyBoard_KeyState[2][col_GPIOPin_Index] = KEYUNPRESSED;}
	delay_us(5);
	if(GPIO_ReadInputDataBit(myKeyBoard_row_GPIOPort, myKeyBoard_row3_GPIOPin)) 
		{myKeyBoard_KeyState[3][col_GPIOPin_Index] = KEYPRESSED;}
	else{myKeyBoard_KeyState[3][col_GPIOPin_Index] = KEYUNPRESSED;}
	delay_us(5);
	if(GPIO_ReadInputDataBit(myKeyBoard_row_GPIOPort, myKeyBoard_row4_GPIOPin)) 
		{myKeyBoard_KeyState[4][col_GPIOPin_Index] = KEYPRESSED;}
	else{myKeyBoard_KeyState[4][col_GPIOPin_Index] = KEYUNPRESSED;}
	//n行就要n个if，格式同上
	GPIO_ResetBits(col_GPIOPort, *(p_col_GPIOPin + col_GPIOPin_Index));
	//delay_us(10);
}

unsigned char myKeyBoard_JudgeKeyPressWithName(const char* keyName)
{
	
	unsigned char i,j;
	for(i = 0; i < myKeyBoard_row_num; i++ )
	{
		for(j = 0; j < myKeyBoard_col_num; j++)
		{
			if(strcmp(myKeyBoard_KeyMap_Name[i][j], keyName) == 0)
			{
				if(myKeyBoard_KeyState[i][j] == KEYUNPRESSED)
				{
					return 0;
				}
				else
				{
					myKeyBoard_KeyState[i][j] = KEYUNPRESSED;
					return 1;
				}
			}
		}
	}
	return 0;
}

//初始化键盘IO
void myKeyBoard_GPIO_Init()
{
	
	GPIO_InitTypeDef GPIOType;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE );

	//初始化 row0-row5 GPIO
	GPIOType.GPIO_Pin = myKeyBoard_row0_GPIOPin | 
						myKeyBoard_row1_GPIOPin | 
						myKeyBoard_row2_GPIOPin | 
						myKeyBoard_row3_GPIOPin | 
						myKeyBoard_row4_GPIOPin ;
	
	GPIOType.GPIO_Speed = GPIO_Speed_50MHz;
	GPIOType.GPIO_Mode = GPIO_Mode_IPD; //下拉输入
	GPIO_Init(myKeyBoard_row_GPIOPort , &GPIOType);
	
	//初始化 col0-col3 GPIO
	GPIOType.GPIO_Pin = myKeyBoard_col0_GPIOPin | 
						myKeyBoard_col1_GPIOPin | 
						myKeyBoard_col2_GPIOPin | 
						myKeyBoard_col3_GPIOPin  
						/*myKeyBoard_col4_GPIOPin*/;

	GPIOType.GPIO_Speed = GPIO_Speed_50MHz;
	GPIOType.GPIO_Mode = GPIO_Mode_Out_PP;		//推挽输出
	GPIO_Init(myKeyBoard_col_GPIOPort , &GPIOType);

}



