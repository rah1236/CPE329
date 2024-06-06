/*******************************************************************************
 * EE 329 UTILITIES
 *******************************************************************************
 * @file        : utils.h
 * @brief       : general, repetitive functions used across projects
 * project      : EE 329
 * authors      : Jack Ryan (jar) - jryan39@calpoly.edu
 * version      : 0.1
 * date         : 240424
 * compiler     : STM32CubeIDE v.1.12.0 Build: 14980_20230301_1550 (UTC)
 * target       : NUCLEO-L4A6ZG
 * clocks       : 4 MHz MSI to AHB2
 * @attention   : (c) 2023 STMicroelectronics. All rights reserved.
 ******************************************************************************/

#include "stm32l4xx_hal.h"


#ifndef INC_UTILS_H_
#define INC_UTILS_H_

// GPIO modes
#define INPUT_MODE      (uint32_t)0
#define OUTPUT_MODE     (uint32_t)1
#define ALT_FUNCT_MODE  (uint32_t)2
#define ANALOG_MODE     (uint32_t)3

// OTYPER types
#define OUT_PUSH_PULL   (uint32_t)0
#define OUT_OPEN_DRAIN  (uint32_t)1

// OSPEEDR speeds
#define LOW_SPEED       (uint32_t)0
#define MEDIUM_SPEED    (uint32_t)1
#define HIGH_SPEED      (uint32_t)2
#define VERY_HIGH_SPEED (uint32_t)3

// PUPDR pull-up/pull-down settings
#define NO_PULLUP_PULLDOWN  (uint32_t)0
#define PULLUP              (uint32_t)1
#define PULLDOWN            (uint32_t)2
#define PUPD_RESERVED       (uint32_t)3


// initialize_pin prototype
void initialize_pin(GPIO_TypeDef *pin_reg, uint32_t pin_num, uint32_t mode,
        uint32_t output_type, uint32_t pupdr, uint32_t speed);

#endif /* INC_UTILS_H_ */
