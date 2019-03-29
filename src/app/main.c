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

uint8_t g_led_red_flag = 0;

void led_proc(void)
{
    led_rev(LED_G);
    if(Pitch > 30 || -Pitch > 30) {
        led_off(LED_BELL);
        g_led_red_flag = 1;
        soft_timer_create(SOFT_TIMER_CONTROL_ANGLE, 0, 1, control_motor_proc, 10);
        motor_set(0, 0);
    }
    if(g_led_red_flag) {
        led_rev(LED_R);
    } else {
        led_off(LED_R);
    }
}


void key_press_proc(int8_t id)
{
    if(g_led_red_flag) {
        g_led_red_flag = 0;
        soft_timer_create(SOFT_TIMER_CONTROL_ANGLE, 1, 1, control_motor_proc, 10);
        led_on(LED_BELL);
    }
}

void sensors_proc(void)
{
    MPU6050_Pose();
    encoder_get_pulse();
}

void sensors_show_proc(void)
{
    APP_DEBUG("mpu6050: %.3f %.3f %.3f \r\n", Pitch, Roll, Yaw);
    APP_DEBUG("encoder: %d %d \r\n", g_s32LeftMotorPulseSigma, g_s32RightMotorPulseSigma);
}


void speed_proc(void)
{
    static float speed = 30;
    control_speed_set(speed);
    speed = -speed;
}


int main(void)
{
    data_interface_hal_init();
    PRINTF("\r\n\r\n Build , %s %s \r\n", __DATE__, __TIME__);
    
    soft_timer_init();
    
    soft_timer_create(SOFT_TIMER_LED, 1, 1, led_proc, 100);
    soft_timer_create(SOFT_TIMER_MPU6050, 1, 1, sensors_proc, 5);
    soft_timer_create(SOFT_TIMER_MPU6050_SHOW, 1, 1, sensors_show_proc, 200);
    soft_timer_create(SOFT_TIMER_CONTROL_ANGLE, 1, 1, control_motor_proc, 5);
    soft_timer_create(SOFT_TIMER_SPEED_SET, 1, 1, speed_proc, 3000);
    
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
