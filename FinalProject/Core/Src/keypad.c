/*******************************************************************************
 * EE 329 A2 KEYPAD INTERFACE
 *******************************************************************************
 * @file           : keypad.c
 * @brief          : keypad utilities
 * project         : EE 329 S'24 Assignment 3
 * authors         : Daniel Hoefer (drh) dhoefer@calpoly.edu
 * version         : 0.1
 * date            : 240424
 * compiler        : STM32CubeIDE v.1.15.0 Build: 20695_20240315_1429 (UTC)
 * target          : NUCLEO-L496VZ
 * @attention      : (c) 2024 STMicroelectronics.  All rights reserved.
 *******************************************************************************
 * PLAN:
 * function for configuring GPIO
 * function for checking if keypad is pressed
 * function for determining which key is pressed
 * function (and supporting functions) for getting current time since boot
 *******************************************************************************
 * KEYPAD WIRING (pinout NUCLEO-L4A6ZG = L496ZG)
 *      peripheral – Nucleo I/O
 * col 1	PC4		OUT
 * col 2  	PC5		OUT
 * col 3	PC6		OUT
 * row 1	PC8		IN PULLDOWN
 * row 2	PC9		IN PULLDOWN
 * row 3	PC10	IN PULLDOWN
 * row 4	PC11	IN PULLDOWN
 *******************************************************************************
 * REVISION HISTORY
 * 0.1 240424 drh	completed all objectives, no known bugs, project complete
 *******************************************************************************
 * TODO
 * N/A
 *******************************************************************************
 * CITATIONS
 * OpenAI. (2024-Apr-16). ChatGPT,
 * query synopsis "generate code for initializing a GPIO input with pulldown on
 * Nucleo STM32 board, to  https://chat.openai.com/chat
 *******************************************************************************
 * 45678-1-2345678-2-2345678-3-2345678-4-2345678-5-2345678-6-2345678-7-234567 */

/* Includes ------------------------------------------------------------------*/
#include "keypad.h"
#include "main.h"
#include "delay.h"

/* Private Variables ---------------------------------------------------------*/
int 				KeyPressed = 1;
volatile uint32_t 	Millis_Count = 0;
uint32_t			Timer_KeyPressed = 0;

/* Private functions ---------------------------------------------------------*/

/* -----------------------------------------------------------------------------
 * function : Keypad_Config( )
 * INs      : None
 * OUTs     : None
 * action   : sets up all GPIO input and outputs for the keypad
 * authors  : Daniel Hoefer (drh) - dhoefer@calpoly.edu
 * version  : 0.1
 * date     : 240417
 * -------------------------------------------------------------------------- */
void Keypad_Config(void)
{

	RCC->AHB2ENR   |=  (RCC_AHB2ENR_GPIOCEN);	//enable clock

	GPIOC->MODER   &= 	~(GPIO_MODER_MODE4 |
					  	  GPIO_MODER_MODE5 |
						  GPIO_MODER_MODE6 );
	GPIOC->MODER   |=  	 (GPIO_MODER_MODE4_0 |
					  	  GPIO_MODER_MODE5_0 |
						  GPIO_MODER_MODE6_0 );
	GPIOC->PUPDR   &= 	~(GPIO_PUPDR_PUPD4 |
					  	  GPIO_PUPDR_PUPD5 |
						  GPIO_PUPDR_PUPD6 );
	GPIOC->OSPEEDR |=  ((3 << GPIO_OSPEEDR_OSPEED4_Pos) |
						(3 << GPIO_OSPEEDR_OSPEED5_Pos) |
						(3 << GPIO_OSPEEDR_OSPEED6_Pos) );
	GPIOC->BRR = (GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6);

	// Configure PC8-11 as inputs with pulldowns
	GPIOC->MODER   &= ~(GPIO_MODER_MODE8_Msk |
	                    GPIO_MODER_MODE9_Msk |
	                    GPIO_MODER_MODE10_Msk |
	                    GPIO_MODER_MODE11_Msk);
	GPIOC->PUPDR   &= ~(GPIO_PUPDR_PUPD8_Msk |
	                    GPIO_PUPDR_PUPD9_Msk |
	                    GPIO_PUPDR_PUPD10_Msk |
	                    GPIO_PUPDR_PUPD11_Msk);
	GPIOC->PUPDR   |=  (GPIO_PUPDR_PUPD8_1 |
	                    GPIO_PUPDR_PUPD9_1 |
	                    GPIO_PUPDR_PUPD10_1 |
	                    GPIO_PUPDR_PUPD11_1);
}

