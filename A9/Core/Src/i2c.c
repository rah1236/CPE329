/*
 * i2c.c
 *
 *  Created on: May 19, 2024
 *      Author: raheel
 */
#include "i2c.h"

#define EEPROM_ADDRESS (0xA0)  // Replace 0x50 with your EEPROM's address

/*
 * PB8 -> i2c SCL
 * PB9 -> i2c SDA
 */

void I2C_init(void){
	/* USER configure GPIO pins for I2C alternate functions SCL and SDA */
	// Configure I2C
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOBEN);   // enable GPIOB clock
	GPIOB->MODER &= ~(GPIO_MODER_MODE8 | GPIO_MODER_MODE9); //clear bits
	GPIOB->MODER |= (GPIO_MODER_MODE8_1 | GPIO_MODER_MODE9_1); //set bits AF mode
	GPIOB->AFR[1] &= ~((0x000F << GPIO_AFRH_AFSEL8_Pos)
			| (0x000F << GPIO_AFRH_AFSEL9_Pos));
	GPIOB->AFR[1] |= ((0x0004 << GPIO_AFRH_AFSEL8_Pos)
			| (0x0004 << GPIO_AFRH_AFSEL9_Pos));	//set bits AF 4
	GPIOB->PUPDR &= (GPIO_PUPDR_PUPD8 | GPIO_PUPDR_PUPD9);
	GPIOB->PUPDR |= (GPIO_PUPDR_PUPD8_1 | GPIO_PUPDR_PUPD9_1);
//	GPIOB->OTYPER |= (GPIO_PIN_8 | GPIO_PIN_9);
//	GPIOB->OSPEEDR &=~(GPIO_OSPEEDR_OSPEED8 | GPIO_OSPEEDR_OSPEED9);
//	GPIOB->OSPEEDR |= (GPIO_OSPEEDR_OSPEED8_1 | GPIO_OSPEEDR_OSPEED9_1);

	RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;  // enable I2C bus clock
	I2C1->CR1   &= ~( I2C_CR1_PE );        // put I2C into reset (release SDA, SCL)
	I2C1->CR1   &= ~( I2C_CR1_ANFOFF );    // filters: enable analog
	I2C1->CR1   &= ~( I2C_CR1_DNF );       // filters: disable digital
	I2C1->TIMINGR = 0x00000004;
	I2C1->CR2   |=  ( I2C_CR2_AUTOEND );   // auto send STOP after transmission
	I2C1->CR2   &= ~( I2C_CR2_ADD10 );     // 7-bit address mode
	I2C1->CR1   |=  ( I2C_CR1_PE );        // enable I2C
	// build EEPROM transaction
	I2C1->CR2   &= ~( I2C_CR2_RD_WRN );    // set WRITE mode
	I2C1->CR2   &= ~( I2C_CR2_NBYTES );    // clear Byte count
	I2C1->CR2   |=  ( 3 << I2C_CR2_NBYTES_Pos); // write 3 bytes (2 Addr, 1 data)
	I2C1->CR2   &= ~( I2C_CR2_SADD );      // clear device address
	I2C1->CR2   |=  ( EEPROM_ADDRESS << (I2C_CR2_SADD_Pos) ); // device addr SHL 1
	I2C1->CR2   |=    I2C_CR2_START;       // start I2C WRITE op
	/* USER wait for I2C_ISR_TXIS to clear before writing each Byte, e.g. ... */
//	while(!(I2C1->ISR & I2C_ISR_TXIS)) ;   // wait for start condition to transmit
//	I2C1->TXDR = (EEPROM_ADDRESS >> 8); // xmit MSByte of address
	/* address high, address low, data  -  wait at least 5 ms before READ
	   the READ op has new NBYTES (WRITE 2 then READ 1) & new RD_WRN for 3rd Byte */
}

void I2C_write(uint8_t device_address, uint8_t tx_data, uint16_t tx_address){

	/* USER wait for I2C_ISR_TXIS to clear before writing each Byte, e.g. ... */
	while (!(I2C1->ISR & I2C_ISR_TXIS));
	I2C1->TXDR = (tx_address >> 8) & 0xFF; // Address high byte

	while (!(I2C1->ISR & I2C_ISR_TXIS));
	I2C1->TXDR = tx_address & 0x00FF; // Address low byte

	// Send the data to write
	while (!(I2C1->ISR & I2C_ISR_TXIS));
	I2C1->TXDR = tx_data;

	// Wait for the STOP condition
	while (!(I2C1->ISR & I2C_ISR_STOPF));

	// Reset and re-enable I2C bus
	I2C1->CR1 &= ~( I2C_CR1_PE);
	I2C1->CR1 |= ( I2C_CR1_PE);
	/* address high, address low, data  -  wait at least 5 ms before READ
	   the READ op has new NBYTES (WRITE 2 then READ 1) & new RD_WRN for 3rd Byte */
}

uint8_t I2C_read(uint8_t device_address, uint16_t tx_address){
	// build EEPROM transaction
	I2C1->CR2   |=  ( I2C_CR2_RD_WRN );    // set READ mode
	I2C1->CR2   &= ~( I2C_CR2_NBYTES );    // clear Byte count
	I2C1->CR2   |=  ( 3 << I2C_CR2_NBYTES_Pos); // write 2 bytes (2 addr) recieve 1 (1 data)
	I2C1->CR2   &= ~( I2C_CR2_SADD );      // clear device address
	I2C1->CR2   |=  ( device_address << (I2C_CR2_SADD_Pos+1) ); // device addr SHL 1
	I2C1->CR2   |=    I2C_CR2_START;       // start I2C WRITE op
	/* USER wait for I2C_ISR_TXIS to clear before writing each Byte, e.g. ... */
	while(!(I2C1->ISR & I2C_ISR_TXIS)) ;   // wait for start condition to transmit
	return (I2C1->RXDR); //Return data captured
	/* address high, address low, data  -  wait at least 5 ms before READ
	   the READ op has new NBYTES (WRITE 2 then READ 1) & new RD_WRN for 3rd Byte */
}