/*
 * File: usart.h
 * Author: Joe Shang
 * Description: The drivers of USART module.
 */

#ifndef _USART_H_
#define _USART_H_

#include "stm32f10x.h"

void Usart_Initialization(void);

void Usart_SendChar(USART_TypeDef* USARTx, char data);
char Usart_GetChar(USART_TypeDef* USARTx);

#endif
