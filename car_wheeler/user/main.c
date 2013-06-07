/*
 * File: main.c
 * Author: Joe Shang
 * Description: The main program of JoyCar.
 */

#include <stdio.h>
#include "stm32f10x.h"
#include "usart.h"
#include "nvic.h"
#include "rcc.h"
#include "config.h"
#include "command.h"
#include "motor.h"
#include "holder.h"

int main(void)
{
	RCC_Config();
	NVIC_Config();
	USART_Config();
	Motor_Init();
	Holder_Init();

//	USART_SendString(USART1, "JoyCar Project\r\n");

	while (1)	   
	{
		if (command_len)
		{
			parse_command(command, command_len);
			command_len = 0;
		}
	}
}
