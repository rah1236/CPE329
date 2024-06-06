/*******************************************************************************
 * EE 329 UTILITIES
 *******************************************************************************
 * @file        : utils.c
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
#include "utils.h"

/* **** FUNCTION HEADER TEMPLATE **** */

/*------------------------------------------------------------------------------
 * Function :
 * IN       :
 * OUT      :
 * Action   :
 * Author   : Jack Ryan (jar) - jryan39@calpoly.edu
 * Version  : 0.1
 * Date    `:
------------------------------------------------------------------------------*/

/*******************************************************************************
 * EE 329 XXXX
 *******************************************************************************
 * @file        :
 * @brief       :
 * project      :
 * authors      : Jack Ryan (jar) - jryan39@calpoly.edu
 * version      : 0.1
 * date         :
 * compiler     : STM32CubeIDE v.1.12.0 Build: 14980_20230301_1550 (UTC)
 * target       : NUCLEO-L4A6ZG
 * clocks       : 4 MHz MSI to AHB2
 * @attention   : (c) 2023 STMicroelectronics. All rights reserved.
 ******************************************************************************/


/*------------------------------------------------------------------------------
 * Function: initialize_pin()
 * IN   : pin_reg - pointer to GPIO struct (i.e. GPIOC, GPIOB)
 *        pin_num - GPIO pin number
 *        mode - pin mode (input, output, etc. as described on page 304 of
 *        STM32 datasheet)
 *        output_type - (0=push/pull, 1 = open drain)
 *        pupdr - pull-up/pull-down/neither (as described on page 306 of
 *        datasheet)
 *        speed - pin speed (as describe on page 305 of datasheet)
 * OUT  : None
 * Action   : Initializes GPIO pin with given parameters
 * Author   : Jack Ryan (jar) - jryan39@calpoly.edu
 * Version  : 0.1
 * Date : 240417
------------------------------------------------------------------------------*/
void initialize_pin(GPIO_TypeDef *pin_reg, uint32_t pin_num, uint32_t mode,
		uint32_t output_type, uint32_t pupdr, uint32_t speed){
    // Enable clock for chosen GPIO register
    if (pin_reg == GPIOA){
        RCC->AHB2ENR   |=  (RCC_AHB2ENR_GPIOAEN);
    }else if (pin_reg == GPIOB){
        RCC->AHB2ENR   |=  (RCC_AHB2ENR_GPIOBEN);
    }else if (pin_reg == GPIOC){
        RCC->AHB2ENR   |=  (RCC_AHB2ENR_GPIOCEN);
    }else if (pin_reg == GPIOD){
        RCC->AHB2ENR   |=  (RCC_AHB2ENR_GPIODEN);
    }else if (pin_reg == GPIOE){
        RCC->AHB2ENR   |=  (RCC_AHB2ENR_GPIOEEN);
    }else if (pin_reg == GPIOF){
        RCC->AHB2ENR   |=  (RCC_AHB2ENR_GPIOFEN);
    }else if (pin_reg == GPIOG){
        RCC->AHB2ENR   |=  (RCC_AHB2ENR_GPIOGEN);
    }else if (pin_reg == GPIOH){
        RCC->AHB2ENR   |=  (RCC_AHB2ENR_GPIOHEN);
    }else if (pin_reg == GPIOI){
        RCC->AHB2ENR   |=  (RCC_AHB2ENR_GPIOIEN);
    }
	// If pin is configured as ouput
	if (mode == 1){
		// Set push/pull config
		pin_reg->OTYPER &= ~(0x1 << pin_num);           // Clear output mode bits
		pin_reg->OTYPER |= (output_type << pin_num);    // Set output type
	}
	// Set pullup/pulldown/neither
	pin_reg->PUPDR &= ~(0x03 << 2*pin_num);         // Clear pupdr bits
	pin_reg->PUPDR |= (pupdr << 2*pin_num);         // Set pupdr bits
	// Set GPIO speed
	pin_reg->OSPEEDR &= ~(0x03 << 2*pin_num);   // Clear bits
	pin_reg->OSPEEDR |= (speed << 2*pin_num);   // Set to chosen speed
	// Initial state
	pin_reg->BRR |= (0x01 << pin_num);          // Initialize as off
    // Set mode
    pin_reg->MODER &= ~(0x03 << 2*pin_num);     // Clear mode bits
    pin_reg->MODER |= (mode << 2*pin_num);      // Set output mode
}

