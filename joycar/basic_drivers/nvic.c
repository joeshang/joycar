/*
 * File: nvic.c
 * Author: Joe Shang
 * Description: The configurations of NVIC module.
 */
 
#include "nvic.h"
#include "stm32f10x.h"
#include "command.h"
			   
void NVIC_Config(void)
{	
	NVIC_InitTypeDef NVIC_InitStruct;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);

	/* usart1 interrupt setting */
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}

void USART1_IRQHandler(void)
{
	char recv = USART_ReceiveData(USART1);

	command_len = construct_command(recv, command);

//	if (USART_GetFlagStatus(USART1, USART_FLAG_TXE))
//	{						
//		USART_SendData(USART1, recv);	
//	}	
}
