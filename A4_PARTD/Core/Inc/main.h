/*
 * MAIN.H FOR CPE329 LAB A4, PARTS D, REACTION TIMER
 *
 * RAHEEL REHMATULLAH AND LOGAN TOM
 */

#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32l4xx_hal.h"
void Error_Handler(void);
void setup_rng(void);
void setup_gpio(void);
uint32_t get_random_number(void);
int get_user_button_state(void);
void setup_ext_interrupt(void);
void setup_TIM2( int iDutyCycle );

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
