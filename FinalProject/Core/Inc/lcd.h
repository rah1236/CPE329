/*******************************************************************************
 * EE 329 A3 LCD INTERFACE
 *******************************************************************************
 * @file           : lcd.h
 * @brief          : header for lcd utility functions
 * project         : EE 329 S'24 Assignment 3
 * authors         : Daniel Hoefer (drh) dhoefer@calpoly.edu
 * version         : 0.1
 * date            : 240424
 * compiler        : STM32CubeIDE v.1.15.0 Build: 20695_20240315_1429 (UTC)
 * target          : NUCLEO-L496VZ
 * @attention      : (c) 2024 STMicroelectronics.  All rights reserved.
 *******************************************************************************
 * REVISION HISTORY
 * 0.1 240424 drh   completed all objectives, no known bugs, project complete
 *******************************************************************************
 * TODO
 * N/A
 *******************************************************************************
 * 45678-1-2345678-2-2345678-3-2345678-4-2345678-5-2345678-6-2345678-7-234567 */

#ifndef INC_LCD_H_
#define INC_LCD_H_

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"     //required for uint32_t type
#include "string.h"     //required for text

/* Private function prototypes ---------------------------------------------*/
void LCD_init( void );
void LCD_pulse_ENA( void );
void LCD_4b_command( uint8_t command );
void LCD_command( uint8_t command );
void LCD_write_char( uint8_t letter );
void LCD_write_text(const char *text);
void LCD_set_cursor(uint8_t x, uint8_t y);
int LCD_get_clock_digit(int seconds, int digit);

/* Private defines -----------------------------------------------------------*/
#define LCD_PORT        GPIOD
#define LCD_DATA_BITS   ((uint16_t)0x000F)  //0000 0000 0000 1111
#define LCD_EN          ((uint16_t)0x0010)  //0000 0000 0001 0000
#define LCD_RS          ((uint16_t)0x0020)  //0000 0000 0010 0000

#define CALIBRATE       487                 //adjust time ot be accurate

#endif /* INC_LCD_H_ */
