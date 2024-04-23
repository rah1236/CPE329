/*
 * lcd.c
 *
 *  Created on: Apr 19, 2024
 *      Author: raheel
 */

/*
 * Pin Mapping:
 *
 *
 *
 * PD0 - R/W
 * PD1 - E
 * PD2 - RS
 *
 * PD3 - D4
 * PD4 - D5
 * PD5 - D6
 * PD6 - D7
 *
 * PD7 - Backlight LED
 */

#include "lcd.h"
#include "string.h"


#define LCD_PORT GPIOD
#define DELAY 50


#define LCD_RW 0x01 //ReadWrite
#define LCD_EN 0x02 //ENable
#define LCD_RS 0x04 //RegisterSelect
#define LCD_BL 0x80 //BackLight
#define LCD_DATA_BITS 0x78


void SysTick_Init(void) {
	SysTick->CTRL |= (SysTick_CTRL_ENABLE_Msk |     	// enable SysTick Timer
                      SysTick_CTRL_CLKSOURCE_Msk); 	// select CPU clock
	SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk);  	// disable interrupt
}

// delay in microseconds using SysTick timer to count CPU clock cycles
// do not call with 0 : error, maximum delay.
// careful calling with small nums : results in longer delays than specified:
//	   e.g. @4MHz, delay_us(1) = 10=15 us delay.
void delay_us(const uint32_t time_us) {
	// set the counts for the specified delay
	SysTick->LOAD = (uint32_t)((time_us * (SystemCoreClock / 1000000)) - 1);
	SysTick->VAL = 0;                                  	 // clear timer count
	SysTick->CTRL &= ~(SysTick_CTRL_COUNTFLAG_Msk);    	 // clear count flag
	while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)); // wait for flag
}


void Lcd_pulse_ENA( void )  {
// ENAble line sends command on falling edge
// set to restore default then clear to trigger
   LCD_PORT->ODR   |= ( LCD_EN );         	// ENABLE = HI
   delay_us( DELAY );                         // TDDR > 320 ns
   LCD_PORT->ODR   &= ~( LCD_EN );        // ENABLE = LOW
   delay_us( DELAY );                         // low values flakey, see A3:p.1
}

void Lcd_4b_command( uint8_t command )  {
// LCD command using high nibble only - used for 'wake-up' 0x30 commands
   LCD_PORT->ODR   &= ~( LCD_DATA_BITS ); 	// clear DATA bits
   LCD_PORT->ODR   |= ( command >> 4 ) << 3;   // DATA = command
   delay_us( DELAY );
   Lcd_pulse_ENA( );
}

void Lcd_command( uint8_t command )  {
// send command to LCD in 4-bit instruction mode
// HIGH nibble then LOW nibble, timing sensitive
   LCD_PORT->ODR   &= ~( LCD_DATA_BITS );               // isolate cmd bits
   LCD_PORT->ODR   |= ( (command>>4)<<3 & LCD_DATA_BITS ); // HIGH shifted low
   delay_us( DELAY );
   Lcd_pulse_ENA( );                                    // latch HIGH NIBBLE

   LCD_PORT->ODR   &= ~( LCD_DATA_BITS );               // isolate cmd bits
   LCD_PORT->ODR   |= ( command<<3 & LCD_DATA_BITS );      // LOW nibble
   delay_us( DELAY );
   Lcd_pulse_ENA( );                                    // latch LOW NIBBLE
}

void Lcd_write_char( uint8_t letter )  {
// calls LCD_command() w/char data; assumes all ctrl bits set LO in LCD_init()
   LCD_PORT->ODR   |= (LCD_RS);       // RS = HI for data to address
   delay_us( DELAY );
   Lcd_command( letter );             // character to print
   LCD_PORT->ODR   &= ~(LCD_RS);      // RS = LO
}


void Lcd_write_string( char *text , uint8_t row){
	if (row == 0){
		Lcd_command(0x80); //Row 1
	}
	else {
		Lcd_command(0xC0); //Row 2
	}
	for (int idx = 0; idx < 17; idx++){
		if (text[idx] == '\0'){
			Lcd_write_char(' ');
		}
		else{
			Lcd_write_char(text[idx]);
		}
	}
}

