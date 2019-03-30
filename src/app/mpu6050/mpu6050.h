/**********************************************************************
Copyright (C) Miaowlabs technology Co., Ltd. 2017. All rights reserved.
Website£∫	http://www.miaowlabs.com
File name:  MPU6050.c
Author:     ﬂ˜Œÿ µ—È “
Version:    3.00
Date:       2017.03.01
Brief: 		


***********************************************************************/
#ifndef __MPU6050_H__
#define __MPU6050_H__

#define q30  1073741824.0f

extern float Pitch, Roll, Yaw;
extern short gyro[3], accel[3];

void MPU6050_Init(void);
void MPU6050_Pose(void);

#endif
