/* Systick 2018 06 27 END */
/* By hxdyxd */

#ifndef __SYSSTICK_H__
#define __SYSSTICK_H__

#include <stm32f10x.h>

//#define TickCounter (TickCounterUs / 1000)

extern uint64_t clk_count;
void systick_init(void);

#endif