void Lcd_Init(void){
	// configure GPIO pins PD0, PD1, PD2, PD3, PD4, PD5, PD6, PD7 for:
	// output mode, push-pull, no pull up or pull down, high speed
	RCC->AHB2ENR   |=  (RCC_AHB2ENR_GPIODEN);
	GPIOD->MODER   &= ~(GPIO_MODER_MODE0
			            | GPIO_MODER_MODE1
							| GPIO_MODER_MODE2
							| GPIO_MODER_MODE3
							| GPIO_MODER_MODE4
							| GPIO_MODER_MODE5
							| GPIO_MODER_MODE6
							| GPIO_MODER_MODE7);

	GPIOD->MODER   |=  (GPIO_MODER_MODE0_0
						   | GPIO_MODER_MODE1_0
							| GPIO_MODER_MODE2_0
							| GPIO_MODER_MODE3_0
							| GPIO_MODER_MODE4_0
							| GPIO_MODER_MODE5_0
							| GPIO_MODER_MODE6_0
							| GPIO_MODER_MODE7_0);

	GPIOD->OTYPER  &= ~(GPIO_OTYPER_OT0
			            | GPIO_OTYPER_OT1
							| GPIO_OTYPER_OT2
							| GPIO_OTYPER_OT3
							| GPIO_OTYPER_OT4
							| GPIO_OTYPER_OT5
							| GPIO_OTYPER_OT6
							| GPIO_OTYPER_OT7);

	GPIOD->PUPDR   &= ~(GPIO_PUPDR_PUPD0
			            | GPIO_PUPDR_PUPD1
							| GPIO_PUPDR_PUPD2
							| GPIO_PUPDR_PUPD3
							| GPIO_PUPDR_PUPD4
							| GPIO_PUPDR_PUPD5
							| GPIO_PUPDR_PUPD6
							| GPIO_PUPDR_PUPD7);

	GPIOD->OSPEEDR |=  ((3 << GPIO_OSPEEDR_OSPEED0_Pos) |
							  (3 << GPIO_OSPEEDR_OSPEED1_Pos) |
							  (3 << GPIO_OSPEEDR_OSPEED2_Pos) |
							  (3 << GPIO_OSPEEDR_OSPEED3_Pos) |
							  (3 << GPIO_OSPEEDR_OSPEED4_Pos) |
							  (3 << GPIO_OSPEEDR_OSPEED5_Pos) |
							  (3 << GPIO_OSPEEDR_OSPEED6_Pos) |
							  (3 << GPIO_OSPEEDR_OSPEED7_Pos) );

	 // preset PC0 thru PC7 to 0
	GPIOD->BRR |= (GPIO_PIN_0
			       | GPIO_PIN_1
					 | GPIO_PIN_2
					 | GPIO_PIN_3
					 | GPIO_PIN_4
					 | GPIO_PIN_5
					 | GPIO_PIN_6
					 | GPIO_PIN_7);

	//Turn on Backlight
	GPIOD->BSRR |= GPIO_PIN_7;



   delay_us( 40000 );                     // power-up wait 40 ms
   for ( int idx = 0; idx < 3; idx++ ) {  // wake up 1,2,3: DATA = 0011 XXXX
      Lcd_4b_command( 0x30 );             // HI 4b of 8b cmd, low nibble = X
      delay_us( 200 );
   }
   Lcd_4b_command( 0x20 ); // fcn set #4: 4b cmd set 4b mode - next 0x28:2-line
   delay_us( 40 );         // remainder of LCD init removed - see LCD datasheets


   Lcd_command( 0x28 );
   delay_us( 40 );
   Lcd_command( 0x10 ); //Set cursor
   delay_us( 40 );
   Lcd_command( 0x0D ); //Display ON; Blinking cursor
   delay_us( 40 );
   Lcd_command( 0x06 ); //Entry mode set
   delay_us( 40 );
   delay_us( 40 );


}






