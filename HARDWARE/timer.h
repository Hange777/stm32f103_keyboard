#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

extern u8 light_flash_flag;
extern u8 flash_flag;

void Tim3_Init(u16 arr,u16 psc); 
void Tim2_Init(u16 arr,u16 psc);
#endif
