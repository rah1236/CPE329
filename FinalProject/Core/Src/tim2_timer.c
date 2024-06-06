/*******************************************************************************
 * EE 329 TIM2 Interrupt Timer
 *******************************************************************************
 * @file        : tim2_timer.c
 * @brief       : Functionality for use of TIM2 timer with interrupts
 * project      : EE_329_A4
 * authors      : Ryan Marienthal (rem) - remarien@calpoly.edu,
 *                Jack Ryan (jar) - jryan39@calpoly.edu
 * version      : 0.1
 * date         : 240430
 * compiler     : STM32CubeIDE v.1.12.0 Build: 14980_20230301_1550 (UTC)
 * target       : NUCLEO-L4A6ZG
 * clocks       : 4 MHz MSI to AHB2
 * @attention   : (c) 2023 STMicroelectronics. All rights reserved.
 ******************************************************************************/
#include "tim2_timer.h"
#include "utils.h"
#include "PID.h"

static int sec_passed = 0;

const int kp = 30;
const int ki = 100;
const int kd = 10;

/*------------------------------------------------------------------------------
 * Function : setup_TIM2();
 * IN       : iDutyCycle - integer that determines default duty cycle of TIM2
 *            clock signal
 * OUT      : None
 * Action   : Initialize TIM2 and enable interrupts to start
 * Author   : Jack Ryan (jar) - jryan39@calpoly.edu
 * Version  : 0.1
 * Date    `: 240501
------------------------------------------------------------------------------*/
void setup_TIM2( int iDutyCycle ) {
    //Configure GPIO pins PA0 and PA1 for TIM2_CH1 and TIM2_CH2
    initialize_pin(GPIOA, 0, ALT_FUNCT_MODE, 0, 0, HIGH_SPEED);
    initialize_pin(GPIOA, 1, ALT_FUNCT_MODE, 0, 0, HIGH_SPEED);
    GPIOA->AFR[0] &= ~(0xF << 0);
    GPIOA->AFR[0] |= (0x1);
    GPIOA->AFR[0] &= ~(0xF << 4);
    GPIOA->AFR[0] |= (0x1 << 4);
   // Enable clock and interrupts
   RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;           // enable clock for TIM2
   TIM2->DIER |= (TIM_DIER_CC1IE | TIM_DIER_UIE);  // enable event gen, rcv CCR1
   TIM2->ARR = PERIOD;                             // ARR = T = counts @4MHz
   TIM2->CCR1 = iDutyCycle;                        // ticks for duty cycle
   TIM2->SR &= ~(TIM_SR_CC1IF | TIM_SR_UIF);       // clr IRQ flag in status reg
   NVIC->ISER[0] |= (1 << (TIM2_IRQn & 0x1F));     // set NVIC interrupt: 0x1F
   __enable_irq();                                 // global IRQ enable
   TIM2->CR1 |= TIM_CR1_CEN;                       // start TIM2 CR1
}

/*------------------------------------------------------------------------------
 * Function : reset_TIM2_timer();
 * IN       : None
 * OUT      : None
 * Action   : Reset TIM2 clock and CCR to be correct value
 * Author   : Jack Ryan (jar) - jryan39@calpoly.edu
 * Version  : 0.1
 * Date    `: 240501
------------------------------------------------------------------------------*/
void reset_TIM2_timer(void){
    TIM2->CNT = 0;              // Set counter to 0
    TIM2->CCR1 = CCR_INC;       // Reset CCR to correct starting spot
    sec_passed = 0;              // Reset ms counter
    __enable_irq();             // Enable interrupts
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
       GPIOG->ODR |= GPIO_PIN_1;        // Turn on heater
   }
   if (TIM2->SR & TIM_SR_UIF) {         // triggered by ARR event ...
       TIM2->SR &= ~(TIM_SR_UIF);
       GPIOG->ODR &= ~(GPIO_PIN_1);     // Turn off heater
      sec_passed += 1/SAMPLE_RATE;      // Increment time
      int duty = pid_output(kp, ki, kd);// Get PID response
      TIM2_set_duty_cycle(duty);        // Set duty cycle based on PID
   }
}

/*------------------------------------------------------------------------------
 * Function : get_rand_num();
 * IN       : None
 * OUT      : uint32_t - 32-bit integer read from RNG data register
 * Action   : Accesses RNG, checks that there are no error flags, and prints
 * Author   : John Penvenne (jlp) - jpenvenn@calpoly.edu
 * Version  : 0.1
 * Date    `: N/A
------------------------------------------------------------------------------*/
void setup_MCO_CLK(void) {
   // Enable MCO, select MSI (4 MHz source)
   RCC->CFGR = ((RCC->CFGR & ~(RCC_CFGR_MCOSEL)) | (RCC_CFGR_MCOSEL_0));
   // Configure MCO output on PA8
   RCC->AHB2ENR   |=  (RCC_AHB2ENR_GPIOAEN);
   GPIOA->MODER   &= ~(GPIO_MODER_MODE8);       // clear MODER bits
   GPIOA->MODER   |=  (GPIO_MODER_MODE8_1); // set alternate function mode
   GPIOA->OTYPER  &= ~(GPIO_OTYPER_OT8);        // Push-pull output
   GPIOA->PUPDR   &= ~(GPIO_PUPDR_PUPD8);       // no resistor
   GPIOA->OSPEEDR |=  (GPIO_OSPEEDR_OSPEED8);   // high speed
   GPIOA->AFR[1]  &= ~(GPIO_AFRH_AFSEL8);       // select MCO function
}

void TIM2_set_duty_cycle(int percentage){
    TIM2->CCR1 = PERIOD*percentage;
}

// General getter for main to access number of ms that have passed
int get_sec_passed(void){
    return sec_passed;
}


