/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * This is the Main.c source code for Lab A2 for CPE329, Spring 2024
  * This code was written by Raheel Rehmatullah and Bradley Buzzini
  *
  * Sources:
  * EE 329 S’24 Lab Manual. (2024-Apr-16). GPIO assignment code, sample code
  *
  ******************************************************************************
  */

#include "main.h"
#include "stdio.h"
#include "keypad.h"


void SystemClock_Config(void);

int main(void){

	// configure GPIO pins PD3, PD4, PD5, PD6 for:
	// output mode, push-pull, no pull up or pull down, high speed
	RCC->AHB2ENR   |=  (RCC_AHB2ENR_GPIODEN);
	GPIOD->MODER   &= ~(GPIO_MODER_MODE3 | GPIO_MODER_MODE4 | GPIO_MODER_MODE5 | GPIO_MODER_MODE6 );
	GPIOD->MODER   |=  (GPIO_MODER_MODE3_0 | GPIO_MODER_MODE4_0 | GPIO_MODER_MODE5_0 | GPIO_MODER_MODE6_0);
	GPIOD->OTYPER  &= ~(GPIO_OTYPER_OT3 | GPIO_OTYPER_OT4 | GPIO_OTYPER_OT5 | GPIO_OTYPER_OT6);
	GPIOD->PUPDR   &= ~(GPIO_PUPDR_PUPD3 | GPIO_PUPDR_PUPD4 | GPIO_PUPDR_PUPD5 | GPIO_PUPDR_PUPD6);
	GPIOD->OSPEEDR |=  ((3 << GPIO_OSPEEDR_OSPEED3_Pos) |
						(3 << GPIO_OSPEEDR_OSPEED4_Pos) |
						 (3 << GPIO_OSPEEDR_OSPEED5_Pos) |
						 (3 << GPIO_OSPEEDR_OSPEED6_Pos));

	 // preset  PD3, PD4, PD5, PD6 to 0
	GPIOD->BSRR |= (GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 );

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();
	SystemClock_Config();
	//Enable keypad
	Keypad_Init();

	uint32_t last_keypad_value;

	while (1){
		//Get the last captured keypad value
		last_keypad_value = Keypad_Read();

		//If the keypad is pressed
		if (last_keypad_value != '\0'){
			switch (last_keypad_value){
				//For the #, set outside LEDs to 0xD
			  case '#':
				  GPIOD->ODR =  11 << 3;
				  break;
			  //For the *, set the inside LEDs 0xE
			  case '*':
				  GPIOD->ODR = 10 << 3;
				  break;
			  //Otherwise, set the leds to the last pressed value
			  default:
				  //Bit shift by 3 to the left because we are toggling pins PD4, PD5, PD6, PD7
				  GPIOD->ODR = last_keypad_value << 3;
				  break;
			}
		}
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
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{

  __disable_irq();
  while (1)
  {
  }
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

}
#endif /* USE_FULL_ASSERT */
