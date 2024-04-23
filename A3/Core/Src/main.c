
#include "main.h"
#include "keypad.h"
#include "lcd.h"
#include "ctype.h"

void SystemClock_Config(void);

int main(void)
{


  HAL_Init();

  SystemClock_Config();
  Keypad_Init();
  Lcd_Init();


  Lcd_write_string("EE 329 A3 TIMER", 0);
  char bottomString[] = "*=SET #=GO 00:00";


  uint8_t state = 0;
  uint8_t time_idx;

  uint8_t currentInput;
  uint8_t lastInput = '\0';


  while (1)
  {
	  //Get currently pressed key
	  currentInput = Keypad_Read();

	  if (lastInput != currentInput){
		  if(currentInput == '*'){
			  state = 0;
		  }
	  }

	  Lcd_write_string(bottomString, 1);

	  //debug:
	  bottomString[10] = state | 0x30;

	  switch (state){

	  case 0: //Idle or 'Reset' state
		  Lcd_write_string("EE 329 A3 TIMER", 0);
		  bottomString[11] = '0';
		  bottomString[12] = '0';
		  bottomString[14] = '0';
		  bottomString[15] = '0';
		  time_idx = 0;

		  delay_us(500000);
		  if (lastInput != currentInput){
		  		  if(currentInput == '*'){
		  			  state++;
		  			  break;
		  		  }
		  }
		  break;
	  case 1:

		  // Let user know that they can input now
		  Lcd_write_string("Enter a time:", 0);

		  //If the current input is a number
		  if (isdigit(currentInput) && currentInput != '\0' && currentInput != '#'){
			  //And if there is an input delta
			  if (lastInput != currentInput){
				  //if the index is at the first 2 digits
				  if(time_idx < 2){
					  bottomString[time_idx + 11] = currentInput;
				  }
				  //if the index is at the last 2 digits
				  else{
					  bottomString[time_idx + 12] = currentInput;
				  }
				  //Add to time digit index
				  time_idx++;
			  }

		  }
		  //Check if we have reached the 4th digit
		  if (time_idx >= 4) {
			  //Jump to state 2
			  state++;
			  //Number conditioning
			  if( ((int)bottomString[11] & 0x0f) > 5){
				  bottomString[11] = '5';
			  }
			  if( ((int)bottomString[14] & 0x0f) > 5){
				  bottomString[14] = '5';
			  }
			  break;
		  }
		  break;

	  case 2:
		  Lcd_write_string("'#' to start", 0);
		  if(Keypad_Read() == '#'){
			  state++;
		  }


		  break;
	  case 3:

		  break;

	  }

	  //Record last known input for checking delta later
	  lastInput = currentInput;
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

int __io_putchar(int ch) {
    ITM_SendChar(ch);
    return ch;
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
