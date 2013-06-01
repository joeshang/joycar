/*
 * File: motor.h
 * Author: Joe Shang
 * Description: The driver of Motor.
 */

#ifndef _MOTOR_H_
#define _MOTOR_H_

#include "stm32f10x.h"

/************************************************************
 * Motor Configuration
 ************************************************************/
/* Motor GPIO Setting */
#define MOTOR_TIM					TIM3
#define RCC_APB_MOTOR_TIM			RCC_APB1Periph_TIM3    //TIM ±÷” πƒ‹	

#define RCC_APB_MOTOR_LEFT_FW  		RCC_APB2Periph_GPIOC
#define MOTOR_LEFT_FW_GPIO			GPIOC
#define MOTOR_LEFT_FW_Pin			GPIO_Pin_6

#define RCC_APB_MOTOR_LEFT_INV		RCC_APB2Periph_GPIOC
#define MOTOR_LEFT_INV_GPIO			GPIOC
#define MOTOR_LEFT_INV_Pin			GPIO_Pin_7

#define RCC_APB_MOTOR_RIGHT_FW		RCC_APB2Periph_GPIOC
#define MOTOR_RIGHT_FW_GPIO			GPIOC
#define MOTOR_RIGHT_FW_Pin			GPIO_Pin_8

#define RCC_APB_MOTOR_RIGHT_INV		RCC_APB2Periph_GPIOC
#define MOTOR_RIGHT_INV_GPIO		GPIOC
#define MOTOR_RIGHT_INV_Pin			GPIO_Pin_9

/* Motor PWM Setting */
#define MOTOR_PERIOD				7200
#define MOTOR_PRESCALER				1

#define MOTOR_SPEED_LEVEL_INIT		2
#define MOTOR_SPEED_LEVEL_MAX		3

#define MOTOR_LEFT_TURN_PULSE		2500
#define MOTOR_RIGHT_TURN_PULSE		2500

/************************************************************
 * Motor Interface
 ************************************************************/
extern void Motor_Init(void);

void Motor_Forward(void);
void Motor_Backward(void);
void Motor_TurnLeft(void);
void Motor_TurnRight(void);
void Motor_Stop(void);
void Motor_SetSpeed(char *speed);

#endif
