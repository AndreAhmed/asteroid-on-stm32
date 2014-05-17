/*****************************************************************
 * LCD Driver, can be used with ili9320 and ssd1289
 * Some functions are still using the direct access GRAM, needs to be rewritten
 *
 */

#include "stm32f10x.h"
#include "ili9320.h"
#include "fonts.h"

#include <math.h>

#define Bank1_LCD_D    ((uint32_t)0x60020000)    //disp Data ADDR
#define Bank1_LCD_C    ((uint32_t)0x60000000)	 //disp Reg ADDR

unsigned int LCD_DeviceCode;

#define fbWidth    180
#define fbHeight   140

#define LCD_REG              (*((volatile unsigned short *) 0x60000000)) /* RS = 0 */
#define LCD_RAM              (*((volatile unsigned short *) 0x60020000)) /* RS = 1 */

u16 frameBuffer[fbWidth * fbHeight]; // 240*100*16bit

u16 colorData = 0xf;
/* Global variable to keep the orientation-mode */
static LCD_OrientationMode_t orientation_mode = LCD_ORIENTATION_DEFAULT;

/*******************************************************************************
 * Function Name  : LCD_WriteReg
 * Description    : Writes to the selected LCD register.
 * Input          : - LCD_Reg: address of the selected register.
 *                  - LCD_RegValue: value to write to the selected register.
 * Output         : None
 * Return         : None
 * Attention		 : None
 *******************************************************************************/
static void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue) {
	/* Write 16-bit Index, then Write Reg */
	LCD_REG = LCD_Reg;
	/* Write 16-bit Reg */
	LCD_RAM = LCD_RegValue;
}

/*******************************************************************************
 * Function Name  : LCD_Delay
 * Description    : Delay Time
 * Input          : - nCount: Delay Time
 * Output         : None
 * Return         : None
 * Return         : None
 * Attention		 : None
 *******************************************************************************/
static void delay_ms(uint16_t ms) {
	uint16_t i, j;
	for (i = 0; i < ms; i++) {
		for (j = 0; j < 1141; j++)
			;
	}
}

void LCD_RCC_Configurations(void) {

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);

	RCC_APB2PeriphClockCmd(
			RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC
					| RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE
					| RCC_APB2Periph_AFIO, ENABLE);

}

void LCD_Init_GPIO(void) {
#if AHMED_BOARD
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; //D1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_6 | GPIO_Pin_3; //
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; //LCD-RST
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4
			| GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14
			| GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* Set PE.07(D4), PE.08(D5), PE.09(D6), PE.10(D7), PE.11(D8), PE.12(D9), PE.13(D10),
	 PE.14(D11), PE.15(D12) as alternate function push pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9
			| GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13
			| GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	/* NE1 configuration */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* RS */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; //RS
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* RST */

	GPIO_SetBits(GPIOD, GPIO_Pin_7); //CS=1
	GPIO_SetBits(GPIOD, GPIO_Pin_14 | GPIO_Pin_15 | GPIO_Pin_0 | GPIO_Pin_1);
	GPIO_SetBits(GPIOE, GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10);
	GPIO_ResetBits(GPIOE, GPIO_Pin_0);
	GPIO_ResetBits(GPIOE, GPIO_Pin_1); //RESET=0
	GPIO_SetBits(GPIOD, GPIO_Pin_4); //RD=1
	GPIO_SetBits(GPIOD, GPIO_Pin_5); //WR=1
#endif
#if ETIENNE_BOARD
	/* Configure the LCD Control pins (FSMC Pins) in alternate function Push-Pull mode. */
	GPIO_InitTypeDef GPIO_InitStructure;

	/* PE.07(D4), PE.08(D5), PE.09(D6), PE.10(D7), PE.11(D8), PE.12(D9),
	 PE.13(D10), PE.14(D11), PE.15(D12) */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
	GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 |
	GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	/* PD.00(D2), PD.01(D3), PD.04(RD), PD.5(WR), PD.7(CS), PD.8(D13), PD.9(D14),
	 PD.10(D15), PD.11(RS) PD.14(D0) PD.15(D1) */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7 |
	GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 |
	GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
#endif
}

/*******************************************************************************
 * Function Name  : LCD_WriteReg
 * Description    : Reads the selected LCD Register.
 * Input          : None
 * Output         : None
 * Return         : LCD Register Value.
 * Attention		 : None
 *******************************************************************************/
static uint16_t LCD_ReadReg(uint8_t LCD_Reg) {
	/* Write 16-bit Index (then Read Reg) */
	LCD_REG = LCD_Reg;
	/* Read 16-bit Reg */
	return (LCD_RAM);
}

/****************************************************************************
 * Function: initialize controller
 * Entrance Parameters: None
 * Output Parameters: None
 * Note:
 * Call the method: LCD_Initializtion ();
 ****************************************************************************/
