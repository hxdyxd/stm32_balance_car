/**
  * @author: hxdyxd@gmail.com
  * @date: 2018 03 26
  */

#ifndef __CONTROL_H__
#define __CONTROL_H__

#include <stdint.h>

/********** pid parameter start ************/
extern float gf_dir_kp;

/* PD algorithm, angle */
extern float gf_ang_kp;
extern float gf_ang_kd;

/* PI algorithm, speed */
extern float gf_spd_kp;
extern float gf_spd_ki;
/************ pid parameter end ************/

void motor_set(int16_t left, int16_t right);

/**
  * @brief  motor control
  * @retval None
  * @date   2019 03 27
  */
void control_motor_proc(void);

/**
  * @brief  motor speed control
  * @retval None
  * @date   2019 03 29
  */
void control_speed_set(float speed_set);

/**
  * @brief  motor direction control
  * @retval None
  * @date   2019 03 29
  */
void control_direction_set(float dir_set);

/**
  * @brief  motor reset
  * @retval None
  * @date   2019 03 30
  */
void control_reset(void);

#endif
/******************* (C) COPYRIGHT 2019 hxdyxd *****END OF FILE****/
