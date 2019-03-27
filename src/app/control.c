/**
  * @author: hxdyxd@gmail.com
  * @date: 2018 03 26
  */

#include "control.h"
#include "data_interface_hal.h"
#include "mpu6050.h"


#define MOTOR_OUT_DEAD_VAL   (1000)

/**
  * @brief  配置电机
  * @retval None
  */
void motor_set(int16_t left, int16_t right)
{
    if(left < 0) {
        //left positive
        pwm_timer_set(1, 0);
        if(-left < MOTOR_OUT_DEAD_VAL)
            //dead stop
            pwm_timer_set(0, 0);
        else
            pwm_timer_set(0, -left);
    } else {
        pwm_timer_set(0, 0);
        if(left < MOTOR_OUT_DEAD_VAL)
            //dead stop
            pwm_timer_set(1, 0);
        else
            pwm_timer_set(1, left);
    }
    if(right < 0) {
        pwm_timer_set(2, 0);
        if(-right < MOTOR_OUT_DEAD_VAL)
            //dead stop
            pwm_timer_set(3, 0);
        else
            pwm_timer_set(3, -right);
    } else {
        //right positive
        pwm_timer_set(3, 0);
        if(right < MOTOR_OUT_DEAD_VAL)
            //dead stop
            pwm_timer_set(2, 0);
        else
            pwm_timer_set(2, right);
    }
}


void control_angle_proc(void)
{
    float *angle = mpu6050_angle();
    
}



/******************* (C) COPYRIGHT 2019 hxdyxd *****END OF FILE****/

