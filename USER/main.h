#ifndef __MAIN_H
#define __MAIN_H
/***************包含的头文件****************/
//f1
#include "stm32f10x_it.h"

//usb
#include "my_usb_init.h"
#include "hw_config.h"

//按键
#include "KeyBoard.h"

//杂项
#include "delay.h"
#include "oled.h"
#include "WS2811.h"
#include "rng.h"
#include "timer.h"
#include "snake.h"
/******************************************/
/*****************宏定义*******************/

//按键
/*****************************
改的话需要修改
myKeyBoard_KeyScan_rowScan()
myKeyBoard_GPIO_Init()
key_Col_Pin()
myKeyBoard_row_GPIOPort
myKeyBoard_col_GPIOPort
键值在myKeyBoard_KeyMap_ATValue
*******************************/

#define myKeyBoard_row_num 5 //行个数
#define myKeyBoard_col_num 4 //列个数

#define led_num 2


#endif
