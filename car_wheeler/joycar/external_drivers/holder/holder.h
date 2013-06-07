/*
 * File: holder.h
 * Author: Joe Shang
 * Description: The driver of Holder.
 */

#ifndef _HOLDER_H_
#define _HOLDER_H_

#include "stm32f10x.h"

/************************************************************
 * Holder Configuration
 ************************************************************/
/* Holder GPIO Setting */
#define HOLDER_TIM				TIM4
#define RCC_APB_HOLDER_TIM		RCC_APB1Periph_TIM4

#define HOLDER_STEP_TIM			TIM2
#define RCC_APB_HOLDER_STEP_TIM	RCC_APB1Periph_TIM2

#define RCC_APB_HOLDER_H		RCC_APB2Periph_GPIOB
#define HOLDER_H_GPIO			GPIOB
#define HOLDER_H_Pin			GPIO_Pin_6

#define RCC_APB_HOLDER_V		RCC_APB2Periph_GPIOB
#define HOLDER_V_GPIO			GPIOB
#define HOLDER_V_Pin			GPIO_Pin_7

/* Holder PWM Setting */
#define HOLDER_PERIOD			10000
#define HOLDER_PRESCALER		72

#define HOLDER_STEP_PERIOD		2500
#define HOLDER_STEP_PRESCALER	720

#define HOLDER_SPEED_LEVEL_INIT	0
#define HOLDER_SPEED_LEVEL_MAX	4

#define HOLDER_V_PULSE_INIT		1050
#define HOLDER_V_PULSE_MIN		1050
#define HOLDER_V_PULSE_MAX		4000

#define HOLDER_H_PULSE_INIT		2000
#define HOLDER_H_PULSE_MIN		600
#define HOLDER_H_PULSE_MAX		4000

/************************************************************
 * Holder Interface
 ************************************************************/
void Holder_Init(void);

void Holder_TurnUp(void);
void Holder_TurnDown(void);
void Holder_TurnLeft(void);
void Holder_TurnRight(void);
void Holder_Stop(void);
void Holder_SetSpeed(char *speed);
void Holder_ChangePulse(void);

#endif
