/**
  * @author: hxdyxd@gmail.com
  * @date: 2018 03 24
  */

#include "stm32f10x.h"
#include "soft_timer.h"
#include "app_debug.h"
#include "type.h"
#include "control.h"
#include "mpu6050.h"

//hal
#include "data_interface_hal.h"


void led_proc(void)
{
    led_rev(LED_G);
    if(Pitch > 20 || -Pitch > 20) {
        led_off(LED_BELL);
    } else {
        led_on(LED_BELL);
    }
}


void key_press_proc(int8_t id)
{
    led_rev(LED_R);
    led_rev(LED_BELL);
}


int main(void)
{
    data_interface_hal_init();
    PRINTF("\r\n\r\n Build , %s %s \r\n", __DATE__, __TIME__);
    
    soft_timer_init();
    
    soft_timer_create(SOFT_TIMER_LED, 1, 1, led_proc, 100);
    soft_timer_create(SOFT_TIMER_MPU6050, 1, 1, MPU6050_Pose, 5);
    soft_timer_create(SOFT_TIMER_MPU6050_SHOW, 1, 1, MPU6050_Show, 200);
    soft_timer_create(SOFT_TIMER_CONTROL_ANGLE, 1, 1, control_angle_proc, 10);
    
    led_off(LED_BELL);
    MPU6050_Init();
    led_on(LED_BELL);
    
    APP_DEBUG("init success \r\n");
    motor_set(0, 0);
    
    while(1)
    {
        soft_timer_proc();   //timer
        keys_proc(key_press_proc);  //key
    }
    
}

/******************* (C) COPYRIGHT 2018 hxdyxd *****END OF FILE****/
