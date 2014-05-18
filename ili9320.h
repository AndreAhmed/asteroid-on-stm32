#ifndef __ili9320_H__
#define __ili9320_H__

#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stdint.h>

#define AHMED_BOARD 1
#define ETIENNE_BOARD !AHMED_BOARD

/* LCD color */
typedef enum {
	LCD_White = 0xFFFF,
	LCD_Black = 0x0000,
	LCD_Blue = 0x001F,
	LCD_Orange = 0xFA20,
	LCD_Red = 0xF800,
	LCD_Magenta = 0xF81F,
	LCD_Green = 0x07E0,
	LCD_Cyan = 0x07FF,
	LCD_Yellow = 0xFFE0
} LCD_Color_t;


/* Exported_Functions */
void LCD_Configuration(void);
void LCD_Initializtion(void);
void LCD_Reset(void);
void LCD_WriteRegister(unsigned int index,unsigned int dat);
void LCD_SetCursor(unsigned int x,unsigned int y);
void LCD_SetWindows(unsigned int StartX,unsigned int StartY,unsigned int EndX,unsigned int EndY);
void LCD_PutChar(unsigned int x,unsigned int y,u8 c,unsigned int charColor,unsigned int bkColor);
void LCD_Text(unsigned int x, unsigned int y, char *text, unsigned int charColor, unsigned int bkColor);
void LCD_Clear(u8 x,u16 y,u8 len,u16 wid, unsigned int bkColor);
void LCD_Delay(volatile uint32_t nCount);
void LCD_WriteData(unsigned int dat);
void LCD_WriteIndex(unsigned int idx);
void LCD_RCC_Configurations(void);
void LCD_Init_GPIO(void);
void LCD_ClearFB(void);
void LCD_Flip(void);
void LCD_WritetoFB(int x, int y, u16 color);

void LCD_DrawMonoPict(unsigned char *Pict);



unsigned int LCD_BGR2RGB(unsigned int c);

unsigned int LCD_GetPoint(unsigned int x,unsigned int y);
unsigned int LCD_ReadData(void);
unsigned int LCD_ReadRegister(unsigned int index);

static inline uint16_t LCD_RGB2Pixel565(uint8_t r, uint8_t g, uint8_t b)
{
	/* RGB2PIXEL565 from a Macro in Greg Haerr's Nano-X, MPL license */
	return ((((r) & 0xf8) << 8) | (((g) & 0xfc) << 3) | (((b) & 0xf8) >> 3));
}



#endif	// __MINISTM32_LCD_H__

