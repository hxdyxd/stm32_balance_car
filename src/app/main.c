/* Proj 2019 */
/* By hxdyxd */

#include "stm32f10x.h"
#include "soft_timer.h"
#include "app_debug.h"
#include "type.h"

//hal
#include "data_interface_hal.h"


void led_proc(void)
{
    led_rev(LED_G);
    APP_DEBUG("rev leds \r\n");
}


void key_press_proc(int8_t id)
{
    led_rev(LED_R);
    led_rev(LED_BELL);
    APP_DEBUG("key press \r\n");
}


int main(void)
{
    data_interface_hal_init();
    soft_timer_init();
    
    soft_timer_create(SOFT_TIMER_LED, 1, 1, led_proc, 100);
    
    APP_DEBUG("init success \r\n");
    
    while(1)
    {
        soft_timer_proc();   //timer
        keys_proc(key_press_proc);  //key
    }
    
}

/******************* (C) COPYRIGHT 2018 hxdyxd *****END OF FILE****/
