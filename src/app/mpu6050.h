/**
  * @author: hxdyxd@gmail.com
  * @date: 2018 03 26
  */

#ifndef __MPU6050_H__
#define __MPU6050_H__

#include <stdint.h>

#define I2C_WriteByte  i2c_write
#define I2C_ReadByte   i2c_read

#define  SMPLRT_DIV   0x19  //陀螺仪输出率的分频，典型值0x07,(1kHz)
#define  CONFIG       0x1A  //低通滤波频率,一般0x01~0x05,数值越大带宽越小延时越长
#define  GYRO_CONFIG  0x1B  
#define  ACCEL_CONFIG 0x1C  
#define  ACCEL_XOUT_H 0x3B    //加速度计X轴数据高位
#define  ACCEL_XOUT_L 0x3C    //加速度计X轴数据低位
#define  ACCEL_YOUT_H 0x3D    //以此类推
#define  ACCEL_YOUT_L 0x3E
#define  ACCEL_ZOUT_H 0x3F
#define  ACCEL_ZOUT_L 0x40
#define  TEMP_OUT_H   0x41    //温度传感器数据
#define  TEMP_OUT_L   0x42
#define  GYRO_XOUT_H  0x43    //陀螺仪X轴数据高位
#define  GYRO_XOUT_L  0x44
#define  GYRO_YOUT_H  0x45
#define  GYRO_YOUT_L  0x46
#define  GYRO_ZOUT_H  0x47
#define  GYRO_ZOUT_L  0x48
#define  PWR_MGMT_1   0x6B    //电源管理，典型值：0x00(正常启用)
#define  WHO_AM_I     0x75    //IIC地址寄存器(默认数值0x68，只读)
#define  SlaveAddress 0xD0    //MPU6050模块AD0引脚接低电平时的地址


void mpu6050_config(void);
void mpu6050_get_acc_gyro_proc(void);
void mpu6050_show(void);
float *mpu6050_angle(void);

#endif
