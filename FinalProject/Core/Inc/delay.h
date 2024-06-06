/*******************************************************************************
 * EE 329 A3 DELAY INTERFACE
 *******************************************************************************
 * @file           : delay.h
 * @brief          : header for delay utility functions
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

#ifndef INC_DELAY_H_
#define INC_DELAY_H_

/* Private function prototypes -----------------------------------------------*/
void SysTick_Init(void);
void delay_us(const uint32_t time_us);

#endif /* INC_DELAY_H_ */
