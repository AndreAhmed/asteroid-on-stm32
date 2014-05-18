/**********************************************
 * Simple Game Console
 * Ahmed Saleh, xgameprogrammer@hotmail.com
 * Last Modified: 14.3.2014
 */
#include "fsmc_sram.h"
#include "ili9320.h"
#include "graphics.h"
#include "misc.h"

#include <stdio.h>
#include <inttypes.h>
#include <stdint.h>

#define SCREEN_WIDTH 180
#define SCREEN_HEIGHT 140



volatile uint32_t fractionaltimeunits = 0;




/**
 * @brief  Configure the SysTick Base time.
 * @param  None
 * @retval : None
 */
void TimingMeasurement_Config(void)
{
	RCC_ClocksTypeDef RCC_ClockFreq;

	/* Get HCLK frequency */
	RCC_GetClocksFreq(&RCC_ClockFreq);

	SysTick->CTRL &= ~SysTick_CTRL_ENABLE;

	/* Configure HCLK clock as SysTick clock source */
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);

	/* Set SysTick Load value to 0xFFFFFF */
	SysTick->LOAD = 0xFFFFFF;

	/* Disable the SysTick Interrupt */
	SysTick->CTRL &= ~SysTick_CTRL_TICKINT;

	SysTick->VAL = 0x0;
}

void StartProfiler(void)
{


	TimingMeasurement_Config();
	/* Start timing counter: measure display time */
	SysTick->CTRL |= SysTick_CTRL_ENABLE;
}


/**
 * @brief  Calculate the number of Ýs in the systick counter.
 * @param  None
 * @retval : None
 */
void DisplayTimingCompute(void)
{
	uint32_t counter = SysTick->VAL;

	if (counter != 0)
	{
		/* Convert timer ticks to ns */
		counter =  (0xFFFFFF - counter) * 111;
	}

	/* Compute timing in microsecond (us) */
	fractionaltimeunits = counter / 1000;

}

void StopProfiler(void)
{
	/* Stop timing counter */
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE;
	DisplayTimingCompute();

	char TxBuffer[20];

	sprintf(TxBuffer,"  %"PRIu32" fps", (uint32_t)(1000000 /fractionaltimeunits) );
	LCD_Text(10, 10, TxBuffer, LCD_Blue, LCD_Black);

}


void Game_Init(void)
{
	FSMC_LCD_Init();

	LCD_RCC_Configurations();
	LCD_Init_GPIO();
	LCD_Initializtion();
}


int main(void)
{


	Game_Init();

	// define points of asteroid
	VERTEX2DF asteroid_vertices[8] = {
			{33, -3}, {9, -18}, {-12, -9}, {-21, -12},
			{-9, 6}, {-15, 15}, {-3, 27}, {21, 21} };
	POLYGON2D asteroid;

	// initialize the asteroid
	asteroid.state       = 1;   // turn it on
	asteroid.num_verts   = 8;
	asteroid.x0          = SCREEN_WIDTH/2; // position it
	asteroid.y0          = SCREEN_HEIGHT/2;
	asteroid.color       = LCD_Blue;

	for ( unsigned char index = 0; index <8; index++)
		asteroid.vlist[index] = asteroid_vertices[index];

	VERTEX2DI ball = { 50, 50};
	VERTEX2DI paddle = { 20, 100};

	int vx = 5;
	int vy = 6;


	while (1) {

		LCD_ClearFB();

		//		DrawSolidTriangle((u16) asteroid_vertices[0].x,
		//				(u16) asteroid_vertices[0].y, (u16) asteroid_vertices[1].x,
		//				(u16) asteroid_vertices[1].y, (u16) asteroid_vertices[2].x,
		//				(u16) asteroid_vertices[2].y, LCD_Blue);

		//	StartProfiler();
		//	Draw_Polygon2D(&asteroid);
		//	Rotate_Polygon2D(&asteroid, 9);

		if (ball.x > SCREEN_WIDTH - 20 || ball.x < 5) {
			vx *= -1;
		}

		if (ball.y > SCREEN_HEIGHT - 20) {
			vy *= -1;
		} else if (ball.y < 5) {
			vy *= -1;
		}

		ball.x += vx;
		ball.y += vy;

		Draw_Rect(ball.x, ball.y, 10, 10, LCD_White);

		LCD_Flip();

		//StopProfiler();
	}
}





