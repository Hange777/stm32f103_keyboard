#ifndef __RNG_H
#define __RNG_H
//这个随机数是adc测来的
#include "stm32f10x_it.h"

void  my_RNG_Init(void);
u16 Get_RNG(void);

#endif