void LCD_Initializtion() {
	LCD_DeviceCode = LCD_ReadReg(0X0000);
#if AHMED_BOARD
	LCD_WriteRegister(0x0000, 0x0001);
	LCD_DeviceCode = 0X9325;
	LCD_Reset();
#endif
	if (LCD_DeviceCode == 0X9325) {
		LCD_Delay(200);
		LCD_WriteRegister(0x0000, 0x0001);
		LCD_Delay(200);
		LCD_Delay(200);
		LCD_Reset();
		LCD_Delay(200);

		LCD_Delay(200);

		//############# void Power_Set(void) ################//
		LCD_WriteRegister(0x0000, 0x0001);
		LCD_Delay(10);

		LCD_WriteRegister(0x0015, 0x0030);
		LCD_WriteRegister(0x0011, 0x0040);
		LCD_WriteRegister(0x0010, 0x1628);
		LCD_WriteRegister(0x0012, 0x0000);
		LCD_WriteRegister(0x0013, 0x104d);
		LCD_Delay(10);
		LCD_WriteRegister(0x0012, 0x0010);
		LCD_Delay(10);
		LCD_WriteRegister(0x0010, 0x2620);
		LCD_WriteRegister(0x0013, 0x344d); //304d
		LCD_Delay(10);

		LCD_WriteRegister(0x0001, 0x0100);
		LCD_WriteRegister(0x0002, 0x0300);
		LCD_WriteRegister(0x0003, 0x1030);
		LCD_WriteRegister(0x0008, 0x0604);
		LCD_WriteRegister(0x0009, 0x0000);
		LCD_WriteRegister(0x000A, 0x0008);

		LCD_WriteRegister(0x0041, 0x0002);
		LCD_WriteRegister(0x0060, 0x2700);
		LCD_WriteRegister(0x0061, 0x0001);
		LCD_WriteRegister(0x0090, 0x0182);
		LCD_WriteRegister(0x0093, 0x0001);
		LCD_WriteRegister(0x00a3, 0x0010);
		LCD_Delay(10);

		//################# void Gamma_Set(void) ####################//
		LCD_WriteRegister(0x30, 0x0000);
		LCD_WriteRegister(0x31, 0x0502);
		LCD_WriteRegister(0x32, 0x0307);
		LCD_WriteRegister(0x33, 0x0305);
		LCD_WriteRegister(0x34, 0x0004);
		LCD_WriteRegister(0x35, 0x0402);
		LCD_WriteRegister(0x36, 0x0707);
		LCD_WriteRegister(0x37, 0x0503);
		LCD_WriteRegister(0x38, 0x1505);
		LCD_WriteRegister(0x39, 0x1505);
		LCD_Delay(10);

		//################## void Display_ON(void) ####################//
		LCD_WriteRegister(0x0007, 0x0001);
		LCD_Delay(10);
		LCD_WriteRegister(0x0007, 0x0021);
		LCD_WriteRegister(0x0007, 0x0023);
		LCD_Delay(10);
		LCD_WriteRegister(0x0007, 0x0033);
		LCD_Delay(10);
		LCD_WriteRegister(0x0007, 0x0133);

		LCD_WriteRegister(32, 0);
		LCD_WriteRegister(33, 0x013F);

	} else if (LCD_DeviceCode == 0x9320 || LCD_DeviceCode == 0x9300) {
		//LCD_Code = ILI9320;
		LCD_WriteReg(0x00, 0x0000);
		LCD_WriteReg(0x01, 0x0100); /* Driver Output Contral */
		LCD_WriteReg(0x02, 0x0700); /* LCD Driver Waveform Contral */
		LCD_WriteReg(0x03, 0x1018); /* Entry Mode Set */

		LCD_WriteReg(0x04, 0x0000); /* Scalling Contral */
		LCD_WriteReg(0x08, 0x0202); /* Display Contral */
		LCD_WriteReg(0x09, 0x0000); /* Display Contral 3.(0x0000) */
		LCD_WriteReg(0x0a, 0x0000); /* Frame Cycle Contal.(0x0000) */
		LCD_WriteReg(0x0c, (1 << 0)); /* Extern Display Interface Contral */
		LCD_WriteReg(0x0d, 0x0000); /* Frame Maker Position */
		LCD_WriteReg(0x0f, 0x0000); /* Extern Display Interface Contral 2. */

		delay_ms(100); /* delay 100 ms */
		LCD_WriteReg(0x07, 0x0101); /* Display Contral */
		delay_ms(100); /* delay 100 ms */

		LCD_WriteReg(0x10,
				(1 << 12) | (0 << 8) | (1 << 7) | (1 << 6) | (0 << 4)); /* Power Control 1.(0x16b0)	*/
		LCD_WriteReg(0x11, 0x0007); /* Power Control 2 */
		LCD_WriteReg(0x12, (1 << 8) | (1 << 4) | (0 << 0)); /* Power Control 3.(0x0138)	*/
		LCD_WriteReg(0x13, 0x0b00); /* Power Control 4 */
		LCD_WriteReg(0x29, 0x0000); /* Power Control 7 */

		LCD_WriteReg(0x2b, (1 << 14) | (1 << 4));

		LCD_WriteReg(0x50, 0); /* Set X Start */
		LCD_WriteReg(0x51, 239); /* Set X End */
		LCD_WriteReg(0x52, 0); /* Set Y Start */
		LCD_WriteReg(0x53, 319); /* Set Y End */

		LCD_WriteReg(0x60, 0x2700); /* Driver Output Control */
		LCD_WriteReg(0x61, 0x0001); /* Driver Output Control */
		LCD_WriteReg(0x6a, 0x0000); /* Vertical Srcoll Control */

		LCD_WriteReg(0x80, 0x0000); /* Display Position? Partial Display 1 */
		LCD_WriteReg(0x81, 0x0000); /* RAM Address Start? Partial Display 1 */
		LCD_WriteReg(0x82, 0x0000); /* RAM Address End-Partial Display 1 */
		LCD_WriteReg(0x83, 0x0000); /* Displsy Position? Partial Display 2 */
		LCD_WriteReg(0x84, 0x0000); /* RAM Address Start? Partial Display 2 */
		LCD_WriteReg(0x85, 0x0000); /* RAM Address End? Partial Display 2 */

		LCD_WriteReg(0x90, (0 << 7) | (16 << 0)); /* Frame Cycle Contral.(0x0013)	*/
		LCD_WriteReg(0x92, 0x0000); /* Panel Interface Contral 2.(0x0000) */
		LCD_WriteReg(0x93, 0x0001); /* Panel Interface Contral 3. */
		LCD_WriteReg(0x95, 0x0110); /* Frame Cycle Contral.(0x0110)	*/
		LCD_WriteReg(0x97, (0 << 8));
		LCD_WriteReg(0x98, 0x0000); /* Frame Cycle Contral */

		LCD_WriteReg(0x07, 0x0173);
	} else if (LCD_DeviceCode == 0x8989) {
		//LCD_Code = SSD1289;
		LCD_WriteReg(0x0000, 0x0001);
		delay_ms(50); /* æ‰“å¼€æ™¶æŒ¯ */
		LCD_WriteReg(0x0003, 0xA8A4);
		delay_ms(50);
		LCD_WriteReg(0x000C, 0x0000);
		delay_ms(50);
		LCD_WriteReg(0x000D, 0x080C);
		delay_ms(50);
		LCD_WriteReg(0x000E, 0x2B00);
		delay_ms(50);
		LCD_WriteReg(0x001E, 0x00B0);
		delay_ms(50);
		LCD_WriteReg(0x0001, 0x2B3F);
		delay_ms(50); /* é©±åŠ¨è¾“å‡ºæŽ§åˆ¶320*240 0x2B3F */
		LCD_WriteReg(0x0002, 0x0600);
		delay_ms(50);
		LCD_WriteReg(0x0010, 0x0000);
		delay_ms(50);
		LCD_WriteReg(0x0011, 0x6070);
		delay_ms(50); /* å®šä¹‰æ•°æ�®æ ¼å¼� 16ä½�è‰² æ¨ªå±� 0x6070 */
		LCD_WriteReg(0x0005, 0x0000);
		delay_ms(50);
		LCD_WriteReg(0x0006, 0x0000);
		delay_ms(50);
		LCD_WriteReg(0x0016, 0xEF1C);
		delay_ms(50);
		LCD_WriteReg(0x0017, 0x0003);
		delay_ms(50);
		LCD_WriteReg(0x0007, 0x0133);
		delay_ms(50);
		LCD_WriteReg(0x000B, 0x0000);
		delay_ms(50);
		LCD_WriteReg(0x000F, 0x0000);
		delay_ms(50); /* æ‰«æ��å¼€å§‹åœ°å�€ */
		LCD_WriteReg(0x0041, 0x0000);
		delay_ms(50);
		LCD_WriteReg(0x0042, 0x0000);
		delay_ms(50);
		LCD_WriteReg(0x0048, 0x0000);
		delay_ms(50);
		LCD_WriteReg(0x0049, 0x013F);
		delay_ms(50);
		LCD_WriteReg(0x004A, 0x0000);
		delay_ms(50);
		LCD_WriteReg(0x004B, 0x0000);
		delay_ms(50);
		LCD_WriteReg(0x0044, 0xEF00);
		delay_ms(50);
		LCD_WriteReg(0x0045, 0x0000);
		delay_ms(50);
		LCD_WriteReg(0x0046, 0x013F);
		delay_ms(50);
		LCD_WriteReg(0x0030, 0x0707);
		delay_ms(50);
		LCD_WriteReg(0x0031, 0x0204);
		delay_ms(50);
		LCD_WriteReg(0x0032, 0x0204);
		delay_ms(50);
		LCD_WriteReg(0x0033, 0x0502);
		delay_ms(50);
		LCD_WriteReg(0x0034, 0x0507);
		delay_ms(50);
		LCD_WriteReg(0x0035, 0x0204);
		delay_ms(50);
		LCD_WriteReg(0x0036, 0x0204);
		delay_ms(50);
		LCD_WriteReg(0x0037, 0x0502);
		delay_ms(50);
		LCD_WriteReg(0x003A, 0x0302);
		delay_ms(50);
		LCD_WriteReg(0x003B, 0x0302);
		delay_ms(50);
		LCD_WriteReg(0x0023, 0x0000);
		delay_ms(50);
		LCD_WriteReg(0x0024, 0x0000);
		delay_ms(50);
		LCD_WriteReg(0x0025, 0x8000);
		delay_ms(50);
		LCD_WriteReg(0x004f, 0);
		LCD_WriteReg(0x004e, 0);
	}

	*(__IO uint16_t *) (Bank1_LCD_C) = 34;
	for (unsigned long color1 = 0; color1 < 76800; color1++) {
		LCD_WriteData(LCD_Black);
	}

	delay_ms(50);
}

