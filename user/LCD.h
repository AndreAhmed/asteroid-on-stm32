#pragma once 


#include <stdio.h>
#include <stm32f10x_rcc.h>
#include <misc.h>


void LCD_Init(void);
void LCD_WR_REG(unsigned int index);
void LCD_WR_CMD(unsigned int index,unsigned int val);

void LCD_WR_Data(unsigned int val);
void LCD_WR_Data_8(unsigned int val);
void LCD_test(void);
void LCD_clear(unsigned int p);
void ini(void);


void LCD_Flip(void);
void LCD_WriteRegister(unsigned int index,unsigned int dat);
void LCD_SetCursor(unsigned int x,unsigned int y);
void LCD_WriteIndex(unsigned int idx);
void LCD_ClearFB(void);
void LCD_Text(unsigned int x, unsigned int y, char *text, unsigned int charColor, unsigned int bkColor);
void LCD_WritetoFB(int x, int y, int color);
void LCD_Reset(void);
void Delay(__IO uint32_t nCount);
unsigned int LCD_RD_data(void);
 

/* LCD color */
#define LCD_White          0xFFFF
#define LCD_Black          0x0000
#define LCD_Blue           0x001F
#define LCD_Orange         0xFA20
#define LCD_Red            0xF800
#define LCD_Magenta        0xF81F
#define LCD_Green          0x07E0
#define LCD_Cyan           0x07FF
#define LCD_Yellow         0xFFE0


