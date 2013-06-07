/*
 * File: main.c
 * Author: Joe Shang
 * Description: The main program of JoyCar.
 */

#include "usart.h"
#include "stm32f10x.h"
#include <stdio.h>

int main(void)
{
	char data;
	Usart_Initialization();

	while(1)	   
	{
		data = Usart_GetChar(USART1);
		Usart_SendChar(USART1, data);
	}
}
