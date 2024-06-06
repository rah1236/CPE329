/*******************************************************************************
 * EE 329 A3 DELAY INTERFACE
 *******************************************************************************
 * @file           : delay.c
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
 * setup the timer using systick
 * create function that handles delays
 *******************************************************************************
 * REVISION HISTORY
 * 0.1 240424 drh   completed all objectives, no known bugs, project complete
 *******************************************************************************
 * TODO
 * N/A
 *******************************************************************************
 * 45678-1-2345678-2-2345678-3-2345678-4-2345678-5-2345678-6-2345678-7-234567 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private functions ---------------------------------------------------------*/

/* -----------------------------------------------------------------------------
 * function : SysTick_Init( )
 * INs      : None
 * OUTs     : None
 * action   : configure SysTick timer for use with delay_us().
 * authors  : provided project code
 * version  : 0.1
 * date     : 240424
 * -------------------------------------------------------------------------- */
void SysTick_Init(void) {
// warning: breaks HAL_delay() by disabling interrupts for shorter delay timing.
    SysTick->CTRL |= (SysTick_CTRL_ENABLE_Msk |         // enable SysTick Timer
                      SysTick_CTRL_CLKSOURCE_Msk);  // select CPU clock
    SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk);   // disable interrupt
}

/* -----------------------------------------------------------------------------
 * function : delay_us( )
 * INs      : time_us (uint32_t): time in microsecons to delay for
 * OUTs     : None
 * action   : delay in microsecs using SysTick timer to count CPU clock cycles
 *            do not call with 0 : error, maximum delay; careful calling with
 *            small nums : results in longer delays than specified:
 *            e.g. @4MHz, delay_us(1) = 10=15 us delay.
 * authors  : provided project code
 * version  : 0.1
 * date     : 240424
 * -------------------------------------------------------------------------- */
void delay_us(const uint32_t time_us) {
    // set the counts for the specified delay
    SysTick->LOAD = (uint32_t)((time_us * (SystemCoreClock / 1000000)) - 1);
    SysTick->VAL = 0;                                      // clear timer count
    SysTick->CTRL &= ~(SysTick_CTRL_COUNTFLAG_Msk);        // clear count flag
    while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)); // wait for flag
}

