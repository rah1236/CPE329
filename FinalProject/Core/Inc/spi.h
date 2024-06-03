#ifndef SRC_SPI_H_
#define SRC_SPI_H_
#include "stm32l4xx_hal.h"
typedef uint32_t dac_data_type;
#define SPI_PORT	GPIOA
#define PS			GPIO_PIN_4
#define SCK			GPIO_PIN_5
#define POCI		GPIO_PIN_6
#define PICO		GPIO_PIN_7
#define CS 			GPIO_PIN_1
void SPI_init( void );
dac_data_type DAC_volt_conv( dac_data_type voltage );
void SPI_write ( uint16_t data );
uint32_t SPI_read(void);
#endif /* SRC_SPI_H_ */