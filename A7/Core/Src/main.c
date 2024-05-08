
#include "main.h"
#include "uart.h"
#include "stdio.h"
#include "string.h"

void SystemClock_Config(void);

extern char keyPressed;

int main(void)
{

  char splash = 'Ω';
  HAL_Init();
  SystemClock_Config();
  LPUART_init();
  LPUART_clearScreen();

  int playerLocation[2] = {0, 12};


  char defaultString[80] =
		  "AAAAAAAAAABBBBBBBBBBAAAAAAAAAABBBBBBBBBBAAAAAAAAAABBBBBBBBBBAAAAAAAAAABBBBBBBBB\0";


  char displayBuffer[24][81];

  for (int row = 0; row < 24; row++){
		strcpy(displayBuffer[row],"////////////////////----------------------------------------\\\\\\\\\\\\\\\\\\\\\0");
  }



  int refreshing = 1;
  int row = 0;


  while (1)
  {
	  if (refreshing == 1){
		  refreshScreen(displayBuffer);
		  refreshing = 0;
	  }

	  switch(keyPressed){
	  case 'w':
		  refreshing = 1;
		  playerLocation[1]--;
		  keyPressed = 0;
		  break;
	  case 's':
		  refreshing = 1;
		  playerLocation[1]++;
		  keyPressed = 0;
		  break;
	  case 'a':
		  refreshing = 1;
		  playerLocation[0]--;
		  keyPressed = 0;
		  break;
	  case 'd':
		  refreshing = 1;
		  playerLocation[0]++;
		  keyPressed = 0;
		  break;
	  default:
		  refreshing = 0;
		  keyPressed = 0;
		  break;
	  }
	  for (int row = 0; row < 24; row++){
	  		strcpy(displayBuffer[row],"                                                                                \0");
	    }

	  displayBuffer [playerLocation[1]] [playerLocation[0]] = 'T';
	  displayBuffer [playerLocation[1] + 1] [playerLocation[0]+1] = 'T';
	  displayBuffer [playerLocation[1] - 1] [playerLocation[0]-1] = 'T';
	  displayBuffer [playerLocation[1] - 1] [playerLocation[0]+1] = 'T';
	  displayBuffer [playerLocation[1] + 1] [playerLocation[0]-1] = 'T';
	  displayBuffer [playerLocation[1]] [playerLocation[0]+1] = 'T';
	  displayBuffer [playerLocation[1]] [playerLocation[0]-1] = 'T';
	  displayBuffer [playerLocation[1] - 1] [playerLocation[0]] = 'T';
	  displayBuffer [playerLocation[1] + 1] [playerLocation[0]] = 'T';





  }
}


void refreshScreen(char displayBuffer[][81]) {

	for (int row = 0; row < 24; row++){
		LPUART_moveCursor(row, 0); // Move cursor to the beginning of the current row
		LPUART_print(displayBuffer[row]); // Print the string from the current row
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
