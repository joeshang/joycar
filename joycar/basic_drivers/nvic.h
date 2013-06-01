/*
 * File: nvic.h
 * Author: Joe Shang
 * Description: The configurations of NVIC module.
 */

#ifndef _NVIC_H_
#define _NVIC_H_

#include "stm32f10x.h"

void NVIC_Config(void);
void TIM2_IRQHanlder(void);

#endif
