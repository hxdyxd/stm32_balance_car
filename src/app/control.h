/**
  * @author: hxdyxd@gmail.com
  * @date: 2018 03 26
  */

#ifndef __CONTROL_H__
#define __CONTROL_H__

#include <stdint.h>



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

#endif
/******************* (C) COPYRIGHT 2019 hxdyxd *****END OF FILE****/
