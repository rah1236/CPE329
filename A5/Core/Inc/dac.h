#ifndef SRC_DAC_H_
#define SRC_DAC_H_
#include "stm32l4xx_hal.h"
typedef uint32_t dac_data_type;
#define DAC_PORT	GPIOA
#define PS			GPIO_PIN_4
#define SCK			GPIO_PIN_5
#define POCI		GPIO_PIN_6
#define PICO		GPIO_PIN_7
#define CS 			GPIO_PIN_1
void SPI_init( void );
void DAC_Init( void );
dac_data_type DAC_volt_conv( dac_data_type voltage );
void DAC_write ( uint16_t data );
#endif /* SRC_DAC_H_ */
