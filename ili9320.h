#ifndef __ili9320_H__
#define __ili9320_H__

#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stdint.h>

#define AHMED_BOARD 1
#define ETIENNE_BOARD !AHMED_BOARD

#define Set_Cs  GPIOD->BSRR = GPIO_Pin_7;
#define Clr_Cs  GPIOD->BRR = GPIO_Pin_7;

#define Set_Rs  GPIOD->BSRR = GPIO_Pin_11;
#define Clr_Rs  GPIOD->BRR = GPIO_Pin_11;

#define Set_nWr GPIOD->BSRR = GPIO_Pin_5;
#define Clr_nWr GPIOD->BRR = GPIO_Pin_5;

#define Set_nRd GPIOD->BSRR = GPIO_Pin_4;
#define Clr_nRd GPIOD->BRR = GPIO_Pin_4;

#define Set_Rst GPIOE->BSRR = GPIO_Pin_1;
#define Clr_Rst GPIOE->BRR = GPIO_Pin_1;

#define Lcd_Light_ON   GPIOC->BSRR = GPIO_Pin_13;
#define Lcd_Light_OFF  GPIOC->BRR = GPIO_Pin_13;


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

/**
 * @brief  LCD orientation and limits
 */

typedef enum {
	LCD_PORTRAIT_TOP_DOWN,
	LCD_PORTRAIT_BOTTOM_UP,
	LCD_LANDSCAPE_TOP_DOWN,
	LCD_LANDSCAPE_BOTTOM_UP,
} LCD_OrientationMode_t;

#define LCD_WIDTH_HW         240
#define LCD_HEIGHT_HW        320

#define LCD_PORTRAIT           0
#define LCD_LANDSCAPE          1
// #define LCD_ORIENTATION        LCD_PORTRAIT
#define LCD_ORIENTATION        LCD_LANDSCAPE

#if LCD_ORIENTATION == LCD_PORTRAIT
	#define LCD_WIDTH              240
	#define LCD_HEIGHT             320
	#define LCD_CHARS_PER_LINE      10
	#define LCD_CHAR_LINES          13
	#define LCD_ENTRY_MODE_DEFAULT 0x1030 /* Entry Mode BGR=1,AM=0,ID1/0=1/1 */
	#define LCD_ORIENTATION_DEFAULT LCD_PORTRAIT_TOP_DOWN
#else
	#define LCD_WIDTH              320
	#define LCD_HEIGHT             240
	#define LCD_CHARS_PER_LINE      15
	#define LCD_CHAR_LINES          10
	#define LCD_ENTRY_MODE_DEFAULT 0x1018 /* Entry Mode BGR=1,AM=1,ID1/0=1/0 */
	#define LCD_ORIENTATION_DEFAULT LCD_LANDSCAPE_TOP_DOWN
#endif

#define LCD_LINE_HORIZONTAL     0x00
#define LCD_LINE_VERTICAL       0x01

/* Exported_Functions */
void LCD_Configuration(void);
void LCD_Initializtion(void);
void LCD_Reset(void);
void LCD_WriteRegister(unsigned int index,unsigned int dat);
void LCD_SetCursor(unsigned int x,unsigned int y);
void LCD_SetWindows(unsigned int StartX,unsigned int StartY,unsigned int EndX,unsigned int EndY);
void LCD_DrawPicture(unsigned int StartX,unsigned int StartY,unsigned int EndX,unsigned int EndY,unsigned int *pic);
void LCD_SetPoint(unsigned int x,unsigned int y,unsigned int point);
void LCD_PutChar(unsigned int x,unsigned int y,u8 c,unsigned int charColor,unsigned int bkColor);
void LCD_Text(unsigned int x, unsigned int y, char *text, unsigned int charColor, unsigned int bkColor);
void LCD_Clear(u8 x,u16 y,u8 len,u16 wid, unsigned int bkColor);
void LCD_Delay(volatile uint32_t nCount);
void LCD_Test(void);
void LCD_WriteData(unsigned int dat);
void LCD_WriteIndex(unsigned int idx);
void LCD_RCC_Configurations(void);
void LCD_Init_GPIO(void);
void LCD_ClearFB(void);
void LCD_Flip(void);

void LCD_DrawMonoPict(unsigned char *Pict);

void LCD_BackLight(u8 status);

unsigned int LCD_BGR2RGB(unsigned int c);

unsigned int LCD_GetPoint(unsigned int x,unsigned int y);
unsigned int LCD_ReadData(void);
unsigned int LCD_ReadRegister(unsigned int index);

static inline uint16_t LCD_RGB2Pixel565(uint8_t r, uint8_t g, uint8_t b)
{
	/* RGB2PIXEL565 from a Macro in Greg Haerr's Nano-X, MPL license */
	return ((((r) & 0xf8) << 8) | (((g) & 0xfc) << 3) | (((b) & 0xf8) >> 3));
}

void LCD_SetOrientation(LCD_OrientationMode_t m);
LCD_OrientationMode_t LCD_GetOrientation(void);
uint16_t LCD_GetWidth(void);
uint16_t LCD_GetHeight(void);

void LCD_DrawLine(float x1, float y1, float x2, float y2, u16 color);
void LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint16_t Height, uint16_t Width, u16 color);
void LCD_DrawBox(uint16_t Xpos, uint16_t Ypos, uint16_t Height, uint16_t Width);

void DrawSolidTriangle(int x0, int y0, int x1, int y1, int x2, int y2, int color );

typedef struct Vertex2DF_TYP
{
	float x,y;

}Vertex2DF, *Vertex2DF_PTR;



#endif	// __MINISTM32_LCD_H__

