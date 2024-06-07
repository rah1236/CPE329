
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

#define KP 30
#define KI 100
#define KD 10

#define MARGIN_HIGH 0
#define MARGIN_LOW 1
uint32_t newtemp;
uint32_t temp ;
uint8_t state = 0;
uint32_t timer_ctr = 0;
char buffer [sizeof(uint32_t)*8+1];


//static int sec_passed = 0;

int main(void)
{
    int set_point = 100;
  HAL_Init();
  SystemClock_Config();
  SysTick_Init();                     //setup delay function
  Keypad_Config();
  SPI_init();
  setup_TIM2(50);
  LPUART_init();
  LCD_init();
    RCC->AHB2ENR   |=  (RCC_AHB2ENR_GPIOGEN); //Heater
    GPIOG->MODER   &= ~(GPIO_MODER_MODE1 );
    GPIOG->MODER   |=  (GPIO_MODER_MODE1_0 );
    GPIOG->OTYPER  &= ~(GPIO_OTYPER_OT1 );
    GPIOG->PUPDR   &= ~(GPIO_PUPDR_PUPD1);
    GPIOG->OSPEEDR |=  (3 << GPIO_OSPEEDR_OSPEED1_Pos);
    GPIOG->ODR &= ~GPIO_PIN_1;

    RCC->AHB2ENR   |=  (RCC_AHB2ENR_GPIOFEN); //Fan
    GPIOF->MODER   &= ~(GPIO_MODER_MODE0 );
    GPIOF->MODER   |=  (GPIO_MODER_MODE0_0 );
    GPIOF->OTYPER  &= ~(GPIO_OTYPER_OT0 );
    GPIOF->PUPDR   &= ~(GPIO_PUPDR_PUPD0);
    GPIOF->OSPEEDR |=  (3 << GPIO_OSPEEDR_OSPEED0_Pos);
    GPIOF->ODR &= ~GPIO_PIN_0;

    initialize_pin(GPIOF, 7, INPUT_MODE, 0, PULLUP, HIGH_SPEED);
    uint32_t temp;
    uint8_t reflow_flag = 0;
    int home_state = 1;
  while (1)
  {
      newtemp = SPI_read();

      timer_ctr = timer_ctr + 1;
      switch(state){
      case 0:
          set_point = 15;
          // If start button pressed...
          if ((GPIOF->IDR & GPIO_PIN_7) == 0){
              LCD_set_cursor(0, 0);
              LCD_write_text("                ");
              LCD_set_cursor(0, 1);
              LCD_write_text("                ");
              state = 3;
          }
          /*LCD_set_cursor(0, 0);
            LCD_write_text("click * to enter ");
            LCD_set_cursor(0, 1);
            LCD_write_text("a setpoint  ");
            for (int i = 0; i < 50 0000; i++);

            LCD_set_cursor(0, 0);
            LCD_write_text("or click 'START'");
            LCD_set_cursor(0, 1);
            LCD_write_text("  to reflow  ");


            for (int i = 0; i < 500000; i++);*/
           if (timer_ctr > 200){
               if(home_state == 0)
                   home_state = 1;
               else
                   home_state = 0;
               timer_ctr = 0;
           }
          if(home_state == 1) {
              LCD_set_cursor(0, 0);
              LCD_write_text("click * to enter ");
              LCD_set_cursor(0, 1);
              LCD_write_text("a setpoint  ");
          }
          if(home_state == 0) {
              LCD_set_cursor(0, 0);
              LCD_write_text("or click 'START'");
              LCD_set_cursor(0, 1);
              LCD_write_text("  to reflow  ");
          }
          if (Keypad_IsAnyKeyPressed() == 1){
              if (Keypad_WhichKeyIsPressed() == 10){
                  LCD_set_cursor(0, 0);
                  LCD_write_text("                ");
                  LCD_set_cursor(0, 1);
                  LCD_write_text("                ");

                  state++;
              }
              /*else if (Keypad_WhichKeyIsPressed() == 15){
                  LCD_set_cursor(0, 0);
                  LCD_write_text("                ");
                  LCD_set_cursor(0, 1);
                  LCD_write_text("                ");

                  state = 3;
              }*/

          }
          break;
      case 1:
        uint8_t user_temp_select_place_index = 3;
        uint8_t user_set_point;

        while(user_temp_select_place_index != 0){
          if (Keypad_IsAnyKeyPressed() == 1){
              uint8_t key_pressed = Keypad_WhichKeyIsPressed();
              if (key_pressed >= 0 && key_pressed < 10){
                  if (user_temp_select_place_index == 3){
                      user_set_point += key_pressed * 100;
                  }
                  else if (user_temp_select_place_index == 2){
                      user_set_point += key_pressed * 10;
                  }
                  else if (user_temp_select_place_index == 1){
                      user_set_point += key_pressed;
                  }

                  user_temp_select_place_index--;

              }


          }
          LCD_set_cursor(0, 0);
          LCD_write_text("User setpoint = ");

          LCD_set_cursor(0, 1);
          itoa(user_set_point, buffer, 10);
          LCD_write_text(buffer);
          LCD_write_text("C");
        }

          LCD_set_cursor(0, 0);
          LCD_write_text("                ");
          LCD_set_cursor(0, 1);
          LCD_write_text("                ");
          set_point = user_set_point;
          state++;
          break;
      case 2:


            //set_input(temp);
            for(int i = 0; i < 10000; i++){};
            LCD_set_cursor(0, 0);
            LCD_write_text("Setpoint = ");
            itoa(set_point, buffer, 10);

            LCD_write_text(buffer);
            LCD_write_text("C");

            if (temp > 0){
                itoa(temp, buffer, 10);
            }
            LCD_set_cursor(0, 1);
            LCD_write_text("Temp = ");
            LCD_write_text(buffer);
            LCD_write_text("C");

            break;
      case 3:
          if (!reflow_flag){
              reset_TIM2_timer();
              reflow_flag = 1;
          }
          setup_TIM2(50);
          int sec = TIM2->CNT/100;
          int num_secs_total = sizeof(reflow_vals)/sizeof(reflow_vals[0]);
          if (sec >= num_secs_total){
              TIM2->CR1 &= ~TIM_CR1_CEN;                       // start TIM2 CR1
              state++;
              reflow_flag = 0;
          }
          set_point = round((double)reflow_vals[sec]);
          for(int i = 0; i < 10000; i++){};

          LCD_set_cursor(0, 0);
          LCD_write_text("                ");
          LCD_set_cursor(0, 1);
          LCD_write_text("                ");
          LCD_set_cursor(0, 0);
          LCD_write_text("Setpoint = ");
          itoa(set_point, buffer, 10);

          LCD_write_text(buffer);
          LCD_write_text("C");

          itoa(temp, buffer, 10);

          LCD_set_cursor(0, 1);
          LCD_write_text("Temp = ");
          LCD_write_text(buffer);
          LCD_write_text("C");
          break;
      case 4:
          set_point = 15;
          for(int i = 0; i < 10000; i++){};
          if (temp < 30){
              state = 0;
          }

          LCD_set_cursor(0, 0);
          LCD_write_text("                ");
          LCD_set_cursor(0, 1);
          LCD_write_text("                ");
          LCD_set_cursor(0, 0);
          LCD_write_text("Setpoint = ");
          itoa(set_point, buffer, 10);

          LCD_write_text(buffer);
          LCD_write_text("C");

          itoa(temp, buffer, 10);

          LCD_set_cursor(0, 1);
          LCD_write_text("Temp = ");
          LCD_write_text(buffer);
          LCD_write_text("C");
          break;
      }


      if (newtemp != 0){
           temp = newtemp;
       }
       if (temp > (set_point + MARGIN_HIGH)){
           GPIOG->ODR &= ~(GPIO_PIN_1);     // Turn off
           GPIOF->ODR |= GPIO_PIN_0;
       }
       else if (temp < (set_point - MARGIN_LOW)){
           GPIOG->ODR |= GPIO_PIN_1;         // Turn on
           GPIOF->ODR &= ~(GPIO_PIN_0);
       }

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
