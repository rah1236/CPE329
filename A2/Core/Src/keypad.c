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

uint8_t Keypad_CheckKeyPressed(uint8_t columnRowByte);



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
 * with the following structure: 0b 0 0   0 0 0   0 0 0
 *                                       |_____| |_____|
 *                                        Row     Column
 *  BEWARNED: Row data is a standard integer, it counts up to 4,
 *  Column data is BITWISE!!!, bit 1 represents column 1, bit 2 column 2, etc
 */
uint8_t Keypad_CheckKeyPressed(uint8_t columnRowByte){

	uint8_t var;

	switch (columnRowByte) {
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


int Keypad_FindMostFrequentElement(int arr[], int size) {
    int freq[DEBOUNCE_ELEMENTS] = {0}; // Array to store frequencies
    int maxFreq = 0;   // Variable to store maximum frequency
    int maxElement;    // Variable to store the most frequent element

    // Count the frequency of each element
    for (int i = 0; i < size; i++) {
        freq[arr[i]]++;
    }

    // Find the element with maximum frequency
    for (int i = 0; i < 5; i++) {
        if (freq[i] > maxFreq) {
            maxFreq = freq[i];
            maxElement = i;
        }
    }

    return maxElement;
}


uint8_t Keypad_Read(void){

	uint8_t columnRowByte;

	uint8_t debounceArray[DEBOUNCE_ELEMENTS];

	for(int debouncePoll = 0; debouncePoll < DEBOUNCE_ELEMENTS; debouncePoll++){

		// Poll through 3 pins
		for(int polledRow = 0; polledRow < 4; polledRow++){

			//Enable current polled row
			GPIOC->ODR = (1 << polledRow);

			// arbitrary delay to allow pin output to settle
			for (int i = 0; i < 100; i++);


			//printf("Currently polling %d, ", polledRow);
			//printf("Pin currently high = %d \n", (GPIOC->IDR >> 4));

			columnRowByte = (polledRow<<3) | ((GPIOC->IDR >> 4));

			debounceArray[debouncePoll] = Keypad_CheckKeyPressed(columnRowByte);

			printf("%c \n",Keypad_CheckKeyPressed(columnRowByte));
		}

		//return (Keypad_FindMostFrequentElement(debounceArray, DEBOUNCE_ELEMENTS));

	}
}

