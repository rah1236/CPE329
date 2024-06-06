/*******************************************************************************
 * EE 329 A2 KEYPAD INTERFACE
 *******************************************************************************
 * @file           : keypad.h
 * @brief          : header for keypad utility functions
 * project         : EE 329 S'24 Assignment 1
 * authors         : Daniel Hoefer (drh) dhoefer@calpoly.edu
 * version         : 0.1
 * date            : 240417
 * compiler        : STM32CubeIDE v.1.15.0 Build: 20695_20240315_1429 (UTC)
 * target          : NUCLEO-L496VZ
 * @attention      : (c) 2024 STMicroelectronics.  All rights reserved.
 *******************************************************************************
 * REVISION HISTORY
 * 0.1 240417 drh	completed all objectives, no known bugs, project complete
 *******************************************************************************
 * TODO
 * N/A
 *******************************************************************************
 * 45678-1-2345678-2-2345678-3-2345678-4-2345678-5-2345678-6-2345678-7-234567 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SRC_KEYPAD_H_
#define SRC_KEYPAD_H_

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"		//required for uint32_t type

/* Private function prototypes -----------------------------------------------*/
void        Keypad_Config(void);
int         Keypad_IsAnyKeyPressed(void);
int         Keypad_WhichKeyIsPressed(void);

/* Private defines -----------------------------------------------------------*/
#define SETTLE			10
#define BIT0 			((uint16_t)0x0001)

#define COL_PINS 		((uint16_t)0x0070)
#define ROW_PINS		((uint16_t)0x0F00)
#define COL_PORT		GPIOC
#define ROW_PORT		GPIOC
#define NUM_ROWS		4
#define NUM_COLS		3

#define KEY_STAR		10
#define KEY_ZERO		11
#define KEY_HASHTAG		12
#define CODE_STAR		10
#define CODE_ZERO		0
#define CODE_HASHTAG	15
#define DISPLAY_STAR	5

#define NO_KEYPRESS		-1


#endif /* SRC_KEYPAD_H_ */
