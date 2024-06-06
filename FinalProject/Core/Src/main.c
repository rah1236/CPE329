
#include "main.h"
#include "spi.h"
#include "uart.h"
#include "stdlib.h"
#include "PID.h"

void SystemClock_Config(void);
int main(void)
{

  HAL_Init();
  SystemClock_Config();
  SPI_init();
  LPUART_init();
    RCC->AHB2ENR   |=  (RCC_AHB2ENR_GPIOGEN); //Heater
  	GPIOG->MODER   &= ~(GPIO_MODER_MODE1 );
  	GPIOG->MODER   |=  (GPIO_MODER_MODE1_0 );
  	GPIOG->OTYPER  &= ~(GPIO_OTYPER_OT1 );
  	GPIOG->PUPDR   &= ~(GPIO_PUPDR_PUPD1);
  	GPIOG->OSPEEDR |=  (3 << GPIO_OSPEEDR_OSPEED1_Pos);
  	GPIOG->ODR |= GPIO_PIN_1;

    RCC->AHB2ENR   |=  (RCC_AHB2ENR_GPIOFEN); //Fan
  	GPIOF->MODER   &= ~(GPIO_MODER_MODE0 );
  	GPIOF->MODER   |=  (GPIO_MODER_MODE0_0 );
  	GPIOF->OTYPER  &= ~(GPIO_OTYPER_OT0 );
  	GPIOF->PUPDR   &= ~(GPIO_PUPDR_PUPD0);
  	GPIOF->OSPEEDR |=  (3 << GPIO_OSPEEDR_OSPEED0_Pos);
  	GPIOF->ODR &= ~GPIO_PIN_0;

//	GPIOG->ODR &= ~GPIO_PIN_1;
//  	GPIOF->ODR |= GPIO_PIN_0;

  while (1)
  {
	  char buffer [sizeof(uint32_t)*8+1];
	  uint32_t temp = SPI_read();
    set_input(temp);

//	  if (temp > 0){
		  itoa(temp, buffer, 10);
//	  }

	  for(int i = 0; i < 10000; i++){};
	  LPUART_print(buffer);
	  LPUART_print("\r");
	  LPUART_print("\n");

	  if (temp > 220){
		GPIOG->ODR &= ~GPIO_PIN_1;
	  	GPIOF->ODR |= GPIO_PIN_0;
	  }

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
