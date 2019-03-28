/**
  * @author: hxdyxd@gmail.com
  * @date: 2019 03 26
  */

#include "control.h"
#include "data_interface_hal.h"
#include "mpu6050.h"


#define MOTOR_OUT_DEAD_VAL   (1000)

/**
  * @brief  配置电机
  * @retval None
  * @date   2019 03 26
  */
void motor_set(int16_t left, int16_t right)
{
    if(left < 0) {
        //left positive
        left -= MOTOR_OUT_DEAD_VAL;
        pwm_timer_set(1, 0);
        pwm_timer_set(0, -left);
//        if(-left < MOTOR_OUT_DEAD_VAL)
//            //dead stop
//            pwm_timer_set(0, 0);
//        else
//            pwm_timer_set(0, -left);
    } else {
        left += MOTOR_OUT_DEAD_VAL;
        pwm_timer_set(0, 0);
        pwm_timer_set(1, left);
//        if(left < MOTOR_OUT_DEAD_VAL)
//            //dead stop
//            pwm_timer_set(1, 0);
//        else
//            pwm_timer_set(1, left);
    }
    if(right < 0) {
        right -= MOTOR_OUT_DEAD_VAL;
        pwm_timer_set(2, 0);
        pwm_timer_set(3, -right);
//        if(-right < MOTOR_OUT_DEAD_VAL)
//            //dead stop
//            pwm_timer_set(3, 0);
//        else
//            pwm_timer_set(3, -right);
    } else {
        right += MOTOR_OUT_DEAD_VAL;
        //right positive
        pwm_timer_set(3, 0);
        pwm_timer_set(2, right);
//        if(right < MOTOR_OUT_DEAD_VAL)
//            //dead stop
//            pwm_timer_set(2, 0);
//        else
//            pwm_timer_set(2, right);
    }
}


float g_fAngleControlOut = 0;

/**
  * @brief  Angle control
  * @retval None
  * @date   2019 03 27
  */
void control_angle_proc(void)
{
    float kp = -150;
    kp = -100;
    float kd = 0;

    g_fAngleControlOut = (Pitch - 2) * kp + gyro[1] * kd;
    motor_set( (float)g_fAngleControlOut, (float)g_fAngleControlOut);
}



/******************* (C) COPYRIGHT 2019 hxdyxd *****END OF FILE****/

