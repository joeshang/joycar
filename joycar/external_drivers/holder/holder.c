/*
 * File: holder.c
 * Author: Joe Shang
 * Description: The driver of Holder.
 */
 
#include "holder.h"
#include "stm32f10x.h"

enum
{
    HOLDER_STATE_TURNUP,
    HOLDER_STATE_TURNDOWN,
    HOLDER_STATE_TURNLEFT,
    HOLDER_STATE_TURNRIGHT,
    HOLDER_STATE_STOP
};

static int holder_state;
static int holder_v_pulse;
static int holder_h_pulse;
static int holder_pulse_step;

static int holder_speed[HOLDER_SPEED_LEVEL_MAX + 1] =
{
    0,   /* Speed Level */
    0,   /* Speed Level */
    0,   /* Speed Level */
    0,   /* Speed Level */
    0    /* Speed Level */
};

static void Holder_GPIO_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB_HOLDER_V | RCC_APB_HOLDER_H, ENABLE);

    GPIO_InitStructure.GPIO_Pin = HOLDER_V_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(HOLDER_V_GPIO, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = HOLDER_H_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(HOLDER_H_GPIO, &GPIO_InitStructure);
}

static void Holder_PWM_Init()
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    /* time base --> all pwm's period and prescaler */
    TIM_TimeBaseStructure.TIM_Period = HOLDER_PERIOD;
    TIM_TimeBaseStructure.TIM_Prescaler = HOLDER_PRESCALER;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(HOLDER_TIM, &TIM_TimeBaseStructure);

    /* output channel --> every pwm's pulse */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

    /* channel 1 --> holder vertical */
    TIM_OCInitStructure.TIM_Pulse = HOLDER_V_PULSE_INIT;
    TIM_OC1Init(HOLDER_TIM, &TIM_OCInitStructure);
    TIM_OC1PolarityConfig(HOLDER_TIM, TIM_OCPreload_Enable);

    /* channel 2 --> holder horizontal */
    TIM_OCInitStructure.TIM_Pulse = HOLDER_H_PULSE_INIT;
    TIM_OC2Init(HOLDER_TIM, &TIM_OCInitStructure);
    TIM_OC2PolarityConfig(HOLDER_TIM, TIM_OCPreload_Enable);

    /* enable holder's pwm module */
    TIM_Cmd(HOLDER_TIM, ENABLE);
}

static void Holder_StepTimer_Init()
{
}
 
void Holder_Init()
{
    Holder_GPIO_Init();
    Holder_PWM_Init();

    holder_state = HOLDER_STATE_STOP;
    holder_v_pulse = HOLDER_V_PULSE_INIT;
    holder_h_pulse = HOLDER_H_PULSE_INIT;
    holder_pulse_step = holder_speed[HOLDER_SPEED_LEVEL_INIT];
}

void Holder_TurnUp()
{
    holder_state = HOLDER_STATE_TURNUP;
}

void Holder_TurnDown()
{
    holder_state = HOLDER_STATE_TURNDOWN;
}

void Holder_TurnLeft()
{
    holder_state = HOLDER_STATE_TURNLEFT;
}

void Holder_TurnRight()
{
    holder_state = HOLDER_STATE_TURNRIGHT;
}

void Holder_Stop()
{
    holder_state = HOLDER_STATE_STOP;	

    /* stop step timer */
}

void Holder_SetSpeed(char *speed)
{
    int speed_level;

    speed_level = atoi(speed);
    if (speed_level < 0 || speed_level > HOLDER_SPEED_LEVEL_MAX)
    {
        return;
    }
}

void Holder_ChangePulse()
{
    switch (holder_state)
    {
        case HOLDER_STATE_TURNUP:
            if (holder_v_pulse + holder_pulse_step > HOLDER_V_PULSE_MAX)
            {
                holder_v_pulse = HOLDER_V_PULSE_MAX;
            }
            else
            {
                holder_v_pulse += holder_pulse_step;
            }
            break;
        case HOLDER_STATE_TURNDOWN:
            if (holder_v_pulse - holder_pulse_step < HOLDER_V_PULSE_MIN)
            {
                holder_v_pulse = HOLDER_V_PULSE_MIN;
            }
            else
            {
                holder_v_pulse -= holder_pulse_step;
            }
            break;
        case HOLDER_STATE_TURNLEFT:
            if (holder_h_pulse + holder_pulse_step > HOLDER_H_PULSE_MAX)
            {
                holder_h_pulse = HOLDER_H_PULSE_MAX;
            }
            else
            {
                holder_h_pulse += holder_pulse_step;
            }
            break;
        case HOLDER_STATE_TURNRIGHT:
            if (holder_h_pulse - holder_pulse_step < HOLDER_H_PULSE_MIN)
            {
                holder_h_pulse = HOLDER_H_PULSE_MIN;
            }
            else
            {
                holder_h_pulse -= holder_pulse_step;
            }
            break;
        default:
            break;
    }
}
