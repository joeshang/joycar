/*
 * File: motor.c
 * Author: Joe Shang
 * Description: The driver of Motor.
 */
 
#include "motor.h"
#include "stm32f10x.h"

enum
{
    MOTOR_LEFT,
    MOTOR_RIGHT,
    MOTOR_CNT
};

enum
{
    MOTOR_STATE_FORWARD,
    MOTOR_STATE_BACKWARD,
    MOTOR_STATE_TURNLEFT,
    MOTOR_STATE_TURNRIGHT,
    MOTOR_STATE_STOP
};

static int motor_state;
static int motor_left_run_pulse;
static int motor_right_run_pulse;
static int motor_run_speed[MOTOR_SPEED_LEVEL_MAX + 1][MOTOR_CNT] = 
{
    {0, 0},     /* Speed Level 0 */
    {0, 0},     /* Speed Level 1 */
    {0, 0},     /* Speed Level 2 */
    {0, 0}      /* Speed Level 3 */
};
 
static void Motor_GPIO_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* motor gpio clock setting */
    RCC_APB1PeriphClockCmd(RCC_APB_MOTOR_LEFT_FW | RCC_APB_MOTOR_LEFT_INV |
            RCC_APB_MOTOR_RIGHT_FW | RCC_APB_MOTOR_RIGHT_INV, ENABLE);

    /* left motor forward gpio */
    GPIO_InitStructure.GPIO_Pin = MOTOR_LEFT_FW_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(MOTOR_LEFT_FW_GPIO, &GPIO_InitStructure);

    /* left motor inverse gpio */
    GPIO_InitStructure.GPIO_Pin = MOTOR_LEFT_INV_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(MOTOR_LEFT_INV_GPIO, &GPIO_InitStructure);

    /* right motor forward gpio */
    GPIO_InitStructure.GPIO_Pin = MOTOR_RIGHT_FW_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(MOTOR_RIGHT_FW_GPIO, &GPIO_InitStructure);

    /* right motor inverse gpio */
    GPIO_InitStructure.GPIO_Pin = MOTOR_RIGHT_INV_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(MOTOR_RIGHT_INV_GPIO, &GPIO_InitStructure);
}

static void Motor_PWM_Init()
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    /* motor pwm clock setting */ 
    RCC_APB1PeriphClockCmd(RCC_APB_MOTOR_TIM, ENABLE);

    /* time base -> all pwm's prescaler + period*/
    TIM_TimeBaseStructure.TIM_Period = MOTOR_PERIOD;
    TIM_TimeBaseStructure.TIM_Prescaler = MOTOR_PRESCALER;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(MOTOR_TIM, &TIM_TimeBaseStructure);
    TIM_ARRPreloadConfig(MOTOR_TIM, ENABLE);

    /* output channel -> every pwm's pulse */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_Pulse = 0;

    /* channel 1 -> motor left forward */
    TIM_OC1Init(MOTOR_TIM, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(MOTOR_TIM, TIM_OCPreload_Enable);

    /* channel 2 -> motor left inverse */
    TIM_OC2Init(MOTOR_TIM, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(MOTOR_TIM, TIM_OCPreload_Enable);

    /* channel 3 -> motor right forward */
    TIM_OC3Init(MOTOR_TIM, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(MOTOR_TIM, TIM_OCPreload_Enable);

    /* channel 4 -> motor right inverse */
    TIM_OC4Init(MOTOR_TIM, &TIM_OCInitStructure);
    TIM_OC4PreloadConfig(MOTOR_TIM, TIM_OCPreload_Enable);

    /* enable motor's pwm module */
    TIM_Cmd(MOTOR_TIM, ENABLE);
}

void Motor_Init()
{
    Motor_GPIO_Init();
    Motor_PWM_Init();

    motor_state = MOTOR_STATE_STOP;
    motor_left_run_pulse = motor_run_speed[MOTOR_SPEEP_LEVEL_INIT][MOTOR_LEFT];
    motor_right_run_pulse = motor_run_speed[MOTOR_SPEED_LEVEL_INIT][MOTOR_RIGHT];
}

void Motor_Forward()
{
    motor_state = MOTOR_STATE_FORWARD;

    /* left motor */
    TIM_SetCompare1(MOTOR_TIM, motor_left_run_pulse);
    TIM_SetCompare2(MOTOR_TIM, 0);

    /* right motor */
    TIM_SetCompare3(MOTOR_TIM, motor_right_run_pulse);
    TIM_SetCompare4(MOTOR_TIM, 0);
}

void Motor_Backward()
{
    motor_state = MOTOR_STATE_BACKWARD;

    /* left motor */
    TIM_SetCompare1(MOTOR_TIM, 0);
    TIM_SetCompare2(MOTOR_TIM, motor_left_run_pulse);

    /* right motor */
    TIM_SetCompare3(MOTOR_TIM, 0);
    TIM_SetCompare4(MOTOR_TIM, motor_right_run_pulse);
}

void Motor_TurnLeft()
{
    motor_state = MOTOR_STATE_TURNLEFT;

    /* left motor */
    TIM_SetCompare1(MOTOR_TIM, MOTOR_LEFT_TURN_PULSE);
    TIM_SetCompare2(MOTOR_TIM, 0);

    /* right motor */
    TIM_SetCompare3(MOTOR_TIM, 0);
    TIM_SetCompare4(MOTOR_TIM, MOTOR_RIGHT_TURN_PULSE);
}

void Motor_TurnRight()
{
    motor_state = MOTOR_STATE_TURNRIGHT;

    /* left motor */
    TIM_SetCompare1(MOTOR_TIM, 0);
    TIM_SetCompare2(MOTOR_TIM, MOTOR_LEFT_TURN_PULSE);

    /* right motor */
    TIM_SetCompare3(MOTOR_TIM, MOTOR_RIGHT_TURN_PULSE);
    TIM_SetCompare4(MOTOR_TIM, 0);
}

void Motor_Stop()
{
    motor_state = MOTOR_STATE_STOP;

    /* left motor */
    TIM_SetCompare1(MOTOR_TIM, 0);
    TIM_SetCompare2(MOTOR_TIM, 0);

    /* right motor */
    TIM_SetCompare3(MOTOR_TIM, 0);
    TIM_SetCompare4(MOTOR_TIM, 0);
}

void Motor_SetSpeed(char *speed)
{
    int speed_level;

    speed_level = atoi(speed);
    if (speed_level < 0 || speed_level > MOTOR_SPEEP_LEVEL_MAX)
    {
        return;
    }
        
    motor_left_run_pulse = motor_run_speed[speed_level][MOTOR_LEFT];
    motor_right_run_pulse = motor_run_speed[speed_level][MOTOR_RIGHT];

    switch (motor_state)
    {
        case MOTOR_STATE_FORWARD:
            Motor_Forward();
            break;
        case MOTOR_STATE_BACKWARD:
            Motor_Backward();
            break;
        default:
            break;
    }
}
