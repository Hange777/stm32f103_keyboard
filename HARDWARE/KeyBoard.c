#include "main.h"

u8 mode=1;
extern u8 direct;
// ��HID��¼��7�̱�� Keyboard ESCAPE ��Ӧ 0x29
// 3A  Keyboard F1
// ���Ƽ�Ϊ0 CTRL WIN ALT FN SHIFT�⼸��
//28  Keyboard Return (ENTER) �س���
//54  Keypad / 
// ����������ļ�ֵ
// ��·���ϵĵ�һ�� ��Ӧ����ĵ�һ��
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

							
// �Լ����ı� ������Ӧ��ϼ��õ� ��������
// ��·���ϵĵ�һ�� ��Ӧ����ĵ�һ��
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


//��ͨ�����ݴ�����
ATKeyDataStructureTypedef ATKeyData[6];
unsigned char ATKeyControlByte5 = 0x00;//״̬���Ƽ��ֽ� ��Shift Ctrl����

//6��x16�м��� ״̬����
KeyState_enumTypedef myKeyBoard_KeyState[myKeyBoard_row_num][myKeyBoard_col_num] = {KEYUNPRESSED};//��ǰ����״̬
KeyState_enumTypedef myKeyBoard_KeyState_Ex[myKeyBoard_row_num][myKeyBoard_col_num] = {KEYUNPRESSED};//�ϴΰ���״̬



unsigned char myKeyBoard_KeyStateChangedFlag = 0;//����״̬�ı��־
unsigned char myKeyBoard_KeyIsControl = 1;//���̿��ư�����־
unsigned char g_myKeyBoard_DataWaitForUploadFlag = 0;//���ݵȴ��ϴ���־


void myKeyBoard_ControlKeyProcess(u8 rowi, u8 colj);
void myKeyBoard_KeyScan_rowScan(GPIO_TypeDef * 		col_GPIOPort, 
								uint8_t 			col_GPIOPin_Index,
								const uint16_t *	p_col_GPIOPin);





//���̸���
void myKeyBoard_ScanKeyAndUpdataATBuffer()
{
	myKeyBoard_KeyScan();//��������״̬ɨ�裬�����Ƿ񱻰��� �� 
	myKeyBoard_JudgeKeyStateChange();//�жϼ���״̬�Ƿ��б仯��������λmyKeyBoard_KeyStateChangedFlag��־
	
	if(myKeyBoard_KeyStateChangedFlag)//�ж��Ƿ��б仯
	{
		myKeyBoard_KeyStateChangedFlag = 0;
		myKeyBoard_UpdataATDataPack();//����AT���ݰ�����
		g_myKeyBoard_DataWaitForUploadFlag = 1;//��λ���ݵȴ��ϴ���־
	}
}

