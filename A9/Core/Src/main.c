/* USER CODE BEGIN Header */
/*******************************************************************************
 * EE 329 A9: EEPROM!
 *******************************************************************************
 * @file           : main.c
 * @brief          : This is an example of using I2C to write to an EEPROM
 * Specifically the 24LC256 from Microchip
 * project         : EE 329 S'24 Assignment 5
 * authors         : Raheel Rehmatullah and Seth Cherry
 * version         : 1
 * date            : 5_22_2024
 * compiler        : STM32CubeIDE v.1.15.0
 * target          : NUCLEO-L4A6ZG
 * clocks          : 4 MHz MSI to AHB2
 * @attention      : (c) 2023 STMicroelectronics.  All rights reserved.
 *******************************************************************************
/* USER CODE END Header */
#include "main.h"
#include "i2c.h"

#define DEVICE_ADDRESS 0xA2
#define DATA_TO_SEND 0x55

void SystemClock_Config(void);

int main(void)
{



  HAL_Init();

  SystemClock_Config();
  I2C_init();

  //Write Data to EEPROM
  I2C_write(DEVICE_ADDRESS, DATA_TO_SEND, 0x05);

  //Delay
  for(int i = 0; i < 50000; i++);

  //Check if the data read is the same as received, if so, turn on the LED
  if(DATA_TO_SEND == (I2C_read( DEVICE_ADDRESS,  0x05))){
	  GPIOB->ODR |= (GPIO_ODR_OD7_Pos);
  }


  while (1){

  }
}

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

void user_io_init(void){
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOCEN);   // enable GPIOC clock
	GPIOC->MODER &= ~(GPIO_MODER_MODE13); // Set button pin to input mode
	GPIOC->PUPDR |= (GPIO_PUPDR_PUPD13_1); //Enable pull up
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
