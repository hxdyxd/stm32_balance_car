/**
  * @author: hxdyxd@gmail.com
  * @date: 2019 03 28
  */

#include "encoder_timer.h"

int g_s32LeftMotorPulseSigma = 0;
int g_s32RightMotorPulseSigma = 0;

/**
  * @brief 正交解码初始化(电机测速)
  */
void TIM2_Encoder_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;

    /* Encoder unit connected to TIM2, 4X mode */    
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* TIM2 clock source enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    /* Enable GPIOA, clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_StructInit(&GPIO_InitStructure);
    /* Configure PA.00,01 as encoder input */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Enable the TIM3 Update Interrupt */

    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Timer configuration in Encoder mode */
    TIM_DeInit(TIM2);
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

    TIM_TimeBaseStructure.TIM_Prescaler = 0x0;  // No prescaling 
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;  
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;  //设置时钟分频系数：不分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数模式 
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    //TIM_ICPolarity_Rising上升沿捕获
    TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12, 
                             TIM_ICPolarity_Rising, 
                             TIM_ICPolarity_Rising); 
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 6; //无滤波器
    TIM_ICInit(TIM2, &TIM_ICInitStructure);

    // Clear all pending interrupts
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);    //使能中断

    TIM_Cmd(TIM2, ENABLE);       //使能定时器3
}


/**
  * @brief 正交解码初始化(电机测速)
  */
void TIM4_Encoder_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;

    /* Encoder unit connected to TIM3, 4X mode */    
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* TIM3 clock source enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    /* Enable GPIOA, clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_StructInit(&GPIO_InitStructure);
    /* Configure PA.06,07 as encoder input */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* Enable the TIM3 Update Interrupt */

    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Timer configuration in Encoder mode */
    TIM_DeInit(TIM4);
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

    TIM_TimeBaseStructure.TIM_Prescaler = 0x0;  // TIM4时钟预分频值
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;  //设定计时器重装值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;  //设置时钟分频系数：不分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数模式 
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    //TIM_ICPolarity_Rising上升沿捕获
    TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, 
                             TIM_ICPolarity_Rising, 
                             TIM_ICPolarity_Rising); 
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 6; //无滤波器
    TIM_ICInit(TIM4, &TIM_ICInitStructure);

    // Clear all pending interrupts
    TIM_ClearFlag(TIM4, TIM_FLAG_Update);  //清除TIM4的更新标志位
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);    //开启TIM4的更新标志位

    TIM_Cmd(TIM4, ENABLE);       //使能定时器3
}

/**
  * @brief 采集电机速度脉冲
  */
void encoder_get_pulse(void)
{
    short g_s16LeftMotorPulse = TIM_GetCounter(TIM2);
    short g_s16RightMotorPulse= -TIM_GetCounter(TIM4);
    TIM2->CNT = 0;
    TIM4->CNT = 0;   //清零
    g_s32LeftMotorPulseSigma +=  g_s16LeftMotorPulse;
    g_s32RightMotorPulseSigma += g_s16RightMotorPulse;
}


void TIM2_IRQHandler(void)
{
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
}

void TIM4_IRQHandler(void)
{
    TIM_ClearFlag(TIM4, TIM_FLAG_Update);
}




/******************* (C) COPYRIGHT 2019 03 28 hxdyxd *****END OF FILE****/
