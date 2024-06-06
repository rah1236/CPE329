/*******************************************************************************
 * EE 329 A3 LCD INTERFACE
 *******************************************************************************
 * @file           : lcd.c
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
 * LCD WIRING (pinout NUCLEO-L4A6ZG = L496ZG)
 *      peripheral – Nucleo I/O
 * VSS  1    3.3V    POWER RAIL
 * VDD  2    GND     POWER
 * RS   4    PD5     OUT
 * R/W  5    GND     POWER RAIL
 * E    6    PD4     OUT
 * DB4  11   PD0     OUT
 * DB5  12   PD1     OUT
 * DB6  13   PD2     OUT
 * DB7  14   PD3     OUT
 * LED+ 15   3.3V    POWER RAIL w/ series 20ohm res
 * LED- 16   GND     POWER RAIL
 *******************************************************************************
 * REVISION HISTORY
 * 0.1 240424 drh   completed all objectives, no known bugs, project complete
 *******************************************************************************
 * TODO
 * N/A
 *******************************************************************************
 * CITATIONS
 * OpenAI. (2024-Apr-24). ChatGPT,
 * query synopsis "c function to convert time variable in seconds to a digit
 * of a clock MM:SS", to  https://chat.openai.com/chat
 *******************************************************************************
 * 45678-1-2345678-2-2345678-3-2345678-4-2345678-5-2345678-6-2345678-7-234567 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "lcd.h"
#include "delay.h"
#include "string.h"
#include "stdint.h"

/* -----------------------------------------------------------------------------
 * function : LCD_init( )
 * INs      : None
 * OUTs     : None
 * action   : sets up all GPIO outputs for the lcd, sends setup commands
 * authors  : Daniel Hoefer (drh) - dhoefer@calpoly.edu
 * version  : 0.1
 * date     : 240424
 * -------------------------------------------------------------------------- */
void LCD_init( void )  {

    // Enable clock for GPIO port
    RCC->AHB2ENR   |=   (RCC_AHB2ENR_GPIODEN);   //enable clock

    // Configure GPIO pins for the lcd
    LCD_PORT->MODER &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE1 |
                         GPIO_MODER_MODE2 | GPIO_MODER_MODE3 |
                         GPIO_MODER_MODE4 | GPIO_MODER_MODE5);

    LCD_PORT->MODER |= (GPIO_MODER_MODE0_0 | GPIO_MODER_MODE1_0 |
                        GPIO_MODER_MODE2_0 | GPIO_MODER_MODE3_0 |
                        GPIO_MODER_MODE4_0 | GPIO_MODER_MODE5_0);

    LCD_PORT->PUPDR &= ~(GPIO_PUPDR_PUPD0 | GPIO_PUPDR_PUPD1 |
                         GPIO_PUPDR_PUPD2 | GPIO_PUPDR_PUPD3 |
                         GPIO_PUPDR_PUPD4 | GPIO_PUPDR_PUPD5);

    LCD_PORT->OSPEEDR |= ((3 << GPIO_OSPEEDR_OSPEED0_Pos) |
                          (3 << GPIO_OSPEEDR_OSPEED1_Pos) |
                          (3 << GPIO_OSPEEDR_OSPEED2_Pos) |
                          (3 << GPIO_OSPEEDR_OSPEED3_Pos) |
                          (3 << GPIO_OSPEEDR_OSPEED4_Pos) |
                          (3 << GPIO_OSPEEDR_OSPEED5_Pos));

    // Default all outputs to off
    LCD_PORT->BRR = (LCD_DATA_BITS | LCD_EN | LCD_RS);


    delay_us( 40000 );                     // power-up wait 40 ms
    for ( int idx = 0; idx < 3; idx++ ) {  // wake up 1,2,3: DATA = 0011 XXXX
        LCD_4b_command( 0x30 );            // HI 4b of 8b cmd, low nibble = X
        delay_us( 200 );
    }
    LCD_4b_command( 0x20 ); // fcn set #4: 4b cmd set 4b mode - next 0x28:2-line
    delay_us( 40 );
    LCD_command( 0x28 );    // Function set: 4-bit/2-line
    delay_us( 40 );
    LCD_command( 0x10 );    // Set cursor
    delay_us( 40 );
    LCD_command( 0x0C );    // display on, no cursor
    delay_us( 40 );
    LCD_command( 0x06 );    // entry mode set
    delay_us( 40 );
    LCD_command( 0x01 );    // clear
    delay_us( 40000 );
}

/* -----------------------------------------------------------------------------
 * function : LCD_pulse_ENA( )
 * INs      : None
 * OUTs     : None
 * action   : pulses enable pin to send data
 * authors  : provided project code
 * version  : 0.1
 * date     : 240424
 * -------------------------------------------------------------------------- */
