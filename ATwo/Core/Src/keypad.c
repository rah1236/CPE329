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
#define DEBOUNCE_ELEMENTS 5

/* function prototypes */
void Keypad_Init(void);

void Keypad_Config(void);

uint8_t Keypad_Read(void);

uint8_t Keypad_CheckKeyPressed(uint8_t column_row_byte);



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




/*
 * The function Keypad_CheckKeyPressed, accepts an single byte
 * with the following encoding: 0 b 0 0   0 0 0   0 0 0
 *                                  |_|  |_____| |_____|
 *                                  N/A   Row     Column
 *  BEWARNED: Row data is a standard integer, it counts up to 4,
 *  Column data is BITWISE!!!, bit 0 represents column 1, bit 1 column 2, etc
 *  This function returns chars relating to which key is pressed
 */
uint8_t Keypad_CheckKeyPressed(uint8_t column_row_byte){

	uint8_t var;

	switch (column_row_byte) {
	  case 0x01: //Column 1, Row 1
		  var = ('1');
		  break;
	  case 0x02: //Column 2, Row 1
		  var = ('2');
		  break;

	  case 0x04: //Column 3, Row 1
		  var = ('3');
		  break;

	  case 0x09: //Column 1, Row 2
		  var = ('4');
		  break;

	  case 0x0a: //Column 2, Row 2
		  var = ('5');
		  break;

	  case 0x0c: //Column 3, Row 2
		  var = ('6');
		  break;

	  case 0x11: //Column 1, Row 3
		  var = ('7');
		  break;

	  case 0x12: //Column 2, Row 3
		  var = ('8');
		  break;

	  case 0x14: //Column 2, Row 3
		  var = ('9');
		  break;

	  case 0x19: //Column 1, Row 4
		  var = ('*');
		  break;

	  case 0x1a: //Column 2, Row 4
		  var = ('0');
		  break;

	  case 0x1c: //Column 3, Row 4
		  var = ('#');
		  break;

	  default:
		  var = ('.'); //period for no press
		  break;
	}

	return var;
}




/* The function Keypad_Read polls through the keypad row pins a total of
 *  DEBOUNCE_ELEMENTS * 3 times, and returns the most recently
 *  consistently pressed key.
 *
 */
uint8_t Keypad_Read(void){


	uint8_t encoded_key_pressed;

	int32_t number_of_key_changes;
	uint8_t last_key_pressed;
	uint8_t current_key_pressed;


	//Run the polling code DEBOUNCE_ELEMENTS number of times
	for(int debounce_round = 0; debounce_round < DEBOUNCE_ELEMENTS; debounce_round++){
		// Poll through 4 rows
		for(int polled_row = 0; polled_row < ROWS; polled_row++){

			//Enable current polled row
			GPIOC->ODR = (1 << polled_row);

			//Get currently pressed key in encoded form
			encoded_key_pressed = (polled_row<<3) | ((GPIOC->IDR >> 4));

			// if the a key was pressed
			if (Keypad_CheckKeyPressed(encoded_key_pressed) != '.'){
				// record WHICH key was pressed
				current_key_pressed = Keypad_CheckKeyPressed(encoded_key_pressed);
			}

			//if the last key pressed isn't the same as the current key pressed
			if (last_key_pressed != current_key_pressed){
				//Record the change
				number_of_key_changes++;
			}
			//if the last key pressed is the same as the current key pressed,
			else {
				//Reset the change counter
				number_of_key_changes = 0;
			}

			//Set the current key pressed to be the last key pressed before polling again
			last_key_pressed = current_key_pressed;
		}
	}
		//If the last key pressed hasn't changed in DEBOUNCE_ELEMENTS number of polling cycles
		//return the last key pressed, otherwise, return the null character
		return((number_of_key_changes == 0) ? last_key_pressed : '\0');


}