/****************************************************************************
 * Name: void LCD_SetCursor (unsigned int x, unsigned int y)
 * Function: Set the screen coordinates
 * Entry parameters: x-line coordinates
 * 					y column coordinate
 * Output Parameters: None
 * Note:
 * Call the method: LCD_SetCursor (10,10);
 ****************************************************************************/
void LCD_SetCursor(unsigned int x, unsigned int y) {
	LCD_WriteRegister(32, y);
	LCD_WriteRegister(33, 319 - x);
}

/****************************************************************************
 * Name: void LCD_SetWindows (unsigned int StartX, unsigned int StartY, unsigned int EndX, unsigned int EndY)
 * Function: Set the window region
 * Entry parameters: StartX line start coordinates
 * StartY out initial coordinates
 * EndX line end coordinates
 * EndY column coordinates of the end
 * Output Parameters: None
 * Note:
 * Call the method: LCD_SetWindows (0,0,100,100);
 ****************************************************************************/
void LCD_SetWindows(unsigned int StartX, unsigned int StartY, unsigned int EndX,
		unsigned int EndY) {
	LCD_WriteRegister(0x0050, StartY); // Horizontal GRAM Start Address
	LCD_WriteRegister(0x0051, EndX); // Horizontal GRAM End Address
	LCD_WriteRegister(0x0052, fbHeight - StartX); // Vertical GRAM Start Address
	LCD_WriteRegister(0x0053, EndY); // Vertical GRAM Start Address
}

