#ifndef __SNAKE_H
#define __SNAKE_H

#include "stm32f10x_it.h"

#define Max_X 128
#define Max_Y	64

#define ringt 1;
#define left 2;
#define up 3;
#define down 4;

extern unsigned char map[];

extern unsigned int snake[128];
extern unsigned int food;
extern u8 flash_flag;
extern u8 direct;
extern u8 first_in_flag;
void map_updata_snake(unsigned char* map,unsigned int* snake);
void map_updata_food(unsigned char* map,unsigned int food);
void showmap(unsigned char* map);
void snak_init(void);
void snake_flash(u8 direct);
#endif
