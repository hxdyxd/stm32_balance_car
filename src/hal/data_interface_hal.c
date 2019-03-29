/* 
 *
 *2018 09 21 & hxdyxd
 *
 */

#include "data_interface_hal.h"
#include "stm32f10x.h"

//TICKET
#include "systick.h"

//GPIO
#include "leds.h"
#include "keys.h"

//uart
#include "interface_usart.h"

//pwm
#include "pwm_timer.h"

//i2c
#include "i2c.h"

//encoder
#include "encoder_timer.h"


/* some low level platform function */
/* public hal function */


void data_interface_hal_init(void)
{
    //CLOCK INTERFACE
    systick_init();
    //GPIO INTERFACE
    leds_init();
    keys_init();
    //uart
    interface_usart_init();
    //pwm
    pwm_4ch_timer_init();
    i2c_init();
    TIM2_Encoder_Init();
    TIM4_Encoder_Init();
}


uint64_t hal_read_TickCounter(void)
{
    return clk_count;
}


/******************* (C) COPYRIGHT 2018 hxdyxd *****END OF FILE****/