/****************************************************************************
 * Name: void LCD_Clear (unsigned int bkColor)
 * Feature: the screen fills to the specified color, such as clear screen, then fill 0xffff
 * Entry parameters: dat fill value
 * Output Parameters: None
 * Note:
 * Call the method: LCD_Clear (0xffff);
 ****************************************************************************/
void LCD_Clear(u8 x, u16 y, u8 len, u16 wid, unsigned int bkColor) {
	if (LCD_DeviceCode == 0x9325) //
			{
		LCD_WriteRegister(0x0050, x); // Horizontal GRAM Start Address
		LCD_WriteRegister(0x0051, x + len - 1); // Horizontal GRAM End Address
		LCD_WriteRegister(0x0052, y); // Vertical GRAM Start Address
		LCD_WriteRegister(0x0053, y + wid - 1); // Vertical GRAM Start Address
		LCD_WriteIndex(34);

		uint32_t temp = (u32) len * wid;
		for (uint32_t n = 0; n < temp; n++)
			LCD_WriteData(bkColor); //
	}

}

/****************************************************************************
 * Name: unsigned int LCD_GetPoint (unsigned int x, unsigned int y)
 * Function: Get the color value of the specified coordinates
 * Entry parameters: x-line coordinates
 * Y column coordinate
 * Export parameters: the current color value coordinates
 * Note:
 * Call the method: i = LCD_GetPoint (10,10);
 ****************************************************************************/
//unsigned int LCD_GetPoint(unsigned int x,unsigned int y)
//{
//  LCD_SetCursor(x,y);
//  return (LCD_BGR2RGB(LCD_ReadRegister(0x0022)));
//}

unsigned int LCD_GetPoint(unsigned int x, unsigned int y) {
	unsigned int temp;
	LCD_SetCursor(x, y);
	Clr_Cs;
	LCD_WriteIndex(0x0022);
	Clr_nRd;
	temp = LCD_ReadData(); //dummy
	Set_nRd;
	Clr_nRd;
	temp = LCD_ReadData();
	Set_nRd;
	Set_Cs;
	if (LCD_DeviceCode != 0x7783 && LCD_DeviceCode != 0x4531)
		temp = LCD_BGR2RGB(temp);

	return (temp);
}

/****************************************************************************
 * Name: void LCD_SetPoint (unsigned int x, unsigned int y, unsigned int point)
 * Function: draw point at the specified coordinates
 * Entry parameters: x-line coordinates
 * Y column coordinate
 * Point point color
 * Output Parameters: None
 * Note:
 * Call the method: LCD_SetPoint (10,10,0 x0fe0);
 ****************************************************************************/
void LCD_SetPoint(unsigned int x, unsigned int y, unsigned int point) {
	if ((x > 320) || (y > 240))
		return;

	if (LCD_DeviceCode == 0x9325) {
		LCD_WriteRegister(0x0050, x); // Horizontal GRAM Start Address
		LCD_WriteRegister(0x0051, 0x00EF); // Horizontal GRAM End Address
		LCD_WriteRegister(0x0052, y); // Vertical GRAM Start Address
		LCD_WriteRegister(0x0053, 0x013F); // Vertical GRAM Start Address
		LCD_WriteRegister(32, x); // Horizontal GRAM Start Address
		LCD_WriteRegister(33, y); // Vertical GRAM Start Address
		LCD_WriteIndex(34);
		LCD_WriteData(point);
	}

}

