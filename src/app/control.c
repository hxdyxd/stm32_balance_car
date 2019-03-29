/**
  * @author: hxdyxd@gmail.com
  * @date: 2019 03 26
  */

#include "control.h"
#include "data_interface_hal.h"
#include "mpu6050.h"

#include "app_debug.h"


#define MOTOR_OUT_DEAD_VAL   (100)

#define CAR_POSITION_MAX 2000
#define CAR_POSITION_MIN -2000

static float gf_speed_set = 0;


/**
  * @brief  配置电机
  * @retval None
  * @date   2019 03 26
  */
void motor_set(int16_t left, int16_t right)
{
    if(left == 0) {
        pwm_timer_set(1, 0);
        pwm_timer_set(0, 0);
    } else if(left < 0) {
        //left positive
        left -= MOTOR_OUT_DEAD_VAL;
        pwm_timer_set(1, 0);
        pwm_timer_set(0, -left);
    } else {
        left += MOTOR_OUT_DEAD_VAL;
        pwm_timer_set(0, 0);
        pwm_timer_set(1, left);
    }
    if(right == 0) {
        pwm_timer_set(2, 0);
        pwm_timer_set(3, 0);
    } else if(right < 0) {
        right -= MOTOR_OUT_DEAD_VAL;
        pwm_timer_set(2, 0);
        pwm_timer_set(3, -right);
    } else {
        right += MOTOR_OUT_DEAD_VAL;
        //right positive
        pwm_timer_set(3, 0);
        pwm_timer_set(2, right);
    }
}


/**
  * @brief  Angle control
  * @retval None
  * @date   2019 03 27
  */
float get_angle_out(float deg)
{
    float kp = -936;
    float kd = -0.79416;

    return (Pitch - -3 - deg) * kp + gyro[1] * kd;
}


/**
  * @brief  Speed control
  * @retval None
  * @date   2019 03 28
  */
float get_speed_out(float set_speed)
{
    float kp = -145.96;
    float ki = kp/100;
    
    
    static float fCarSpeedOld = 0;
    static float fCarPosition = 0;
    float fCarSpeed = (g_s32LeftMotorPulseSigma  + g_s32RightMotorPulseSigma) * 0.5;
    g_s32LeftMotorPulseSigma = g_s32RightMotorPulseSigma = 0;     //全局变量 注意及时清零
    
    //阶低通滤波器
    fCarSpeed = 0.8 * fCarSpeedOld + 0.2 * fCarSpeed ;
    fCarSpeedOld = fCarSpeed;
    //前正后负
    
    fCarPosition += fCarSpeed * ki;
    //fCarPosition -= set_speed;
    
    //积分上限设限
    if(fCarPosition > CAR_POSITION_MAX)    fCarPosition = CAR_POSITION_MAX;
    if(fCarPosition < CAR_POSITION_MIN)    fCarPosition = CAR_POSITION_MIN;
    
    return (fCarSpeed + set_speed) * kp + fCarPosition;
}

/**
  * @brief  motor control
  * @retval None
  * @date   2019 03 27
  */
void control_motor_proc(void)
{
    static float speed_out = 0;
    static uint32_t count = 0;
    static float cur_speed_set = 0;
    if(gf_speed_set - cur_speed_set > 0) {
        cur_speed_set += 1.5;
    } else if(gf_speed_set - cur_speed_set < 0) {
        cur_speed_set += -1.5;
    }
    
    if(count++ % 5 == 0) {
        speed_out = get_speed_out(cur_speed_set);
        //APP_DEBUG("speed_out: %.4f \r\n", speed_out);
    }
//    speed_out = 0;
    float angle_out = get_angle_out(0);
//    angle_out = 0;
    float out =  angle_out + speed_out;
    
    motor_set(out, out);
}

/**
  * @brief  motor speed control
  * @retval None
  * @date   2019 03 29
  */
void control_speed_set(float speed_set)
{
    gf_speed_set = speed_set;
}

/******************* (C) COPYRIGHT 2019 hxdyxd *****END OF FILE****/

