#ifndef __WS2811_H
#define	__WS2811_H
#include "main.h"

#define 	red 						0xFF0000
#define 	orange 					0xFF6400
#define	  yellow 					0xFFFF00
#define 	green 					0x00FF00
#define 	light_blue 			0x00FFFF
#define 	blue 						0x0000FF
#define 	pink  					0xFF00FF

//spi = 1 2 3

extern u32 colour_data[];
extern u8 max_light;
extern float now_light;
extern u32 now_color;
void LED_SPI1_LowLevel_Init(void);
void LED_SPI2_LowLevel_Init(void);


void LED_SPI_SendPixel(uint32_t color,u8 spi);

void LED_SPI_SendBits(uint8_t bits,u8 spi);
void LED_SPI_WriteByte(uint16_t Data,u8 spi);

void LED_SPI_Update(u32 buffer[], uint32_t length,u8 spi);

void breath_light(u32 buffer[],uint32_t length,u8 spi);
void change_color(u32 buffer[],uint32_t length,u32 color);
#endif /* __LED_H */