/****************************************************************************
 * Name: void LCD_DrawPicture (unsigned int StartX, unsigned int StartY, unsigned int EndX, unsigned int EndY, unsigned int * pic)
 * Function: the scope of the specified coordinates display a picture
 * Entry parameters: StartX line start coordinates
 * StartY out initial coordinates of
 * EndX line end coordinates
 * EndY column coordinates of the end
 pic picture head pointer
 * Output Parameters: None
 * Note: the level of picture taking mode format scanning, 16-bit color mode
 * Call the method: LCD_DrawPicture (0,0,100,100, (unsigned int *) demo);
 ****************************************************************************/
void LCD_DrawPicture(unsigned int StartX, unsigned int StartY,
		unsigned int EndX, unsigned int EndY, unsigned int *pic) {
	unsigned int i;
	LCD_SetWindows(StartX, StartY, EndX, EndY);
	LCD_SetCursor(StartX, StartY);

	Clr_Cs;

	LCD_WriteIndex(0x0022);
	Set_Rs;
	for (i = 0; i < (EndX * EndY); i++) {
		LCD_WriteData(*pic++);
		Clr_nWr;
		Set_nWr;
	}

	Set_Cs;
}

/*******************************************************************************
 * Function Name  : LCD_DrawMonoPict
 * Description    : Displays a monocolor picture.
 * Input          : - Pict: pointer to the picture array.
 * Output         : None
 * Return         : None
 *******************************************************************************/
void LCD_DrawMonoPict(unsigned char *Pict) {
	u32 index = 0, i = 0;

	LCD_SetWindows(0, 0, 239, 319);
	LCD_SetCursor(0, 0);

	LCD_WriteRegister(0x03, 0x1018); // freeRTOS demo  ( LCD_Message.h format )

	Clr_Cs;
	LCD_WriteIndex(0x0022);
	Set_Rs;

	for (index = 0; index < 9600; index++) {
		for (i = 0; i < 8; i++) {
			if ((Pict[index] & (1 << i)) == 0x00) {
				LCD_WriteData(LCD_Blue); // Font Color
			} else {
				LCD_WriteData(LCD_Yellow); // Background Color

			}
			Clr_nWr;
			Set_nWr;
		}
	}
	//LCD_WriteRegister(0x03,0x1030);	// original scan direction
	Set_Cs;
}

/****************************************************************************
 * Name: void LCD_PutChar (unsigned int x, unsigned int y, u8 c, unsigned int charColor, unsigned int bkColor)
 * Function: specifies coordinates in a 8x16 matrix display of ascii characters
 * Entry parameters: x-line coordinates
 * Y column coordinate
 * CharColor the color of characters
 * BkColor character background color
 * Output Parameters: None
 * Note: Display range is limited to display the ascii code
 * Call the method: LCD_PutChar (10,10, 'a', 0x0000, 0xffff);
 ****************************************************************************/
void LCD_PutChar(unsigned int x, unsigned int y, u8 c, unsigned int charColor,
		unsigned int bkColor) {

#define MAX_CHAR_POSX 232
#define MAX_CHAR_POSY 304
	u8 temp;
	u8 pos, t;
	if (x > MAX_CHAR_POSX || y > MAX_CHAR_POSY)
		return;

	if (LCD_DeviceCode == 0x9325 || LCD_DeviceCode == 0x9328
			|| LCD_DeviceCode == 0x9320) {
		LCD_WriteRegister(0x0050, x); // Horizontal GRAM Start Address
		LCD_WriteRegister(0x0051, x + 5); // Horizontal GRAM End Address
		LCD_WriteRegister(0x0052, y); // Vertical GRAM Start Address
		LCD_WriteRegister(0x0053, y + 11); // Vertical GRAM Start Address
		LCD_WriteRegister(32, x);
		LCD_WriteRegister(33, y); //
		LCD_WriteIndex(34);
	}

	c = c - ' ';
	for (pos = 0; pos < 12; pos++) {
		temp = asc2_1206[c][pos];
		for (t = 0; t < 6; t++) {
			if (temp & 0x01)
				LCD_WriteData(charColor);
			else
				LCD_WriteData(bkColor);
			temp >>= 1;
		}
	}
}

void LCD_Text(unsigned int x, unsigned int y, char *text,
		unsigned int charColor, unsigned int bkColor) {
	u8 i = 0;
	//for (i=0;i<len;i++)
	while (*text) {
		LCD_PutChar((x + 8 * i), y, *text++, charColor, bkColor);
		i++;
	}
}

/****************************************************************************
 * Name: void LCD_Test ()
 * Function: testing LCD screen
 * Entry parameters: None
 * Export Parameters: None
 * Note: Display color bar test LCD screen is working
 * Call the method: LCD_Test ();
 ****************************************************************************/
