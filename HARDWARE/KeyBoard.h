#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "stm32f10x.h"
#include "string.h"
#include "delay.h"


#define myKeyBoard_row_GPIOPort		GPIOB

#define myKeyBoard_row0_GPIOPin		GPIO_Pin_3
//#define myKeyBoard_row1_GPIOPin		GPIO_Pin_4
#define myKeyBoard_row1_GPIOPin		GPIO_Pin_8
#define myKeyBoard_row2_GPIOPin		GPIO_Pin_5
#define myKeyBoard_row3_GPIOPin		GPIO_Pin_6
#define myKeyBoard_row4_GPIOPin		GPIO_Pin_7


#define myKeyBoard_col_GPIOPort		GPIOA

#define myKeyBoard_col0_GPIOPin		GPIO_Pin_0
#define myKeyBoard_col1_GPIOPin		GPIO_Pin_1
#define myKeyBoard_col2_GPIOPin		GPIO_Pin_4
#define myKeyBoard_col3_GPIOPin		GPIO_Pin_5
//#define myKeyBoard_col4_GPIOPin		GPIO_Pin_6

///* 26个字母的数组*/
//const char keyboard_26[26]= {
//	0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0X0B,0X0C,0X0D,
//	0X0E,0X0F,0X10,0X11,0X12,0X13,0X14,0X15,0X16,0X17,
//	0X18,0X19,0X11,0X1B,0X1C,0X1D
//};
#define  keyboard_A  0x04
#define  keyboard_B  0x05
#define  keyboard_C  0x06
#define  keyboard_D  0x07
#define  keyboard_E  0x08
#define  keyboard_F  0x09
#define  keyboard_G  0x0A
#define  keyboard_H  0x0B
#define  keyboard_I  0x0C
#define  keyboard_J  0x0D
#define  keyboard_K  0x0E
#define  keyboard_L  0x0F
#define  keyboard_M  0x10
#define  keyboard_N  0x11
#define  keyboard_O  0x12
#define  keyboard_P  0x13
#define  keyboard_Q  0x14
#define  keyboard_R  0x15
#define  keyboard_S  0x16
#define  keyboard_T  0x17
#define  keyboard_U  0x18
#define  keyboard_V  0x19
#define  keyboard_W  0x1A
#define  keyboard_X  0x1B
#define  keyboard_Y  0x1C
#define  keyboard_Z  0x1D

///* 最上面一行,不含波浪号*/
//#define  keyboard_12[12] = {
//	0x1E,0X1F,0X20,0X21,0X22,0X23,0X24,0X25,0x26,0x27,0x2D,0X2E
///*对应 1   2    3    4     5   6    7    8    9   0    -    + 
//*/
//};
#define  keyboard_1  0x1E
#define  keyboard_2  0x1F
#define  keyboard_3  0x20
#define  keyboard_4  0x21
#define  keyboard_5  0x22
#define  keyboard_6  0x23
#define  keyboard_7  0x24
#define  keyboard_8  0x25
#define  keyboard_9  0x26
#define  keyboard_10 0x27
#define  keyboard_11 0x2D
#define  keyboard_12 0x2E

#define  keyboard_Enter  0x28
#define  keyboard_Esc  0x29
#define  keyboard_Backpace  0x2A // backspace 删除键
#define  keyboard_Tab  0x2B
#define  keyboard_Caps  0x39
#define  keyboard_leftControl  0xE0
#define  keyboard_leftShift  0xE1
#define  keyboard_leftAlt  0xE2
#define  keyboard_rightControl  0xE4
#define  keyboard_rightShift  0xE5
#define  keyboard_rightAlt  0xE6
#define  keyboard_Wave  0x35 //`~左上角这个波浪号
#define  keyboard_Space 0x2c //空格
#define  keyboard_Win  0xE3 

///* 标号 ,[]|;'<>?       */
//#define  keyboard_sign[8] = {
//	0X2F,0X30,0X31,0X33,0X34,0X36,0X37 0x38
///*    [    ]   |     ;    '   <    >   ?  */
//};
#define  keyboard_sign1  0x2f
#define  keyboard_sign2  0x30
#define  keyboard_sign3  0x31
#define  keyboard_sign4  0x33
#define  keyboard_sign5  0x34
#define  keyboard_sign6  0x36
#define  keyboard_sign7  0x37
#define  keyboard_sign8  0x38

#define  keyboard_F1  0x3A
#define  keyboard_F2  0x3B
#define  keyboard_F3  0x3C
#define  keyboard_F4  0x3D
#define  keyboard_F5  0x3E
#define  keyboard_F6  0x3F
#define  keyboard_F7  0x40
#define  keyboard_F8  0x41
#define  keyboard_F9  0x42
#define  keyboard_F10 0x43
#define  keyboard_F11 0x44
#define  keyboard_F12 0x45

///* 上下左右 */
//#define  keyboard_arrow[4] =
//{
//	0x52,0x51,0x50,0x4f
////    上   下  左   右
//};
#define  keyboard_UpArrow  0x52
#define  keyboard_DownArrow 0x51
#define  keyboard_LeftArrow  0x50
#define  keyboard_RightArrow  0x4f



/**************************************************
小键盘的                                          */
#define keypad_chu 0x54 //   /
#define keypad_cheng 0x55 // *
#define keypad_jian 0x56 //  -
#define keypad_jia 0x57 //   +

#define keypad_ENTER 0x58 
#define keypad_num1 0x59
#define keypad_num2 0x5A 
#define keypad_num3 0x5B 
#define keypad_num4 0x5C 
#define keypad_num5 0x5D 
#define keypad_num6 0x5E 
#define keypad_num7 0x5F 
#define keypad_num8 0x60 
#define keypad_num9 0x61 
#define keypad_num0 0x62
#define keypad_dot 0x63 // 小数点 .


#define keypad_Volume_Up 0x80 // 音量 加
#define keypad_Volume_Down 0x81 // 音量 减  没卵用 不触发

#define keyboard_Home 0x4A
#define keyboard_End 0x4D 
#define Keyboard_PageUp 0X4B
#define Keyboard_PageDown   0X4E


#define  keyboard_Delete_Forward  0x4c  // Delete
#define  keyboard_Insert  0x49 // insert
#define  keyboard_Fn 0x00



extern u8 g_USBModeFlag ; // TRUE:USB模式  FALSE:蓝牙模式
extern u8 ws2812b_led_control ; // 用来切换灯效的
extern u32 WsDat1[25];
extern unsigned char send_data[8];
extern unsigned char send_data_to_lcd[6];
extern u8 mode;
typedef enum 
{
	KEYUNPRESSED= 0,
	KEYPRESSED  = 1
}KeyState_enumTypedef;//按键状态


extern unsigned char g_myKeyBoard_DataWaitForUploadFlag;//数据等待上传标志
extern unsigned char ATKeyControlByte5 ;//状态控制键字节 如Shift Ctrl键等


void myKeyBoard_GPIO_Init(void);//按键IO初始化
void myKeyBoard_KeyScan(void);//物理层键盘状态扫描，按键是否被按下 

void myKeyBoard_JudgeKeyStateChange(void);//判断键盘状态是否有变化，有则置位myKeyBoard_KeyStateChangedFlag标志
void myKeyBoard_UpdataATDataPack(void);//键盘AT数据包更新
void myKeyBoard_ScanKeyAndUpdataATBuffer(void);//键盘扫描并更新Key数据包
unsigned char myKeyBoard_JudgeKeyPressWithName(const char* keyName);
void myKeyBoard_Process_All(void);//键盘中心程序

#endif