/* -----------------------------------------------------------------------------
 * function : Keypad_IsAnyKeyPressed( )
 * INs      : None
 * OUTs     : int, (0) for no press, (1) for button press
 * action   : checks if any one button on the keypad is pressed, incorporates
 * 			  a 100ms delay then confirmation for debounce
 * authors  : Daniel Hoefer (drh) - dhoefer@calpoly.edu
 * version  : 0.1
 * date     : 240417
 * -------------------------------------------------------------------------- */
int Keypad_IsAnyKeyPressed(void)
{
   COL_PORT->BSRR = COL_PINS;         	     	// set all columns HI
   if ((ROW_PORT->IDR & ROW_PINS) != 0 )		//check for keypress
   {
       delay_us( 100000 );
       if((ROW_PORT->IDR & ROW_PINS) != 0 )
           return( 1 );						//yes!
   }
   return( 0 );                         // nope.
}

/* -----------------------------------------------------------------------------
 * function : Keypad_IsAnyKeyPressed( )
 * INs      : None
 * OUTs     : int, (0) for no press, (1) for button press
 * action   : detect and encode a pressed key at {row,col}, assumes a previous
 * 			  call to Keypad_IsAnyKeyPressed() returned TRUE, also verifies the
 * 			  Keypad_IsAnyKeyPressed() result, also determines which key is
 * 			  pressed and returns the encoded key ID. Takes 200us maximum!
 * authors  : Daniel Hoefer (drh) - dhoefer@calpoly.edu
 * version  : 0.1
 * date     : 240417
 * -------------------------------------------------------------------------- */
int Keypad_WhichKeyIsPressed(void)
{
	int8_t iRow=0, iCol=0, iKey=0;  // keypad row & col index, key ID result
	int8_t bGotKey = 0;             // bool for keypress, 0 = no press

	COL_PORT->BSRR = COL_PINS;							// set all columns HI
	delay_us( SETTLE );
	for ( iRow = 0; iRow < NUM_ROWS; iRow++ )			// check all ROWS
	{
		if ( ROW_PORT->IDR & (BIT0 << (8+iRow)) ) {		// keypress in iRow!
			COL_PORT->BRR = COL_PINS;            	 	// set all columns LO
			delay_us( SETTLE );
			for ( iCol = 0; iCol < NUM_COLS; iCol++ )		//1 column at a time
			{
				COL_PORT->BSRR = ( BIT0 << (4+iCol) );     	//set this column HI
				delay_us( SETTLE );
				if ( ROW_PORT->IDR & (BIT0 << (8+iRow)) )	// keypress in iCol!
				{
				   bGotKey = 1;								// yay, can now exit
				   break;                                  	// for iCol loop!
				}
			}
			if ( bGotKey )
				break;							//and we can exit for iRow loop!
		}
	}
   // encode {iRow,iCol} into LED word : row 1-3 : numeric, ‘1’-’9’
   //	                                 row 4   : ‘*’=10, ‘0’=0, ‘#’=15
   //                                    no press: send NO_KEYPRESS
	if ( bGotKey )
	{
		iKey = ( iRow * NUM_COLS ) + iCol + 1;  // handle numeric keys ...

		if ( iKey == KEY_ZERO )                 // handle '0' case
			iKey = CODE_ZERO;
		if ( iKey == KEY_STAR )                 // handles '*' case
 	         iKey = CODE_STAR;
		if ( iKey == KEY_HASHTAG )				// handles '#' case
 	         iKey = CODE_HASHTAG;

		return( iKey );                         // return encoded keypress
   }
   return( NO_KEYPRESS );                     	// unable to verify keypress
}
