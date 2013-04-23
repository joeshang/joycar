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
#define MOTOR_TIM
#define RCC_APB_MOTOR_TIM

#define RCC_APB_MOTOR_LEFT_FW
#define MOTOR_LEFT_FW_GPIO
#define MOTOR_LEFT_FW_Pin

#define RCC_APB_MOTOR_LEFT_INV
#define MOTOR_LEFT_INV_GPIO
#define MOTOR_LEFT_INV_Pin

#define RCC_APB_MOTOR_RIGHT_FW
#define MOTOR_RIGHT_FW_GPIO
#define MOTOR_RIGHT_FW_Pin

#define RCC_APB_MOTOR_RIGHT_INV
#define MOTOR_RIGHT_INV_GPIO
#define MOTOR_RIGHT_INV_Pin

/* Motor PWM Setting */
#define MOTOR_PERIOD
#define MOTOR_PRESCALER

#define MOTOR_SPEED_LEVEL_INIT
#define MOTOR_SPEED_LEVEL_MAX

#define MOTOR_LEFT_TURN_PULSE
#define MOTOR_RIGHT_TURN_PULSE

/************************************************************
 * Motor Interface
 ************************************************************/
void Motor_Init(void);

void Motor_Forward(void);
void Motor_Backward(void);
void Motor_TurnLeft(void);
void Motor_TurnRight(void);
void Motor_Stop(void);
void Motor_SetSpeed(char *speed);

#endif
