/*
 * delay.h
 *
 *  Created on: May 28, 2024
 *      Author: raheel
 */

#ifndef INC_DELAY_H_
#define INC_DELAY_H_

#include "stm32l4xx_hal.h"

void SysTick_Init(void);
void delay_us(const uint32_t time_us);

#endif /* INC_DELAY_H_ */
