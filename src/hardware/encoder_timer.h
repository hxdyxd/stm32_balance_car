/*
 * hxdyxd@gmail.com, 2019
 */

#ifndef __ENCODER_TIMER_H
#define __ENCODER_TIMER_H

#include "stm32f10x.h"

extern int g_s32LeftMotorPulseSigma;
extern int g_s32RightMotorPulseSigma;

/**
  * @brief 正交解码初始化(电机测速)
  */
void TIM2_Encoder_Init(void);

/**
  * @brief 正交解码初始化(电机测速)
  */
void TIM4_Encoder_Init(void);

/**
  * @brief 采集电机速度脉冲
  */
void encoder_get_pulse(void);

#endif
/******************* (C) COPYRIGHT 2019 03 28 hxdyxd *****END OF FILE****/
