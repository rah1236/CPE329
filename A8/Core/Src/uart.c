#include "uart.h"

char keyPressed;

void LPUART_init(){
	//power avail and RCC config
	PWR->CR2 |= (PWR_CR2_IOSV);
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOGEN);
	RCC->APB1ENR2 |= RCC_APB1ENR2_LPUART1EN;

	//GPIOG pins and Alt Func config
	//PG7 (TX) and PG8 (RX)
	GPIOG->MODER &= ~(GPIO_MODER_MODE7 | GPIO_MODER_MODE8);
	GPIOG->MODER |= (GPIO_MODER_MODE7_1 | GPIO_MODER_MODE8_1); //AF mode
	GPIOG->OTYPER &= ~(GPIO_PIN_7 | GPIO_PIN_8);
	GPIOG->PUPDR &= ~(GPIO_PUPDR_PUPD7 | GPIO_PUPDR_PUPD8);
	GPIOG->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED7 | GPIO_OSPEEDR_OSPEED8);
	GPIOG->AFR[0] &= ~(GPIO_AFRL_AFSEL7); //pin7 AF to FN8
	GPIOG->AFR[0] |= (GPIO_AFRL_AFSEL7_3);
	GPIOG->AFR[1] &= ~(GPIO_AFRH_AFSEL8); //pin8 AF to FN8
	GPIOG->AFR[1] |= (GPIO_AFRH_AFSEL8_3);

	//UART control reg config
	LPUART1->CR1 &= ~(USART_CR1_M1 | USART_CR1_M0); //8b data mode
	LPUART1->CR1 |= USART_CR1_UE; //en LPUART1
	LPUART1->CR1 |= (USART_CR1_TE | USART_CR1_RE); //en xmit & recv
	LPUART1->CR1 |= USART_CR1_RXNEIE; //en LPUART1 recv int
	LPUART1->ISR &= ~(USART_ISR_RXNE); //clr Recv-Not-Empty flag
	LPUART1->BRR = 256*2000000/115200; //set baud-rate reg

	NVIC->ISER[2] = (1 << (LPUART1_IRQn & 0x1F));   //en LPUART1 ISR
	__enable_irq();
}

void LPUART_print(const char* message){
	//send chars until empty
	uint16_t iStrIdx = 0;
	while(message[iStrIdx]!=0){
		while(!(LPUART1->ISR & USART_ISR_TXE));
		LPUART1->TDR = message[iStrIdx++];
	}
}

void LPUART_printESC(const char* message){
	LPUART_print("\x1B");//ESC
	LPUART_print("[");
	LPUART_print(message);
}

void LPUART1_IRQHandler(void){
	//if data is in recieve buffer then read and decode
//    uint8_t character;
//	if (LPUART1->ISR & USART_ISR_RXNE){
//	  character = LPUART1->RDR;
//	  switch(character){
//	   case 'R':
//		   LPUART_setTextColor(1);
//		   break;
//	   case 'G':
//		   LPUART_setTextColor(2);
//		   break;
//	   case 'B':
//		   LPUART_setTextColor(4);
//		   break;
//	   case 'W':
//		   LPUART_setTextColor(7);
//		   break;
//	   default:
//		  while(!(LPUART1->ISR & USART_ISR_TXE));
//		  LPUART1->TDR = character;
//	  }
//	}
}

void LPUART_moveCursor(int spaces, int lines){
	char spacesStr[16];
	sprintf(spacesStr, "%d", spaces);
	LPUART_printESC(spacesStr);
	char linesStr[16];
	sprintf(linesStr, "%d", lines);
	LPUART_print(";");
	LPUART_print(linesStr);
	LPUART_print("H");
}

void LPUART_resetCursor(){
	LPUART_printESC("H");
}

void LPUART_clearLine(){
	LPUART_printESC("2K");
}

void LPUART_clearScreen(){
	LPUART_printESC("2J");
}

void LPUART_clearAttributes(){
	LPUART_printESC("0m");
}

void LPUART_setBold(){
	LPUART_printESC("1m");
}

void LPUART_setUnderline(){
	LPUART_printESC("4m");
}

void LPUART_setBlink(){
	LPUART_printESC("5m");
}

void LPUART_return(){
	LPUART_printESC("015");
}

void LPUART_setTextColor(unsigned int color){
	char colorStr[16];
	sprintf(colorStr, "%d", color);
	LPUART_printESC("3");
	LPUART_print(colorStr);
	LPUART_print("m");
}

void LPUART_setBackgroundColor(unsigned int color){
	char colorStr[16];
	sprintf(colorStr, "%d", color);
	LPUART_printESC("4");
	LPUART_print(colorStr);
	LPUART_print("m");
}
