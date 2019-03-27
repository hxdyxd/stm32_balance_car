/* Systick 2018 06 27 END */
/* By hxdyxd */

#include "systick.h"

uint64_t clk_count = 0;

void systick_init(void)
{
	SystemCoreClockUpdate();
	if( SysTick_Config( SystemCoreClock / 1000 ) )
    { 
        /* Capture error */ 
        while (1);
    }
}

void SysTick_Handler(void)
{
	/* System Tick Counter */
    clk_count++;
}
