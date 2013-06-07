/*
 * File: usart.h
 * Author: Joe Shang
 * Description: The drivers of USART module.
 */

#ifndef _USART_H_
#define _USART_H_

#include "stm32f10x.h"

void USART_Config(void);

void USART_SendChar(USART_TypeDef* USARTx, char data);
char USART_GetChar(USART_TypeDef* USARTx);

void USART_SendString(USART_TypeDef* USARTx, char* string);

#endif
