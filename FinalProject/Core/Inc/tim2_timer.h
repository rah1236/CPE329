/*******************************************************************************
 * EE 329 TIM2 Interrupt Timer
 *******************************************************************************
 * @file        : tim2_timer.h
 * @brief       : Functionality for use of TIM2 timer with interrupts
 * project      : EE_329_A4
 * authors      : Ryan Marienthal (rem) - remarien@calpoly.edu,
 *                Jack Ryan (jar) - jryan39@calpoly.edu
 * version      : 0.1
 * date         : 240430
 * compiler     : STM32CubeIDE v.1.12.0 Build: 14980_20230301_1550 (UTC)
 * target       : NUCLEO-L4A6ZG
 * clocks       : 4 MHz MSI to AHB2
 * @attention   : (c) 2023 STMicroelectronics. All rights reserved.
 ******************************************************************************/
#include "stm32l4a6xx.h"
#include "PID.h"

#ifndef INC_TIM2_TIMER_H_
#define INC_TIM2_TIMER_H_

// Number of clock cycles for 5kHz wave
#define PERIOD      4000000/SAMPLE_RATE     // Constant ARR
#define CCR_INC     1000                    // 1 ms delay

// Function prototypes
void setup_TIM2(int iDutyCycle);
void TIM2_IRQHandler(void);
void reset_TIM2_timer(void);
int get_ms_passed(void);


#endif /* INC_TIM2_TIMER_H_ */
