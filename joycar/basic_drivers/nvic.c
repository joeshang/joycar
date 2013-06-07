/*
 * File: nvic.c
 * Author: Joe Shang
 * Description: The configurations of NVIC module.
 */
 
#include "nvic.h"
#include "stm32f10x.h"
#include "command.h"
#include "holder.h"
#include "misc.h"
			   
void NVIC_Config(void)
{	
	NVIC_InitTypeDef NVIC_InitStruct;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);

	/* usart1 interrupt setting */
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);

	/* holder step timer interrupt setting */
	NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}

void USART1_IRQHandler(void)
{
	char recv = USART_ReceiveData(USART1);

	command_len = construct_command(recv, command);	
}

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
		TIM_ClearITPendingBit(TIM2 , TIM_IT_Update);

		Holder_ChangePulse();
	}
}