void LCD_Test() {
	u8 R_data, G_data, B_data, i, j;

	LCD_SetCursor(0x00, 0x0000);
	LCD_WriteRegister(0x0050, 0x00); //GRAM horizontal start position
	LCD_WriteRegister(0x0051, 239); //GRAM horizontal end position
	LCD_WriteRegister(0x0052, 0); //Vertical GRAM Start position
	LCD_WriteRegister(0x0053, 319); //Vertical GRAM end position
	Clr_Cs;
	LCD_WriteIndex(0x0022);
	Set_Rs;
	R_data = 0;
	G_data = 0;
	B_data = 0;
	for (j = 0; j < 50; j++) //red crescendo Article
			{
		for (i = 0; i < 240; i++) {
			R_data = i / 8;
			LCD_WriteData(R_data << 11 | G_data << 5 | B_data);
			Clr_nWr;
			Set_nWr;
		}
	}
	R_data = 0x1f;
	G_data = 0x3f;
	B_data = 0x1f;
	for (j = 0; j < 50; j++) {
		for (i = 0; i < 240; i++) {
			G_data = 0x3f - (i / 4);
			B_data = 0x1f - (i / 8);
			LCD_WriteData(R_data << 11 | G_data << 5 | B_data);
			Clr_nWr;
			Set_nWr;
		}
	}
	//----------------------------------
	R_data = 0;
	G_data = 0;
	B_data = 0;
	for (j = 0; j < 50; j++) //green crescendo Article
			{
		for (i = 0; i < 240; i++) {
			G_data = i / 4;
			LCD_WriteData(R_data << 11 | G_data << 5 | B_data);
			Clr_nWr;
			Set_nWr;
		}
	}

	R_data = 0x1f;
	G_data = 0x3f;
	B_data = 0x1f;
	for (j = 0; j < 50; j++) {
		for (i = 0; i < 240; i++) {
			R_data = 0x1f - (i / 8);
			B_data = 0x1f - (i / 8);
			LCD_WriteData(R_data << 11 | G_data << 5 | B_data);
			Clr_nWr;
			Set_nWr;
		}
	}
	//----------------------------------

	R_data = 0;
	G_data = 0;
	B_data = 0;
	for (j = 0; j < 60; j++) //blue crescendo Article
			{
		for (i = 0; i < 240; i++) {
			B_data = i / 8;
			LCD_WriteData(R_data << 11 | G_data << 5 | B_data);
			Clr_nWr;
			Set_nWr;
		}
	}

	B_data = 0;
	R_data = 0x1f;
	G_data = 0x3f;
	B_data = 0x1f;

	for (j = 0; j < 60; j++) {
		for (i = 0; i < 240; i++) {
			G_data = 0x3f - (i / 4);
			R_data = 0x1f - (i / 8);
			LCD_WriteData(R_data << 11 | G_data << 5 | B_data);
			Clr_nWr;
			Set_nWr;
		}
	}
	Set_Cs;
}

/****************************************************************************
 * Name: unsigned int LCD_BGR2RGB (unsigned int c)
 * Function: RRRRRGGGGGGBBBBB to BBBBBGGGGGGRRRRR format
 * Entry parameters: c BRG color value
 * Output Parameters: RGB color value
 * Note: Internal function call
 ****************************************************************************/
unsigned int LCD_BGR2RGB(unsigned int c) {
	unsigned int r, g, b, rgb;

	b = (c >> 0) & 0x1f;
	g = (c >> 5) & 0x3f;
	r = (c >> 11) & 0x1f;

	rgb = (b << 11) + (g << 5) + (r << 0);

	return (rgb);
}

/****************************************************************************
 * Name: void LCD_WriteIndex (unsigned int idx)
 * Function: write LCD controller register address
 * Import parameter: idx register address
 * Output Parameters: None
 * Note: you must first select the controller before calling the internal function
 * Call the method: LCD_WriteIndex (0x0000);
 ****************************************************************************/
__inline void LCD_WriteIndex(unsigned int idx) {
	//	Clr_Rs;
	//	Set_nRd;
	//	LCD_WriteData(idx);
	//	Clr_nWr;
	//	Set_nWr;
	//	Set_Rs;
	*(__IO uint16_t *) (Bank1_LCD_C) = idx;
}

/****************************************************************************
 * Name: void LCD_WriteData (unsigned int dat)
 * Function: write LCD register data
 * Entry parameters: dat register data
 * Output Parameters: None
 * Note: specify the address to write data to the controller, called the former must first write register address, the internal function
 * Call the method: LCD_WriteData (0x1030)
 ****************************************************************************/
void LCD_WriteData(unsigned int data) {
	*(__IO uint16_t *) (Bank1_LCD_D) = data;
}

/****************************************************************************
 * Name: unsigned int LCD_ReadData (void)
 * Function: Read controller data
 * Entry parameters: None
 * Output Parameters: Return to read the data
 * Note: Internal function
 * Calling method: i = LCD_ReadData ();
 ****************************************************************************/
__inline unsigned int LCD_ReadData(void) {
	//========================================================================
	// **                                                                    **
	// ** nCS       ----\__________________________________________/-------  **
	// ** RS        ------\____________/-----------------------------------  **
	// ** nRD       -------------------------\_____/---------------------  **
	// ** nWR       --------\_______/--------------------------------------  **
	// ** DB[0:15]  ---------[index]----------[data]-----------------------  **
	// **                                                                    **
	//========================================================================
	unsigned int tmp;
	GPIOB->CRH = (GPIOB->CRH & 0x00000000) | 0x44444444;
	GPIOC->CRL = (GPIOC->CRL & 0x00000000) | 0x44444444;
	tmp = (((GPIOB->IDR) >> 8) | ((GPIOC->IDR) << 8));
	GPIOB->CRH = (GPIOB->CRH & 0x00000000) | 0x33333333;
	GPIOC->CRL = (GPIOC->CRL & 0x00000000) | 0x33333333;
	return tmp;
}

