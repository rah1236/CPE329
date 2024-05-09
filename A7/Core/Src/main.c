/* USER CODE BEGIN Header */
/*******************************************************************************
 * EE 329 A7: UART
 *******************************************************************************
 * @file           : main.c
 * @brief          : This is a small game like interface that
 * 					 streams over uart to your terminal
 * project         : EE 329 S'24 Assignment 7
 * authors         : Raheel Rehmatullah and Harrison Vinas
 * version         : 1
 * date            : 5_8_2024
 * compiler        : STM32CubeIDE v.1.15.0
 * target          : NUCLEO-L4A6ZG
 * clocks          : 4 MHz MSI to AHB2
 * @attention      : (c) 2023 STMicroelectronics.  All rights reserved.
 *******************************************************************************
 * Pin Map:
 *
 * PG7 - UART TX
 * PG8 - UART RX
 *
 *******************************************************************************
 *
 * UART Code credit to Braydon Burkhardt May 23, 2023
 *
 *******************************************************************************/
/* USER CODE END Header */

#include "main.h"
#include "uart.h"
#include "stdio.h"
#include "string.h"

void SystemClock_Config(void);

extern char keyPressed;

int main(void)
{

  HAL_Init();
  SystemClock_Config();
  //Init UART and clear the screen
  LPUART_init();
  LPUART_clearScreen();

  // Player location array, where [0] is on the X axis, and [1] is on the Y axis
  // Positive is going down and to the right
  int8_t player_location[2] = {40, 12};

  //Defining Display buffer
  char display_buffer[24][81];

  //For loop for the splash screen
  //Does a funky little animation for a few seconds
  for (int i; i < 10; i++){

	  for (int row = 0; row < 24; row++){
		  if(row < 10){
			strcpy(display_buffer[row],
					"////////////////////----------------------------------------||||||||||||||||||||\0");
		  }
		  else if (row > 10 && row < 12){
			strcpy(display_buffer[row],
					"----------------------------- Mike's Floating Eye ------------------------------\0");
		  }
		  else{
			strcpy(display_buffer[row],
					"||||||||||||||||||||----------------------------------------////////////////////\0");
		  }

	  }

	  refresh_screen(display_buffer);


	  for (int row = 0; row < 24; row++){
		  if(row < 10){
			strcpy(display_buffer[row],
					"||||||||||||||||||||----------------------------------------////////////////////\0");
		  }
		  else if (row > 10 && row < 12){
			strcpy(display_buffer[row],
					"----------------------------- Mike's Floating Eye ------------------------------\0");
		  }
		  else{
			strcpy(display_buffer[row],
					"////////////////////----------------------------------------||||||||||||||||||||\0");
		  }

	  }

	  refresh_screen(display_buffer);
  }
  int refreshing = 1;

  //Starting infinite loop
  while (1)
  {

	  //Defining border
	  for (int row = 0; row < 24; row++){
	  		strcpy(display_buffer[row],
	  				"                                                                                \0");
	  		display_buffer [row] [0] = '@';
	  		display_buffer [row] [79] = '@';
	   }
	  strcpy(display_buffer [1] ,
			  "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\0");
	  strcpy(display_buffer [23],
			  "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\0");

	  //Defining player deadzones (to bring them to the otherside of the border)
	  if (player_location[0] < 3) {
		  player_location[0] = 77;
	  }
	  else if(player_location[0] > 77){
		  player_location[0] = 3;
	  }
	  if (player_location[1] < 3) {
	  		  player_location[1] = 21;
	  }
	  else if(player_location[1] > 21){
		  player_location[1] = 3;
	  }

	  //defining the 'sprite' for the player character
	  display_buffer [player_location[1]] [player_location[0]] = '*';
	  display_buffer [player_location[1] + 1] [player_location[0]+1] = '/';
	  display_buffer [player_location[1] - 1] [player_location[0]-1] = '/';
	  display_buffer [player_location[1] - 1] [player_location[0]+1] = '\\';
	  display_buffer [player_location[1] + 1] [player_location[0]-1] = '\\';
	  display_buffer [player_location[1]] [player_location[0]+1] = '|';
	  display_buffer [player_location[1]] [player_location[0]-1] = '|';
	  display_buffer [player_location[1] - 1] [player_location[0]] = '-';
	  display_buffer [player_location[1] + 1] [player_location[0]] = '-';


	  //Refresh the screen when the flag is set to do so
	  if (refreshing == 1){
		  refresh_screen(display_buffer);
		  refreshing = 0;
	  }

	  //Switch case to check for player input, refresh upon key press, also moves the player location
	  switch(keyPressed){
	  case 'w':
		  refreshing = 1;
		  player_location[1]--;
		  keyPressed = 0;
		  break;
	  case 's':
		  refreshing = 1;
		  player_location[1]++;
		  keyPressed = 0;
		  break;
	  case 'a':
		  refreshing = 1;
		  player_location[0]--;
		  keyPressed = 0;
		  break;
	  case 'd':
		  refreshing = 1;
		  player_location[0]++;
		  keyPressed = 0;
		  break;
	  default:
		  refreshing = 0;
		  keyPressed = 0;
		  break;
	  }



  }
}



// Refreshes the screen, accepts a display_buffer 2D array,
// Expects a display that is 24 rows long, and 80 characters wide
void refresh_screen(char displayBuffer[24][81]) {

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
