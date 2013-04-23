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
#define HOLDER_TIM
#define RCC_APB_HOLDER_TIM

#define RCC_APB_HOLDER_H
#define HOLDER_H_GPIO
#define HOLDER_H_Pin

#define RCC_APB_HOLDER_V
#define HOLDER_V_GPIO
#define HOLDER_V_Pin

/* Holder PWM Setting */
#define HOLDER_PERIOD
#define HOLDER_PRESCALER

#define HOLDER_SPEED_LEVEL_INIT
#define HOLDER_SPEED_LEVEL_MAX

#define HOLDER_V_PULSE_INIT
#define HOLDER_V_PULSE_MIN
#define HOLDER_V_PULSE_MAX

#define HOLDER_H_PULSE_INIT
#define HOLDER_H_PULSE_MIN
#define HOLDER_H_PULSE_MAX

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

#endif