/****************************************************************************
 * Name: unsigned int LCD_ReadRegister (unsigned int index)
 * Function: Read the value of the specified address register
 * Entry parameters: index register address
 * Output Parameters: Register values
 * Note: Internal function
 * Call the method: i = LCD_ReadRegister (0x0022);
 ****************************************************************************/
__inline unsigned int LCD_ReadRegister(unsigned int index) {
	Clr_Cs;
	LCD_WriteIndex(index);
	Clr_nRd;
	index = LCD_ReadData();
	Set_nRd;
	Set_Cs;
	return index;
}

/****************************************************************************
 * Name: void LCD_WriteRegister (unsigned int index, unsigned int dat)
 * Function: write the value of the specified address register
 * Entry parameters: index register address
 *: Dat register value
 * Output Parameters: None
 * Note: Internal function
 * Call the method: LCD_WriteRegister (0x0000, 0x0001);
 ****************************************************************************/
void LCD_WriteRegister(unsigned int index, unsigned int dat) {
	/************************************************************************
	 **                                                                    **
	 ** nCS       ----\__________________________________________/-------  **
	 ** RS        ------\____________/-----------------------------------  **
	 ** nRD       -------------------------------------------------------  **
	 ** nWR       --------\_______/--------\_____/-----------------------  **
	 ** DB[0:15]  ---------[index]----------[data]-----------------------  **
	 **                                                                    **
	 ************************************************************************/

	*(__IO uint16_t *) (Bank1_LCD_C) = index;
	*(__IO uint16_t *) (Bank1_LCD_D) = dat;
}

/****************************************************************************
 * Name: void LCD_Reset ()
 * Function: reset ili9320 controller
 * Entry parameters: None
 * Output Parameters: None
 * Description: reset the controller, the internal function
 * Call the method: LCD_Reset ()
 ****************************************************************************/
void LCD_Reset() {
	/***************************************
	 **                                   **
	 **  -------\______________/-------   **
	 **         | <---Tres---> |          **
	 **                                   **
	 **  Tres: Min.1ms                    **
	 ***************************************/

	GPIO_ResetBits(GPIOE, GPIO_Pin_1);
	LCD_Delay(0xAFFFFf);
	GPIO_SetBits(GPIOE, GPIO_Pin_1);
	LCD_Delay(0xAFFFFf);
}

/****************************************************************************
 * Name: void LCD_BackLight (u8 status)
 * Function: open, off LCD backlight
 * Entry parameters: status 1: backlit 0: backlight off
 * Output Parameters: None
 * Note:
 * Call Method: LCD_BackLight (1);
 ****************************************************************************/
void LCD_BackLight(u8 status) {
	if (status >= 1) {
		Lcd_Light_ON;
	} else {
		Lcd_Light_OFF;
	}
}

/****************************************************************************
 * Name: void LCD_Delay (vu32 nCount)
 * Function: Delay
 * Entry parameters: nCount delay value
 * Output Parameters: None
 * Note:
 * Call the method: LCD_Delay (10000);
 ****************************************************************************/
void LCD_Delay(__IO uint32_t nCount) {
	for (; nCount != 0; nCount--)
		;
}
/*****************************************************************************************************************/

/**
 * @brief Sets the LCD orientation
 * @param m: orientation mode
 * @retval none
 */
void LCD_SetOrientation(LCD_OrientationMode_t m) {
	uint16_t em;

	switch (m) {
	case LCD_PORTRAIT_TOP_DOWN:
		em = 0x1030;
		break;
	case LCD_PORTRAIT_BOTTOM_UP:
		em = 0x1010;
		break;
	case LCD_LANDSCAPE_TOP_DOWN:
		em = 0x1018;
		break;
	case LCD_LANDSCAPE_BOTTOM_UP:
		em = 0x1008;
		break;
	default:
		em = 0x0130;
		break;
	}
	LCD_WriteRegister(0x0003, em); /* Entry Mode */
	orientation_mode = m;
	LCD_SetCursor(0x00, 0x00);
}

/**
 * @brief Get current LCD orientation
 * @param none
 * @retval orientation
 */
LCD_OrientationMode_t LCD_GetOrientation(void) {
	return orientation_mode;
}

u32 abs(s32 res) {
	if (res < 0)
		return -res;
	else
		return res;
}

static void LCD_WritetoFB(int x, int y, u16 color) {
	frameBuffer[x + y * fbWidth] = color;
}

/**
 * @brief  Displays a line.
 * @param  Xpos: specifies the X position.
 * @param  Ypos: specifies the Y position.
 * @param  Length: line length.
 * @param  Direction: line direction.
 *   This parameter can be one of the following values: LCD_LINE_VERTICAL or Horizontal.
 * @retval None
 */
