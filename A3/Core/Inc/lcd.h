/*
 * lcd.h
 *
 *  Created on: Apr 19, 2024
 *      Author: raheel
 */

#ifndef SRC_LCD_H_
#define SRC_LCD_H_

#include "stdio.h"
#include "stm32l4a6xx.h"


#define LCD_RW 0x01 //ReadWrite
#define LCD_EN 0x02 //ENable
#define LCD_RS 0x04 //RegisterSelect
#define LCD_BL 0x80 //BackLight
#define LCD_DATA_BITS 0x78


void Lcd_Init( void );
void Lcd_pulse_ENA( void );
void Lcd_4b_command( uint8_t command );
void Lcd_command( uint8_t command );
void Lcd_write_char( uint8_t letter );
uint8_t Lcd_swapNibbles( uint8_t dataByte );

void SysTick_Init(void);
void delay_us(const uint32_t time_us);





#define GPIO_PIN_0                 ((uint16_t)0x0001)  /* Pin 0 selected    */
#define GPIO_PIN_1                 ((uint16_t)0x0002)  /* Pin 1 selected    */
#define GPIO_PIN_2                 ((uint16_t)0x0004)  /* Pin 2 selected    */
#define GPIO_PIN_3                 ((uint16_t)0x0008)  /* Pin 3 selected    */
#define GPIO_PIN_4                 ((uint16_t)0x0010)  /* Pin 4 selected    */
#define GPIO_PIN_5                 ((uint16_t)0x0020)  /* Pin 5 selected    */
#define GPIO_PIN_6                 ((uint16_t)0x0040)  /* Pin 6 selected    */
#define GPIO_PIN_7                 ((uint16_t)0x0080)  /* Pin 7 selected    */
#define GPIO_PIN_8                 ((uint16_t)0x0100)  /* Pin 8 selected    */
#define GPIO_PIN_9                 ((uint16_t)0x0200)  /* Pin 9 selected    */
#define GPIO_PIN_10                ((uint16_t)0x0400)  /* Pin 10 selected   */
#define GPIO_PIN_11                ((uint16_t)0x0800)  /* Pin 11 selected   */
#define GPIO_PIN_12                ((uint16_t)0x1000)  /* Pin 12 selected   */
#define GPIO_PIN_13                ((uint16_t)0x2000)  /* Pin 13 selected   */
#define GPIO_PIN_14                ((uint16_t)0x4000)  /* Pin 14 selected   */
#define GPIO_PIN_15                ((uint16_t)0x8000)  /* Pin 15 selected   */
#define GPIO_PIN_All               ((uint16_t)0xFFFF)  /* All pins selected */

#endif /* SRC_LCD_H_ */