void LCD_pulse_ENA( void )  {
    // ENAble line sends command on falling edge
    // set to restore default then clear to trigger
    LCD_PORT->ODR   |= ( LCD_EN );         // ENABLE = HI
    delay_us( 5 );                         // TDDR > 320 ns
    LCD_PORT->ODR   &= ~( LCD_EN );        // ENABLE = LOW
    delay_us( 5 );                         // low values flakey, see A3:p.1
}

/* -----------------------------------------------------------------------------
 * function : LCD_4b_command( )
 * INs      : command (uint8_t): command to send using nibble
 * OUTs     : None
 * action   : LCD command using high nibble only (wake-up' 0x30 commands)
 * authors  : provided project code
 * version  : 0.1
 * date     : 240424
 * -------------------------------------------------------------------------- */
void LCD_4b_command( uint8_t command )  {
    LCD_PORT->ODR   &= ~( LCD_DATA_BITS );      // clear DATA bits
    LCD_PORT->ODR   |= ( command >> 4 );        // DATA = command
    delay_us( 5 );
    LCD_pulse_ENA( );
}

/* -----------------------------------------------------------------------------
 * function : LCD_command( )
 * INs      : command (uint8_t): command to send
 * OUTs     : None
 * action   : send command to LCD in 4-bit instruction mode
 * authors  : provided project code
 * version  : 0.1
 * date     : 240424
 * -------------------------------------------------------------------------- */
void LCD_command( uint8_t command )  {
    // HIGH nibble then LOW nibble, timing sensitive
    LCD_PORT->ODR   &= ~( LCD_DATA_BITS );               // isolate cmd bits
    LCD_PORT->ODR   |= ( (command>>4) & LCD_DATA_BITS ); // HIGH shifted low
    delay_us( 5 );
    LCD_pulse_ENA( );                                    // latch HIGH NIBBLE

    LCD_PORT->ODR   &= ~( LCD_DATA_BITS );               // isolate cmd bits
    LCD_PORT->ODR   |= ( command & LCD_DATA_BITS );      // LOW nibble
    delay_us( 5 );
    LCD_pulse_ENA( );                                    // latch LOW NIBBLE
}

/* -----------------------------------------------------------------------------
 * function : LCD_write_char( )
 * INs      : letter (uint8_t): single character to send to display
 * OUTs     : None
 * action   : prints a single character on the display
 * authors  : provided project code
 * version  : 0.1
 * date     : 240424
 * -------------------------------------------------------------------------- */
void LCD_write_char( uint8_t letter )  {
    // calls LCD_command() w/char data; assumes all ctrl bits set LO in LCD_init()
    LCD_PORT->ODR   |= (LCD_RS);       // RS = HI for data to address
    delay_us( 5 );
    LCD_command( letter );             // character to print
    LCD_PORT->ODR   &= ~(LCD_RS);      // RS = LO
}

/* -----------------------------------------------------------------------------
 * function : LCD_write_text( )
 * INs      : *text (char): text "sting" to send to display
 * OUTs     : None
 * action   : prints a line of text to the display
 * authors  : provided project code
 * version  : 0.1
 * date     : 240424
 * -------------------------------------------------------------------------- */
void LCD_write_text(const char *text) {
    size_t  len = strlen(text);             // find length of char array

    for ( size_t  i = 0; i < len; i++ ) {
        LCD_write_char(text[i]);            //write each char to the LCD
        delay_us( 40 );
    }
}

/* -----------------------------------------------------------------------------
 * function : LCD_set_cursor( )
 * INs      : x, y (uint8_t): x and y cursor location to set on the LCD
 * OUTs     : None
 * action   : moves the cursor to a specific position on the LCD
 * authors  : Daniel Hoefer (drh) - dhoefer@calpoly.edu
 * version  : 0.1
 * date     : 240424
 * -------------------------------------------------------------------------- */
void LCD_set_cursor(uint8_t x, uint8_t y) {

    uint8_t address = 0x80;         //first line address offset
    if (y == 1) {                   //add the line offset for second line
        address += 0x40;            //address offset for the second line
    }

    address += x;           // find column offset using x pos

    LCD_command(address);   //send this command to lcd to update cursor
}

/* -----------------------------------------------------------------------------
 * function : LCD_get_clock_digit( )
 * INs      : seconds (int): timer time in seconds
 *            digit   (int): which digit position we want to find
 * OUTs     : int     (int): the returned calculated digit
 * action   : calculates (each) digit for the lcd based on the total time in sec
 * authors  : Daniel Hoefer (drh) - dhoefer@calpoly.edu
 * version  : 0.1
 * date     : 240424
 * -------------------------------------------------------------------------- */
int LCD_get_clock_digit(int seconds, int digit) {
    int minutes = seconds / 60;
    seconds %= 60;

    if ( digit == 0 )
        return minutes / 10;
    else if ( digit == 1 )
        return minutes % 10;
    else if ( digit == 2 )
        return seconds / 10;
    else if ( digit == 3 )
        return seconds % 10;
    else
        return -1;                  // Return an invalid value to indicate error
}
