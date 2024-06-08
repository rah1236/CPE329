
#include "main.h"
#include "spi.h"
#include "uart.h"
#include "stdlib.h"
#include "tim2_timer.h"
#include "PID.h"
#include "LCD.h"
#include "keypad.h"
#include "data.c"
#include "utils.h"
#include "delay.h"
#include <math.h>

void SystemClock_Config(void);

#define MARGIN_HIGH 0
#define MARGIN_LOW 1
uint32_t newtemp;           // Variable updated each read
uint32_t temp ;             // For storing user input temp
uint8_t state = 0;          // Initial state of FSM
uint32_t timer_ctr = 0;     // Clock divider for interrupt
char buffer [sizeof(uint32_t)*8+1]; // For storing multi-digit numbers



int main(void)
{
    int set_point = 100;    // Initial dummy setpoint
  HAL_Init();
  SystemClock_Config();
  SysTick_Init();           // setup delay function
  Keypad_Config();          // Keypad GPIO setup
  SPI_init();               // Setup SPI for thermocouple
  setup_TIM2(50);   // Initialize TIM2(duty cycle N/A)
  LPUART_init();    // Setup UART for measuring/debugging set point tracking
  LCD_init();       // Setup LCD
  // Initialize GPIOG pin 1 for heating element
    RCC->AHB2ENR   |=  (RCC_AHB2ENR_GPIOGEN); //Heater
    GPIOG->MODER   &= ~(GPIO_MODER_MODE1 );
    GPIOG->MODER   |=  (GPIO_MODER_MODE1_0 );
    GPIOG->OTYPER  &= ~(GPIO_OTYPER_OT1 );
    GPIOG->PUPDR   &= ~(GPIO_PUPDR_PUPD1);
    GPIOG->OSPEEDR |=  (3 << GPIO_OSPEEDR_OSPEED1_Pos);
    GPIOG->ODR &= ~GPIO_PIN_1;
    // Initialize GPIOF pin 0 for fan
    RCC->AHB2ENR   |=  (RCC_AHB2ENR_GPIOFEN); //Fan
    GPIOF->MODER   &= ~(GPIO_MODER_MODE0 );
    GPIOF->MODER   |=  (GPIO_MODER_MODE0_0 );
    GPIOF->OTYPER  &= ~(GPIO_OTYPER_OT0 );
    GPIOF->PUPDR   &= ~(GPIO_PUPDR_PUPD0);
    GPIOF->OSPEEDR |=  (3 << GPIO_OSPEEDR_OSPEED0_Pos);
    GPIOF->ODR &= ~GPIO_PIN_0;

    // Initialize GPIOF pins 7 and 8 for START and SCROLL buttons, respectively
    initialize_pin(GPIOF, 7, INPUT_MODE, 0, PULLUP, HIGH_SPEED);    // Pin 7
    initialize_pin(GPIOF, 8, INPUT_MODE, 0, PULLUP, HIGH_SPEED);    // Pin 8
    uint32_t temp;
    // Flags for initialization of each state
    uint8_t reflow_flag = 0;    // Flag for reflow initialization
    uint8_t user_sp_flag = 0;   // Flag for user set point initialization
    uint8_t sp_flag = 0;        // Flag for set point tracking initialization
    uint8_t start_flag = 0;     // Flag for reflow initialization
    int home_state = 1;         // Toggles home display state
  while (1)
  {
      newtemp = SPI_read();     // Read temp from thermocouple

      timer_ctr = timer_ctr + 1;    // Increment clock div counter
      // FSM for main functionality of machine
      switch(state){
      case 0:   // Home screen state
          // Only run on first time through loop
          if (!start_flag){
              for (int i = 0; i < 100000; i++); // "Debounce" delay
              start_flag = 1;   // Don't run this next time throuhg
              reflow_flag = 0;  // Reset other states
              sp_flag = 0;
              user_sp_flag = 0;
          }
          // Dummy set point (below room temp for safety)
          set_point = 15;
          // If start button pressed...
          if ((GPIOF->IDR & GPIO_PIN_7) == 0){
              LCD_set_cursor(0, 0);
              LCD_write_text("                ");
              LCD_set_cursor(0, 1);
              LCD_write_text("                ");
              state = 3;
              start_flag = 0;
          }
          // If scroll button pressed
          if ((GPIOF->IDR & GPIO_PIN_8) == 0){
              LCD_set_cursor(0, 0);
              LCD_write_text("                ");
              LCD_set_cursor(0, 1);
              LCD_write_text("                ");
              state++;
              start_flag = 0;
          }

          // Change states every 400 main loops (around 2s)
           if (timer_ctr > 400){
               // Toggle display state
               if(home_state == 0)
                   home_state = 1;
               else
                   home_state = 0;
               timer_ctr = 0;
           }
          // Display welcome state on LCD
          if(home_state == 1) {
              LCD_set_cursor(0, 0);
              LCD_write_text("   welcome to   ");
              LCD_set_cursor(0, 1);
              LCD_write_text("- HOTPLATE PRO -");
          }
          // Display instruction state on LCD
          if(home_state == 0) {
              LCD_set_cursor(0, 0);
              LCD_write_text("START:  Reflow  ");
              LCD_set_cursor(0, 1);
              LCD_write_text("SCROLL: Setpoint");
          }
          break;
      case 1:       // User temp select state
          uint8_t user_temp_select_place_index = 3;
          uint16_t user_set_point = 0;  // Initial set point (to be updated)
          // Only run on first time through loop
          if (!user_sp_flag){
              for (int i = 0; i < 100000; i++); // Button "debounce" delay
              user_sp_flag = 1;     // Don't run again
          }
          // If SCROLL button pressed
          if ((GPIOF->IDR & GPIO_PIN_8) == 0){
              // Reset screen
              LCD_set_cursor(0, 0);
              LCD_write_text("                ");
              LCD_set_cursor(0, 1);
              LCD_write_text("                ");
              LCD_set_cursor(0, 0);
              state = 0;    // End and go back to start
          }

          // Wait for user to enter all numbers
          while(user_temp_select_place_index != 0){
            if (Keypad_IsAnyKeyPressed() == 1){
                uint8_t key_pressed = Keypad_WhichKeyIsPressed();
                if (key_pressed >= 0 && key_pressed < 10){  // If real num
                    if (user_temp_select_place_index == 3){ // Hundreds place
                        user_set_point += key_pressed * 100;
                    }
                    else if (user_temp_select_place_index == 2){    // Tens
                        user_set_point += key_pressed * 10;
                    }
                    else if (user_temp_select_place_index == 1){    // Ones
                        user_set_point += key_pressed;
                    }

                    user_temp_select_place_index--; // Go to next digit

                }
                for (int i = 0; i < 50000; i++);    // "Debounce" delay

            }
            // Display current temp and target temp
            LCD_set_cursor(0, 0);
            LCD_write_text("User Target Temp ");
            LCD_set_cursor(0, 1);
            if(user_temp_select_place_index == 3) { // If hundreds place
                LCD_write_text("___ C           ");
            }
            else if(user_temp_select_place_index == 2) {    // Tens place
                itoa(user_set_point/100, buffer, 10);
                LCD_write_text(buffer);
                LCD_set_cursor(1, 1);
                LCD_write_text("__ C");
            }
            else if(user_temp_select_place_index == 1) {   // Ones place
              itoa(user_set_point/10, buffer, 10);
              LCD_write_text(buffer);
              LCD_set_cursor(2, 1);
              LCD_write_text("_ C");
            }
            else if(user_temp_select_place_index == 0) {
                itoa(user_set_point, buffer, 10);
                LCD_write_text(buffer);
                LCD_set_cursor(3, 1);
                LCD_write_text(" C");
            }
           }
           for (int i = 0; i < 300000; i++);    // "Debounce" delay
           // Limit temperature to 300C
           if (user_set_point > 300){
               user_set_point = 300;
           }
           // Clear screen
            LCD_set_cursor(0, 0);
            LCD_write_text("                ");
            LCD_set_cursor(0, 1);
            LCD_write_text("                ");
            set_point = user_set_point; // Set the overall set point
            state++;            // Go to next state (track set point)
            break;
      case 2:       // Tracking user entered set point
          // Only run on first time through loop
          if (!sp_flag){
              for (int i = 0; i < 50000; i++); // "Debounce" delay
              sp_flag = 1;      // Don't run again
          }
          // If SCROLL button pressed:
          if ((GPIOF->IDR & GPIO_PIN_8) == 0){
              // Reset screen
              LCD_set_cursor(0, 0);
              LCD_write_text("                ");
              LCD_set_cursor(0, 1);
              LCD_write_text("                ");
              LCD_set_cursor(0, 0);
              state = 0;        // Back to home state
          }
            for(int i = 0; i < 10000; i++){};   // "Debounce" delay
            // Write target and temp on LCD
            LCD_set_cursor(0, 0);
            LCD_write_text("Targ: ");
            itoa(set_point, buffer, 10);    // Get current set point as txt

            LCD_write_text(buffer);
            LCD_write_text("C");

            if (temp > 0){      // Make sure number > 0
                itoa(temp, buffer, 10);
            }
            LCD_set_cursor(0, 1);   // Display current temp
            LCD_write_text("Temp: ");
            LCD_write_text(buffer);
            LCD_write_text("C");

            break;
      case 3:   // Reflow cycle running
          // Only run on first time through loop
          if (!reflow_flag){
              for (int i = 0; i < 200000; i++);  // Button delay
              reset_TIM2_timer();   // Set timer count to 0 (start timer)
              reflow_flag = 1;      // Don't run on next loop
          }
          // If start button pressed...
            if ((GPIOF->IDR & GPIO_PIN_7) == 0){
                // Clear LCD
                LCD_set_cursor(0, 0);
                LCD_write_text("                ");
                LCD_set_cursor(0, 1);
                LCD_write_text("                ");
                LCD_set_cursor(0, 0);
                state = 0;      // Back to home screen
            }
          setup_TIM2(50);
          int sec = TIM2->CNT/100;  // Convert 100Hz TIM2 ctr to sec passed
          // Total secs in reflow cycle
          int num_secs_total = sizeof(reflow_vals)/sizeof(reflow_vals[0]);
          // If we're done
          if (sec >= num_secs_total){
              TIM2->CR1 &= ~TIM_CR1_CEN;                       // start TIM2 CR1
              state++;
              reflow_flag = 0;
          }
          int percentage = (sec*100)/num_secs_total;    // Percent done
          // Get set-point from pre-loaded array
          set_point = round((double)reflow_vals[sec]);
          for(int i = 0; i < 10000; i++){}; // Brief delay

          // Clear LCD
          LCD_set_cursor(0, 0);
          LCD_write_text("                ");
          LCD_set_cursor(0, 1);
          LCD_write_text("                ");
          LCD_set_cursor(0, 0);
          // Display target (set point)
          LCD_write_text("Targ: ");
          itoa(set_point, buffer, 10);  // Get set point as txt
          LCD_write_text(buffer);
          LCD_write_text("C");

          LCD_set_cursor(11, 0);
          LCD_write_text("Prog: ");
          // Display temp
          itoa(temp, buffer, 10);       // Get temp as txt
          LCD_set_cursor(0, 1);
          LCD_write_text("Temp: ");
          LCD_write_text(buffer);
          LCD_write_text("C   ");
          // Display progress
          itoa(percentage, buffer, 10); // Get percentage as txt
          LCD_write_text(buffer);
          LCD_write_text("%");

          break;
      case 4:   // End of reflow cycle
          set_point = 15;   // Set point below room temp
          for(int i = 0; i < 10000; i++){}; // Brief delay
          // If we are dont cooling
          if (temp < 30){
              state = 0;    // Back to home state
          }
          // Clear LCD
          LCD_set_cursor(0, 0);
          LCD_write_text("                ");
          LCD_set_cursor(0, 1);
          LCD_write_text("                ");
          LCD_set_cursor(0, 0);
          // Write setpoint to LCD
          LCD_write_text("Setpoint = ");
          itoa(set_point, buffer, 10);  // Get set point as txt

          LCD_write_text(buffer);
          LCD_write_text("C");
          // Write temp to LCD
          itoa(temp, buffer, 10);

          LCD_set_cursor(0, 1);
          LCD_write_text("Temp = ");
          LCD_write_text(buffer);
          LCD_write_text("C");
          break;
      }

      // Regardless of case: track the setpoint

      if (newtemp != 0){    // Filter out occasional zeros from thermocouple
           temp = newtemp;
       }
       if (temp > (set_point + MARGIN_HIGH)){   // If over where we want
           GPIOG->ODR &= ~(GPIO_PIN_1);     // Turn off heater
           GPIOF->ODR |= GPIO_PIN_0;        // Turn on fan
       }
       else if (temp < (set_point - MARGIN_LOW)){
           GPIOG->ODR |= GPIO_PIN_1;        // Turn on heater
           GPIOF->ODR &= ~(GPIO_PIN_0);     // Turn off fan
       }

      // Print set point and current temp to UART to measure tracking
      itoa(set_point, buffer, 10);
      LPUART_print(buffer);
      LPUART_print(",");
      itoa(temp, buffer, 10);
      LPUART_print(buffer);
      LPUART_print("\r");
      LPUART_print("\n");


  }
}
/*------------------------------------------------------------------------------
 * Function : TIM2_IRQHandler();
 * IN       : None
 * OUT      : None
 * Action   : Interrupt that is called by TIM2 timer on CCR1 event, calculates
 *            number of ms that have passed and updates static variable
 * Author   : Jack Ryan (jar) - jryan39@calpoly.edu
 * Version  : 0.1
 * Date    `: 240501
------------------------------------------------------------------------------*/
void TIM2_IRQHandler(void) {
   if (TIM2->SR & TIM_SR_CC1IF) {       // triggered by CCR1 event ...
       TIM2->SR &= ~(TIM_SR_CC1IF);
      // GPIOG->ODR &= ~GPIO_PIN_1;        // Turn off heater
   }
   if (TIM2->SR & TIM_SR_UIF) {         // triggered by ARR event ...
       TIM2->SR &= ~(TIM_SR_UIF);
      // GPIOG->ODR |= (GPIO_PIN_1);     // Turn on heater
//      sec_passed += 1/SAMPLE_RATE;      // Increment time
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
