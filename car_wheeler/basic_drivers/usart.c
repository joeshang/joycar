/*
 * File: usart.c
 * Author: Joe Shang
 * Description: The drivers of USART module.
 */
 
#include "usart.h"
#include "stm32f10x.h"
			   
void Usart_Initialization(void)
{	
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

	/* usart1 clock setting */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
	
	/* usart1 gpio setting */
	/* transmit port */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;	
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	/* receive port */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;	
	GPIO_Init(GPIOA,&GPIO_InitStruct);	

	/* usart1 property */
	USART_InitStruct.USART_BaudRate = 9600;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART1,&USART_InitStruct);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	/* usart1 interrupt setting */
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);

	USART_Cmd(USART1, ENABLE);
}

void Usart_SendChar(USART_TypeDef* USARTx, char data)
{
	while (!USART_GetFlagStatus(USARTx, USART_FLAG_TXE));	
	USART_SendData(USARTx, data);
}

char Usart_GetChar(USART_TypeDef* USARTx)
{
	char data;
	while (!USART_GetFlagStatus(USARTx, USART_FLAG_RXNE));
	data = (char)(USART_ReceiveData(USARTx) & 0xff);
	
	return data;		
}

void USART1_IRQHandler(void)
{
	if (USART_GetFlagStatus(USART1, USART_FLAG_TXE))
	{
		Usart_SendChar(USART1, USART_ReceiveData(USART1));	
	}	
}
