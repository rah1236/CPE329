/*
 * MAIN.H FOR CPE329 LAB A4, Parts A-C, CONFIGURED FOR PART B

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
void setup_TIM2( int iDutyCycle );
void setup_MCO_CLK(void) ;
void setup_gpio(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
