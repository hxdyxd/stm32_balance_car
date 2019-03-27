/**
  * @author: hxdyxd@gmail.com
  * @date: 2018 03 26
  */

#ifndef __MPU6050_H__
#define __MPU6050_H__

#include <stdint.h>

#define I2C_WriteByte  i2c_write
#define I2C_ReadByte   i2c_read

#define  SMPLRT_DIV   0x19  //����������ʵķ�Ƶ������ֵ0x07,(1kHz)
#define  CONFIG       0x1A  //��ͨ�˲�Ƶ��,һ��0x01~0x05,��ֵԽ�����ԽС��ʱԽ��
#define  GYRO_CONFIG  0x1B  
#define  ACCEL_CONFIG 0x1C  
#define  ACCEL_XOUT_H 0x3B    //���ٶȼ�X�����ݸ�λ
#define  ACCEL_XOUT_L 0x3C    //���ٶȼ�X�����ݵ�λ
#define  ACCEL_YOUT_H 0x3D    //�Դ�����
#define  ACCEL_YOUT_L 0x3E
#define  ACCEL_ZOUT_H 0x3F
#define  ACCEL_ZOUT_L 0x40
#define  TEMP_OUT_H   0x41    //�¶ȴ���������
#define  TEMP_OUT_L   0x42
#define  GYRO_XOUT_H  0x43    //������X�����ݸ�λ
#define  GYRO_XOUT_L  0x44
#define  GYRO_YOUT_H  0x45
#define  GYRO_YOUT_L  0x46
#define  GYRO_ZOUT_H  0x47
#define  GYRO_ZOUT_L  0x48
#define  PWR_MGMT_1   0x6B    //��Դ��������ֵ��0x00(��������)
#define  WHO_AM_I     0x75    //IIC��ַ�Ĵ���(Ĭ����ֵ0x68��ֻ��)
#define  SlaveAddress 0xD0    //MPU6050ģ��AD0���Žӵ͵�ƽʱ�ĵ�ַ


void mpu6050_config(void);
void mpu6050_get_acc_gyro_proc(void);
void mpu6050_show(void);
float *mpu6050_angle(void);

#endif
