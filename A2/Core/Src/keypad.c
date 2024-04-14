/*
 * keypad.c
 *
 *  Created on: Apr 12, 2024
 *      Author: raheel
 */

/*
 * This is a library for hard coding keypad functionality onto an STM32L4A MCU
 * This library assumes you are using
 * this keypad: https://www.mouser.com/ProductDetail/474-COM-14662
 *
 * Pins PC0, PC1, PC2, PC3 are used on the rows and set high one at a time for polling
 *
 * Pins PC4, PC5, PC6 are used on the columns, and are checked upon each
 * enabling of PC0, PC1, PC2, PC3
 *
 * If you are using a different keypad, or different pins MAKE SURE TO MODIFY
 * Keypad_Init, otherwise your shit will not work!
 *
 * The setup i'm going for looks something like this:
 *
 *        C1          C2          C3
 *
 * PC0     1           2           3        R1
 *
 *
 * PC1     4           5           6        R2
 *
 *
 * PC2     7           8           9        R3
 *
 *
 * PC3     *           0           #        R4
 *
 *
 *         PC4         PC5         PC6
 *
 *
 */

#include "keypad.h"


#define ROWS 4
#define COLUMNS 3


/* function prototypes */
void Keypad_Init(void);

void Keypad_Config(void);

void Keypad_Read(void);



// Use to initialize Keypad GPIO and other funcitonality of keypad
void Keypad_Init(void){
	// configure GPIO pins PC0, PC1, PC2, PC3 for:
	// output mode, push-pull, no pull up or pull down, high speed
	RCC->AHB2ENR   |=  (RCC_AHB2ENR_GPIOCEN);
	GPIOC->MODER   &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE1 | GPIO_MODER_MODE2 | GPIO_MODER_MODE3 );
	GPIOC->MODER   |=  (GPIO_MODER_MODE0_0 | GPIO_MODER_MODE1_0 | GPIO_MODER_MODE2_0 | GPIO_MODER_MODE3_0);
	GPIOC->OTYPER  &= ~(GPIO_OTYPER_OT0 | GPIO_OTYPER_OT1 | GPIO_OTYPER_OT2 | GPIO_OTYPER_OT3);
	GPIOC->PUPDR   &= ~(GPIO_PUPDR_PUPD0 | GPIO_PUPDR_PUPD1 | GPIO_PUPDR_PUPD2 | GPIO_PUPDR_PUPD3);
	GPIOC->OSPEEDR |=  ((3 << GPIO_OSPEEDR_OSPEED0_Pos) |
							  (3 << GPIO_OSPEEDR_OSPEED1_Pos) |
							  (3 << GPIO_OSPEEDR_OSPEED2_Pos) |
							  (3 << GPIO_OSPEEDR_OSPEED3_Pos));

	// configure GPIO pins PC4, PC5, PC6 for Input, with pull down
	GPIOC->MODER   &= ~(GPIO_MODER_MODE4 | GPIO_MODER_MODE5 | GPIO_MODER_MODE6 );
	GPIOC->PUPDR |= (GPIO_PUPDR_PUPD4_1 | GPIO_PUPDR_PUPD5_1 | GPIO_PUPDR_PUPD6_1);


	 // preset PC0, PC1, PC2, PC3 to 0
	GPIOC->BRR |= (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 );

}


void Keypad_Read(void){
	// Poll through 3 pins
	for(int poll = 0; poll < 4; poll++)
	{
		//Enable current polled row
		GPIOC->ODR = (1 << poll);

		// arbitrary delay to allow pin output to settle
		for (int i = 0; i < 100; i++);


		printf("Currently polling %d, ", poll);
		printf("Pin currently high = %d \n", (GPIOC->IDR >> 4));


	}
}