void myKeyBoard_UpdataATDataPack()
{
	u8 i,j,k;
	u8 AlreadeyExistflag = 0;
	ATKeyControlByte5 = 0x00;//������ư�����ֵ

	
	
	//���ܴ���
	if(myKeyBoard_KeyState[0][0] == KEYPRESSED) // ���Ͻ� Fn ֻ����������ϼ� ��������ַ�
	{
		
		if(myKeyBoard_KeyState[0][1] == KEYPRESSED) // Fn + ��.�� �л�����USB
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
		return ; //��ֹ���������Ĵ��������
	}
	
	
	
	
	for(i = 0; i < 6 ; i++)//�ж��ϴΰ��µİ��������Ƿ񻹱�����,������ATKeyData����������
	{
		if(ATKeyData[i].ATKeyData != 0x00)
		{
			if(myKeyBoard_KeyState[ATKeyData[i].rowN][ATKeyData[i].colN] == KEYUNPRESSED)
			{
				ATKeyData[i].ATKeyData = 0x00;
			}
		}
	}
	for(i = 0; i < myKeyBoard_row_num; i++) // ��
	{
		for(j = 0; j < myKeyBoard_col_num; j++) // ��Ӧ��
		{
			if(myKeyBoard_KeyState[i][j] == KEYPRESSED)//�����������������������ݰ�
			{
				myKeyBoard_ControlKeyProcess(i,j);//���ư������� ��ʱ��Ҫ ���¾����
				if(myKeyBoard_KeyIsControl == 0)
				{
					for(k = 0; k < 5; k++)//�ж�ATKeyData���ݰ����Ƿ��Ѿ��иü���ֵ�ˣ����Ƿ�֮ǰ�Ѿ���������
					{
						if(ATKeyData[k].ATKeyData == myKeyBoard_KeyMap_ATValue[i][j])
						{
							AlreadeyExistflag = 1;
							break;
						}
					}
					if(AlreadeyExistflag == 0)//���ATKeyData���ݰ�û�иü�ֵ�����
					{
						//ѭ��	ֻҪATKeyData���ݰ��п�λ�����ü�ֵ������break��
						//�������ѭ�����Ҳû�����Ч����Ϊһ��ֻ�ܰ�����������
						for(k = 0; k < 6; k++)
						{
							if(ATKeyData[k].ATKeyData == 0x00)//������ATֵ����ATKeyData  Buffer
							{
								ATKeyData[k].ATKeyData = myKeyBoard_KeyMap_ATValue[i][j];
								ATKeyData[k].rowN = i;
								ATKeyData[k].colN = j;
								break;
							}
						}
						
						//̰�����õ�
						if(mode==2)
						{
						if(myKeyBoard_KeyMap_ATValue[i][j]==keypad_num8&&direct!=4)direct=3;//��
						if(myKeyBoard_KeyMap_ATValue[i][j]==keypad_num5&&direct!=3)direct=4;//��
						if(myKeyBoard_KeyMap_ATValue[i][j]==keypad_num4&&direct!=1)direct=2;//��
						if(myKeyBoard_KeyMap_ATValue[i][j]==keypad_num6&&direct!=2)direct=1;//��
						}
					}
					else {
						AlreadeyExistflag = 0;}
					myKeyBoard_KeyIsControl = 1;
				}
				
			}
			
		}
	}
	
	//������ֵ�ж���Ϻ󣬽��������ݰ�����
	send_data[0] = ATKeyControlByte5;	//���¿��ư����ֽ�
	for(k = 0; k < 6; k++)
	{
		send_data[2 + k] = ATKeyData[k].ATKeyData;//������ͨ����
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
	else//������϶�û���б�������ͨ��������λ���ư�����־
	{
		myKeyBoard_KeyIsControl = 0;
	}
}


void myKeyBoard_JudgeKeyStateChange()
{
	u8 i,j;
	//������״̬�Ƿ�ı�
	for(i = 0; i < myKeyBoard_row_num; i++) //��i��
	{
		for(j = 0; j < myKeyBoard_col_num; j++) //��k��
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
	
	//����ǰ�ļ���״̬���ݸ��ϴ�
	for(i = 0; i < myKeyBoard_row_num; i++)
	{
		for(j = 0; j < myKeyBoard_col_num; j++)
		{
			myKeyBoard_KeyState_Ex[i][j] = myKeyBoard_KeyState[i][j];//����ǰ�ļ���״̬���ݸ��ϴ�
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
	//n�о�Ҫn��if����ʽͬ��
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

//��ʼ������IO
void myKeyBoard_GPIO_Init()
{
	
	GPIO_InitTypeDef GPIOType;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE );

	//��ʼ�� row0-row5 GPIO
	GPIOType.GPIO_Pin = myKeyBoard_row0_GPIOPin | 
						myKeyBoard_row1_GPIOPin | 
						myKeyBoard_row2_GPIOPin | 
						myKeyBoard_row3_GPIOPin | 
						myKeyBoard_row4_GPIOPin ;
	
	GPIOType.GPIO_Speed = GPIO_Speed_50MHz;
	GPIOType.GPIO_Mode = GPIO_Mode_IPD; //��������
	GPIO_Init(myKeyBoard_row_GPIOPort , &GPIOType);
	
	//��ʼ�� col0-col3 GPIO
	GPIOType.GPIO_Pin = myKeyBoard_col0_GPIOPin | 
						myKeyBoard_col1_GPIOPin | 
						myKeyBoard_col2_GPIOPin | 
						myKeyBoard_col3_GPIOPin  
						/*myKeyBoard_col4_GPIOPin*/;

	GPIOType.GPIO_Speed = GPIO_Speed_50MHz;
	GPIOType.GPIO_Mode = GPIO_Mode_Out_PP;		//�������
	GPIO_Init(myKeyBoard_col_GPIOPort , &GPIOType);

}



