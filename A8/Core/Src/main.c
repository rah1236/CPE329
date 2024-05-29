
#include "main.h"
#include "uart.h"
#include "delay.h"

#define SAMPLE_COUNT 50

uint8_t ADC_conversion_flag = 0;
uint16_t ADC_last_read;
uint16_t ADC_readings[SAMPLE_COUNT];
uint8_t ADC_reading_index = 0;
uint16_t ADC_average;
uint16_t ADC_min = 4095;
uint16_t ADC_max = 0;

void SystemClock_Config(void);

int main(void)
{

  HAL_Init();
  SystemClock_Config();
  SysTick_Init();
  LPUART_init();
  ADC_init();



  while (1)
  {

	  while( !ADC_conversion_flag ){ // *global* await next ISR trip




			if (ADC1->DR < ADC_min) {
				ADC_min = ADC1->DR;
			}


			if (ADC1->DR > ADC_max){
				ADC_max = ADC1->DR;
			}

			uint32_t sum = 0;
			for (int index = 0; index < SAMPLE_COUNT; index++){
					sum += ADC_readings[index];
			}

			ADC_average = (sum/SAMPLE_COUNT);

//		  ADC_average = get_average(ADC_readings, 100);
		  //ADC_min = get_min(ADC_readings, 100);
		  //ADC_max = get_max(ADC_readings, 100);

		  char* ADC_max_cnt_str = int_to_str(ADC_max);
		  char* ADC_min_cnt_str = int_to_str(ADC_min);
		  char* ADC_avg_cnt_str = int_to_str(ADC_average);

		  char* ADC_max_str = int_to_str(ADC_max * 3300/4095);
		  char* ADC_min_str = int_to_str((ADC_min * 3300/4095) / 1000);
		  char* ADC_avg_str = int_to_str(ADC_average * 3300/4095);

		  LPUART_print("ADC counts volts\0");
		  LPUART_moveCursor(2,0);
		  LPUART_print("MIN   ");
		  LPUART_print(ADC_min_cnt_str);
		  LPUART_print("   ");
		  LPUART_print(ADC_min_str);
		  LPUART_print(".");
//		  LPUART_print((ADC_min * 3300/4095)/100 | 0x30);
//		  LPUART_print((ADC_min * 3300/4095)/10 | 0x30);


		  LPUART_print("\r");


		  delay_us(5000);
		  LPUART_clearScreen();
		  LPUART_resetCursor();

		  free(ADC_max_cnt_str);
		  free(ADC_min_cnt_str);
		  free(ADC_avg_cnt_str);
		  free(ADC_max_str);
		  free(ADC_min_str);
		  free(ADC_avg_str);

		  ADC_conversion_flag = 0;                         //Reset Flag
		  ADC1->CR |= ADC_CR_ADSTART;                // start 1st conversion

	  }


  }
}

char* int_to_str(int num) {
    int len = 0, tmp = num;

    // Determine the length of the string
    do {
        len++;
        tmp /= 10;
    } while (tmp != 0);

    // Allocate memory for the string
    char* str = (char*) malloc(sizeof(char) * (len + 1));

    // Convert integer to string
    int i = len - 1;
    tmp = num;
    str[len] = '\0'; // Null terminator

    while (tmp != 0) {
        str[i--] = tmp % 10 + '0';
        tmp /= 10;
    }

    return str;
}

uint16_t get_average(uint16_t array[], uint16_t size_of_array){
	uint32_t sum;
	for (int index = 0; index < size_of_array; index++){
		sum += array[index];
	}
	return(sum/size_of_array);
}

uint16_t get_min(uint16_t array[], uint16_t size_of_array){
	uint16_t min = 0xFFFF;
	for (int index = 0; index < size_of_array; index++){
			if (array[index] < min) {
				min = array[index];
			}
		}
	return (min);
}

uint16_t get_max(uint16_t array[], uint16_t size_of_array){
	uint32_t max = 0;
	for (int index = 0; index < size_of_array; index++){
			if (array[index] > max){
				max = array[index];
			}
		}
	return(max);
}

void ADC_init(void){
	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;         // turn on clock for ADC
	// power up & calibrate ADC
	ADC123_COMMON->CCR |= (1 << ADC_CCR_CKMODE_Pos); // clock source = HCLK/1
	ADC1->CR &= ~(ADC_CR_DEEPPWD);             // disable deep-power-down
	ADC1->CR |= (ADC_CR_ADVREGEN);             // enable V regulator - see RM 18.4.6
	delay_us(20);                              // wait 20us for ADC to power up
	ADC1->DIFSEL &= ~(ADC_DIFSEL_DIFSEL_5);    // PA0=ADC1_IN5, single-ended
	ADC1->CR &= ~(ADC_CR_ADEN | ADC_CR_ADCALDIF); // disable ADC, single-end calib
	ADC1->CR |= ADC_CR_ADCAL;                  // start calibration
	while (ADC1->CR & ADC_CR_ADCAL) {;}        // wait for calib to finish
	// enable ADC
	ADC1->ISR |= (ADC_ISR_ADRDY);              // set to clr ADC Ready flag
	ADC1->CR |= ADC_CR_ADEN;                   // enable ADC
	while(!(ADC1->ISR & ADC_ISR_ADRDY)) {;}    // wait for ADC Ready flag
	ADC1->ISR |= (ADC_ISR_ADRDY);              // set to clr ADC Ready flag
	// configure ADC sampling & sequencing
	ADC1->SQR1  |= (5 << ADC_SQR1_SQ1_Pos);    // sequence = 1 conv., ch 5
	ADC1->SMPR1 |= (1 << ADC_SMPR1_SMP5_Pos);  // ch 5 sample time = 6.5 clocks
	ADC1->CFGR  &= ~( ADC_CFGR_CONT  |         // single conversion mode
	                  ADC_CFGR_EXTEN |         // h/w trig disabled for s/w trig
	                  ADC_CFGR_RES   );        // 12-bit resolution
	// configure & enable ADC interrupt
	ADC1->IER |= ADC_IER_EOCIE;                // enable end-of-conv interrupt
	ADC1->ISR |= ADC_ISR_EOC;                  // set to clear EOC flag
	NVIC->ISER[0] = (1<<(ADC1_2_IRQn & 0x1F)); // enable ADC interrupt service
	__enable_irq();                            // enable global interrupts
	// configure GPIO pin PA0
	RCC->AHB2ENR  |= (RCC_AHB2ENR_GPIOAEN);    // connect clock to GPIOA
	GPIOA->MODER  |= (GPIO_MODER_MODE0);       // analog mode for PA0 (set MODER last)

	ADC1->CR |= ADC_CR_ADSTART;                // start 1st conversion

}

void ADC1_2_IRQHandler( void ) {
	if ( ADC1->ISR & ADC_ISR_EOC ) { // conversion done?
		ADC_last_read = ADC1->DR;
		if ( ADC_reading_index > SAMPLE_COUNT - 1){
				  ADC_reading_index = 0;                       //Set index to 0 at the end
		}
		ADC_readings[ADC_reading_index] = ADC_last_read;      // get data
		ADC_reading_index++;                             //Increment index
		ADC_conversion_flag = 1; // tell main(): got one
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
