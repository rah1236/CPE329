/* USER CODE BEGIN Header */
/*******************************************************************************
 * EE 329 A5: SPI based DAC
 *******************************************************************************
 * @file           : main.c
 * @brief          : This is a programmable voltage output leveraging the SPI
 * 					bus on the STM32 and the MCP4281
 * project         : EE 329 S'24 Assignment 5
 * authors         : Raheel Rehmatullah and Colin Lucio
 * version         : 1
 * date            : 5_17_2024
 * compiler        : STM32CubeIDE v.1.15.0
 * target          : NUCLEO-L4A6ZG
 * clocks          : 4 MHz MSI to AHB2
 * @attention      : (c) 2023 STMicroelectronics.  All rights reserved.
 *******************************************************************************
/* USER CODE END Header */
#include "main.h"
#include "dac.h"
#include "keypad.h"

void SystemClock_Config(void);
void setup_user_LEDs_PBSW(void);
int ginumRecv = 0;
int giNumFlag = 0;

int main(void)
{
 HAL_Init();
 SystemClock_Config();
 Keypad_Init();
 setup_user_LEDs_PBSW();
 DAC_Init();
 DAC_write(0);
 int iVoltage = 0;
 int iCount = 0;
 uint8_t keyPressed;
 uint8_t lastKeyPressed;

 while (1)
 {
	 keyPressed = Keypad_Read();

	 if ( (keyPressed != lastKeyPressed) && (keyPressed != '\0') && (keyPressed != '*') ){
		 ginumRecv = keyPressed & 0x0F;
			  switch( iCount ) {
			  case 0:
				  //Voltage value ones place
				  iVoltage += ( 1000 * ginumRecv );
				  iCount++;
				  break;
			  case 1:
				  //voltage value in hundreds of mV
				  iVoltage += ( 100 * ginumRecv );
				  iCount++;
				  break;
			  case 2:
				  //voltage value in tens of mV
				  iVoltage += ( 10 * ginumRecv );
				  iCount++;
				  break;
			  default:
				  break;
			  }
		  }

		  if ( (keyPressed == '*') && (keyPressed != lastKeyPressed) ){
			  iCount = 0;
			  iVoltage = 0;
			  GPIOB->ODR &= ~GPIO_PIN_7;
		  }

		  //User has finished inputting desired voltage (in mV)
	 	  if(iCount >= 3){
	 		 GPIOB->ODR |= GPIO_PIN_7;
	 		  if ((iVoltage <= 3300) && (iVoltage != 0) ){
				  dac_data_type iDacVolt = DAC_volt_conv( iVoltage );
				  DAC_write( iDacVolt );
	 		  }
	 		  else if (iVoltage == 0){
				  DAC_write( 0 );
	 		  }
	 		  else if (iVoltage > 3300){
					  DAC_write( 3300 );
		 		  }
	 		  else {
					  dac_data_type iDacVolt = DAC_volt_conv( iVoltage );
					  DAC_write( iVoltage );
			  }


	 	  }
	 	lastKeyPressed = keyPressed;
	  }

}
/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
 RCC_OscInitTypeDef RCC_OscInitStruct = {0};
 RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
 /** Configure the main internal regulator output voltage
 */
 if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
 {
   Error_Handler();
 }
 /** Initializes the RCC Oscillators according to the specified parameters
 * in the RCC_OscInitTypeDef structure.
 */
 RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
 RCC_OscInitStruct.MSIState = RCC_MSI_ON;
 RCC_OscInitStruct.MSICalibrationValue = 0;
 RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
 RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
 if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
 {
   Error_Handler();
 }
 /** Initializes the CPU, AHB and APB buses clocks
 */
 RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                             |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
 RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
 RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
 RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
 RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
 if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
 {
   Error_Handler();
 }
}
/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
/* USER CODE BEGIN 4 */

void setup_user_LEDs_PBSW(void) {
	RCC->AHB2ENR |= ( RCC_AHB2ENR_GPIOBEN |
					  RCC_AHB2ENR_GPIOCEN );
	GPIOB->MODER &= ~GPIO_MODER_MODE7;
	GPIOB->MODER |= GPIO_MODER_MODE7_0;
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT7;
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD7;
	GPIOB->OSPEEDR |= ( 3 << GPIO_OSPEEDR_OSPEED7_Pos );
	GPIOB->BRR |= GPIO_PIN_7;
	GPIOC->MODER &= ~GPIO_MODER_MODE13;
	GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD13;
	GPIOC->PUPDR |= GPIO_PUPDR_PUPD13_1;
}
/* USER CODE END 4 */
/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
 /* USER CODE BEGIN Error_Handler_Debug */
 /* User can add his own implementation to report the HAL error return state */
 __disable_irq();
 while (1)
 {
 }
 /* USER CODE END Error_Handler_Debug */
}
#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
 /* USER CODE BEGIN 6 */
 /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
 /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
