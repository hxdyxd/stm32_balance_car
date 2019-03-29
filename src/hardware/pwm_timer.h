/*
 * hxdyxd@gmail.com, 2019
 */

#ifndef __PWM_TIMER_H
#define __PWM_TIMER_H

#include "stm32f10x.h"

#define PERIOD 7200

/*
 * 4通道PWM输出
 */
void pwm_4ch_timer_init(void);

void pwm_timer_set(uint8_t id, uint16_t ccr);


#endif
/******************* (C) COPYRIGHT 2019 03 25 hxdyxd *****END OF FILE****/
