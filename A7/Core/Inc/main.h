/*
 * Main.h for EE329 A7 by Raheel Rehmatullah and Harrison Vinas
 */
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
void Error_Handler(void);
void refresh_screen(char displayBuffer[24][81]);
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
