#ifndef __MAIN_H
#define __MAIN_H
/***************������ͷ�ļ�****************/
//f1
#include "stm32f10x_it.h"

//usb
#include "my_usb_init.h"
#include "hw_config.h"

//����
#include "KeyBoard.h"

//����
#include "delay.h"
#include "oled.h"
#include "WS2811.h"
#include "rng.h"
#include "timer.h"
#include "snake.h"
/******************************************/
/*****************�궨��*******************/

//����
/*****************************
�ĵĻ���Ҫ�޸�
myKeyBoard_KeyScan_rowScan()
myKeyBoard_GPIO_Init()
key_Col_Pin()
myKeyBoard_row_GPIOPort
myKeyBoard_col_GPIOPort
��ֵ��myKeyBoard_KeyMap_ATValue
*******************************/

#define myKeyBoard_row_num 5 //�и���
#define myKeyBoard_col_num 4 //�и���

#define led_num 2


#endif