void LCD_DrawLine(float x1, float y1, float x2, float y2, u16 color) {
	float xdiff = (x2 - x1);
	float ydiff = (y2 - y1);

	if (xdiff == 0.0f && ydiff == 0.0f) {
		LCD_WritetoFB((int) x1, (int) y1, color);
		return;
	}

	if (fabs(xdiff) > fabs(ydiff)) {
		float xmin, xmax;

		// set xmin to the lower x value given
		// and xmax to the higher value
		if (x1 < x2) {
			xmin = x1;
			xmax = x2;
		} else {
			xmin = x2;
			xmax = x1;
		}

		// draw line in terms of y slope
		float slope = ydiff / xdiff;
		float x;
		for (x = xmin; x <= xmax; x += 1.0f) {
			float y = y1 + ((x - x1) * slope);
			LCD_WritetoFB((int) x, (int) y, color);
		}
	} else {
		float ymin, ymax;

		// set ymin to the lower y value given
		// and ymax to the higher value
		if (y1 < y2) {
			ymin = y1;
			ymax = y2;
		} else {
			ymin = y2;
			ymax = y1;
		}

		// draw line in terms of x slope
		float slope = xdiff / ydiff;
		for (float y = ymin; y <= ymax; y += 1.0f) {
			float x = x1 + ((y - y1) * slope);
			LCD_WritetoFB((int) x, (int) y, color);
		}
	}
}

/**
 * @brief  Displays a rectangle.
 * @param  Xpos: specifies the X position.
 * @param  Ypos: specifies the Y position.
 * @param  Height: display rectangle height.
 * @param  Width: display rectangle width.
 * @retval None
 */
void LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint16_t Height, uint16_t Width,
		u16 color) {
	// must do clippign here
	for (int ymin = Ypos; ymin < Ypos + Height; ymin++) {
		for (int xmin = Xpos; xmin < Xpos + Width; xmin++) {

			LCD_WritetoFB(xmin, ymin, color);
		}
	}
}

/**
 * @brief  Displays a box (filled rectangle).
 * @param  Xpos: specifies the X position.
 * @param  Ypos: specifies the Y position.
 * @param  Height: display rectangle height.
 * @param  Width: display rectangle width.
 * @retval None
 */
void LCD_DrawBox(uint16_t Xpos, uint16_t Ypos, uint16_t Height, uint16_t Width) {
	for (int32_t CurY = Ypos; CurY < (Ypos + Height); CurY++) {
		LCD_DrawLine(Xpos, CurY, Width, LCD_LINE_HORIZONTAL, LCD_Red);
	}
}

void swap(int *i, int *j) {
	int t = *i;
	*i = *j;
	*j = t;
}

void DrawSolidTriangle(int x0, int y0, int x1, int y1, int x2, int y2,
		int color) {
	// Sort our points into order of y
	// 0 top
	// 2 middle
	// 1 bottom
	if (y1 < y0) {
		swap(&y1, &y0);
		swap(&x1, &x0);
	}
	if (y2 < y0) {
		swap(&y2, &y0);
		swap(&x2, &x0);
	}
	if (y1 < y2) {
		swap(&y2, &y1);
		swap(&x2, &x1);
	}

	float xl_edge = (float) x0; // left edge
	float xr_edge = (float) x0; // right edge

	float dxldy;
	float dxrdy;

	float dxdy1 = (float) (x2 - x0) / (y2 - y0);
	float dxdy2 = (float) (x1 - x0) / (y1 - y0);

	if (dxdy1 < dxdy2) {
		dxldy = dxdy1;
		dxrdy = dxdy2;
	} else {
		dxldy = dxdy2;
		dxrdy = dxdy1;
	}

	// Top of the triangle
	for (int y = y0; y < y2; y++) {
		int x;
		for (x = xl_edge; x < xr_edge; x++) {
			LCD_WritetoFB(x, y, color);

		} //end for loop x

		xl_edge = xl_edge + dxldy;
		xr_edge = xr_edge + dxrdy;

	} // end for loop y

	  // Bottom half of the triangle

	if (dxdy1 < dxdy2) {
		dxldy = (float) (x2 - x1) / (y2 - y1);
	} else {
		dxrdy = (float) (x2 - x1) / (y2 - y1);
	}

	for (int y = y2; y < y1; y++) {
		int x;
		for (x = xl_edge; x < xr_edge; x++) {
			LCD_WritetoFB(x, y, color);
		} //end for loop x

		xl_edge = xl_edge + dxldy;
		xr_edge = xr_edge + dxrdy;

	} // end for loop y
}

void LCD_ClearFB(void) {
	for (uint16_t pixel = 0; pixel < fbHeight * fbWidth; pixel++) {
		frameBuffer[pixel] = 0x0;

	}

}

void buf(void) {
	for (int i = 0; i < 1000; i++) {
		*(__IO uint16_t *) (Bank1_LCD_D) = 0x00;
	}

}
void LCD_Flip(void) {
	//LCD_SetCursor(0x00, 0x0000);

	LCD_WriteRegister(32, 0);
	LCD_WriteRegister(33, 0);
	LCD_WriteRegister(0x0050, 0x00); //GRAM horizontal start position
	LCD_WriteRegister(0x0051, fbWidth - 1); //GRAM horizontal end position
	LCD_WriteRegister(0x0052, 0); //Vertical GRAM Start position
	LCD_WriteRegister(0x0053, fbHeight - 1); //Vertical GRAM end position
	LCD_WriteIndex(0x0022);

	int i;

	for (i = 0; i < fbWidth * fbHeight; i++) {

		*(__IO uint16_t *) (Bank1_LCD_D) = frameBuffer[i];

	}

}

