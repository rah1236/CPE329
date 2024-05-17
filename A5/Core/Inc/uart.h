#ifndef INC_UART_H_
#define INC_UART_H_

#include "stm32l4xx_hal.h"
#include <stdio.h>

#define _sys_clk 4000000 //system mclk = 4MHz
#define LPUART_baudRate 112500 //uart baud rate 112.5kbps

void LPUART_init();
void LPUART_print(const char* message);
void LPUART_printESC(const char* message);
void LPUART1_IRQHandler(void);

//ESC codes
void LPUART_moveCursor(int spaces, int lines);
void LPUART_resetCursor();
void LPUART_clearLine();
void LPUART_clearScreen();
void LPUART_clearAttributes();
void LPUART_setBold();
void LPUART_setUnderline();
void LPUART_setBlink();
void LPUART_setTextColor(unsigned int color);
void LPUART_setBackgroundColor(unsigned int color);

#endif
