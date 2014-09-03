 
	/* Includes ------------------------------------------------------------------*/
	#include "fsmc_sram.h"
	#include "graphics.h"
	#include "lcd.h"

	GPIO_InitTypeDef GPIO_InitStructure;

	/* Private function prototypes -----------------------------------------------*/
	void RCC_Configuration(void);
	void GPIO_Configuration(void);
	 
	void lcd_rst(void);
	 
	void Delay(__IO uint32_t nCount);
	GPIO_InitTypeDef GPIO_InitStructure;
 

	volatile uint32_t fractionaltimeunits = 0;

			
	char TxBuffer[20];


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

	}



	/* Private functions ---------------------------------------------------------*/

	/**
		* @brief  Main program.
		* @param  None
		* @retval : None
		*/
	int main(void)
	{
			
		unsigned char index;

		
	//	POLYGON2D asteroid;
		POLYGON3D triangle;
		// define points of asteroid
//		VERTEX2DF asteroid_vertices[8] = 
//		{
//				{33, -3}, {9, -18}, {-12, -9}, {-21, -12},
//				{-9, 6}, {-15, 15}, {-3, 27}, {21, 21} 
//		};
//		
		
		 VERTEX3DF triangle_verts[3] = 
	  { 0, 2, 0, 
   	 -5, -5, 0,
	  5, -5, 0
	  };
		/* System Clocks Configuration */
		RCC_Configuration();   

		/* Enable the FSMC Clock */
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
	
		GPIO_Configuration();
		
		/* Configure FSMC Bank1 NOR/PSRAM */
		FSMC_LCD_Init();

		LCD_Init();

//	
//		// initialize the asteroid
//		asteroid.state       = 1;   // turn it on
//		asteroid.num_verts   = 8;
//		asteroid.x0          = 180/2; // position it
//		asteroid.y0          = 140/2;
//		asteroid.color       = LCD_Blue;

//		for ( index = 0; index <8; index++)
//		{
//			asteroid.vlist[index] = asteroid_vertices[index];
//		}
		 
		triangle.state = 1;   // turn it on
		triangle.num_verts = 3;
		triangle.x0 = WINDOW_WIDTH/2; // position it
		triangle.y0 = WINDOW_HEIGHT/2;
		triangle.color = LCD_Blue;
		triangle.world_x = 0;
		triangle.world_y = 0;
		triangle.world_z = 150; // set world position
		
		for ( index = 0; index <3; index++)
			triangle.vlist[index] = triangle_verts[index];
	  
		while (1)
		{					  
	 			
	  StartProfiler();
		 
		LCD_ClearFB();		
		
	 //  	Draw_Polygon2D(&asteroid);
	//   	Rotate_Polygon2D(&asteroid, 2);
			
	 if(!GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5))        //S1
   {
     triangle.world_z+=1;
	 }

   if(!GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4))       //S2
   {
		triangle.world_z-=1;
   }
			
  	Rotate_Polygon3D_YAxis(&triangle, 1);
    Transform_WorldToCamera(&triangle);
   	Draw_Polygon3D(&triangle);
			
		LCD_Flip();	
 
		StopProfiler();
		sprintf(TxBuffer," %lu fps", (unsigned long)(1000000 /fractionaltimeunits) );
 		LCD_Text(10, 10, TxBuffer, LCD_Red, LCD_Black);

		}
	}

	/**
		* @brief  Configures the different system clocks.
		* @param  None
		* @retval : None
		*/
	void RCC_Configuration(void)
	{   
		/* Setup the microcontroller system. Initialize the Embedded Flash Interface,  
			 initialize the PLL and update the SystemFrequency variable. */
		SystemInit();
	}											

	void GPIO_Configuration(void){
		GPIO_InitTypeDef GPIO_InitStructure;
		
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |
													 RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE , ENABLE); 	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;				     //D1
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOC, &GPIO_InitStructure);					 
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 |GPIO_Pin_6|GPIO_Pin_3;		 //
		GPIO_Init(GPIOD, &GPIO_InitStructure);

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;		 //
		GPIO_Init(GPIOE, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ; 	 //LCD-RST
		GPIO_Init(GPIOE, &GPIO_InitStructure);  	
		
	 
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 |
																	GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 | 
																	GPIO_Pin_15;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOD, &GPIO_InitStructure);

		/* Set PE.07(D4), PE.08(D5), PE.09(D6), PE.10(D7), PE.11(D8), PE.12(D9), PE.13(D10),
			 PE.14(D11), PE.15(D12) as alternate function push pull */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
																	GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
																	GPIO_Pin_15;
		GPIO_Init(GPIOE, &GPIO_InitStructure); 

		
		/* NE1 configuration */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 
		GPIO_Init(GPIOD, &GPIO_InitStructure);
		
		/* RS */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 ; 	  //RS
		GPIO_Init(GPIOD, &GPIO_InitStructure); 
		
		/* RST */
	 	
		GPIO_SetBits(GPIOD, GPIO_Pin_7);			//CS=1 
		GPIO_SetBits(GPIOD, GPIO_Pin_14| GPIO_Pin_15 |GPIO_Pin_0 | GPIO_Pin_1);  //µÍ8Î»	 
		GPIO_SetBits(GPIOE, GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10); //µÍ8Î»  
		GPIO_ResetBits(GPIOE, GPIO_Pin_0);
		GPIO_ResetBits(GPIOE, GPIO_Pin_1);		//RESET=0
		GPIO_SetBits(GPIOD, GPIO_Pin_4);		    //RD=1
		GPIO_SetBits(GPIOD, GPIO_Pin_5);			//WR=1
		
		
		
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;		//D1  D2
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);					 

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_13;		 //D3, D4
  GPIO_Init(GPIOD, &GPIO_InitStructure);
 
	 }

 

	/**
		 * @brief  Fill the global buffer
		* @param pBuffer: pointer on the Buffer to fill
		* @param BufferSize: size of the buffer to fill
		* @param Offset: first value to fill on the Buffer
		*/

	#ifdef  USE_FULL_ASSERT

	/**
		* @brief  Reports the name of the source file and the source line number
		*   where the assert_param error has occurred.
		* @param file: pointer to the source file name
		* @param line: assert_param error line source number
		* @retval : None
		*/
	void assert_failed(uint8_t* file, uint32_t line)
	{ 
		/* User can add his own implementation to report the file name and line number,
			 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

		/* Infinite loop */
		while (1)
		{
		}
	}
	#endif
	 