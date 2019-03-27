#ifndef __PWM_TIMER_H
#define __PWM_TIMER_H

#include "stm32f10x.h"

#define PERIOD 7200
#define CCR1_V(p) (1200-p)
#define CCR2_V(p) (2400-p)

/*
 * 4Í¨µÀPWMÊä³ö
 */
void pwm_4ch_timer_init(void);

void pwm_timer_set(uint8_t id, uint16_t ccr);


#endif
/******************* (C) COPYRIGHT 2019 03 25 hxdyxd *****END OF FILE****/
