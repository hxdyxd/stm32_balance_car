/**
  * @author: hxdyxd@gmail.com
  * @date: 2019 03 26
  */

#include "control.h"
#include "data_interface_hal.h"
#include "mpu6050.h"

#include "app_debug.h"


#define MOTOR_OUT_DEAD_VAL   (100)

#define CAR_POSITION_MAX ( 7200)
#define CAR_POSITION_MIN (-7200)

/* pid parameter start */
float gf_dir_kp = -0.5;

/* PD algorithm, angle */
float gf_ang_kp = -936;
float gf_ang_kd = -0.79416;

/* PI algorithm, speed */
float gf_spd_kp = -145.96;
float gf_spd_ki = -145.96/100;
/* pid parameter end */

static float gf_speed_set = 0;
static float gf_direction_set = 0;
static float fCarPosition = 0;

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
float get_direction_out(float dir)
{
    float kp = gf_dir_kp;

    return (gyro[2] - dir) * kp;
}


/**
  * @brief  Angle control
  * @retval None
  * @date   2019 03 27
  */
float get_angle_out(float deg)
{
    float kp = gf_ang_kp;
    float kd = gf_ang_kd;

    return (Pitch - -3 - deg) * kp + gyro[1] * kd;
}


/**
  * @brief  Speed control
  * @retval None
  * @date   2019 03 28
  */
float get_speed_out(float set_speed)
{
    float kp = gf_spd_kp;
    float ki = gf_spd_ki;
    
    static float fCarSpeedOld = 0;
    float fCarSpeed = (g_s32LeftMotorPulseSigma  + g_s32RightMotorPulseSigma) * 0.5;
    g_s32LeftMotorPulseSigma = g_s32RightMotorPulseSigma = 0;     //全局变量 注意及时清零
    
    //阶低通滤波器
    fCarSpeed = 0.8 * fCarSpeedOld + 0.2 * fCarSpeed ;
    fCarSpeedOld = fCarSpeed;
    //前正后负
    
    fCarPosition += fCarSpeed * ki;
    fCarPosition += set_speed;
    
    //积分上限设限
    if(fCarPosition > CAR_POSITION_MAX)    fCarPosition = CAR_POSITION_MAX;
    if(fCarPosition < CAR_POSITION_MIN)    fCarPosition = CAR_POSITION_MIN;
    
    return (fCarSpeed /*+ set_speed*/) * kp + fCarPosition;
}


/**
  * @brief  motor control
  * @retval None
  * @date   2019 03 27
  */
void control_motor_proc(void)
{
    static float speed_out = 0;
    static float direct_out = 0;
    static uint32_t count = 0;
    static float cur_speed_set = 0;
    if(gf_speed_set - cur_speed_set > 0) {
        cur_speed_set += 20;
    } else if(gf_speed_set - cur_speed_set < 0) {
        cur_speed_set += -20;
    }
    
    if(count++ % 5 == 0) {
        speed_out = get_speed_out(cur_speed_set);
        direct_out = get_direction_out(gf_direction_set);
    }

    float angle_out = get_angle_out(0);
    float out =  angle_out + speed_out;
    
    motor_set(out -direct_out, out +direct_out);
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


/**
  * @brief  motor direction control
  * @retval None
  * @date   2019 03 29
  */
void control_direction_set(float dir_set)
{
    gf_direction_set = dir_set;
}


/**
  * @brief  motor reset
  * @retval None
  * @date   2019 03 30
  */
void control_reset(void)
{
    fCarPosition = 0;
    g_s32LeftMotorPulseSigma = g_s32RightMotorPulseSigma = 0;     //全局变量 注意及时清零
}

/******************* (C) COPYRIGHT 2019 hxdyxd *****END OF FILE****/
