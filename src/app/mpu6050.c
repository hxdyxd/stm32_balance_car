/**
  * @author: hxdyxd@gmail.com
  * @date: 2018 03 26
  */

#include <math.h>

//hal
#include "data_interface_hal.h"
#include "mpu6050.h"
#include "app_debug.h"


void mpu6050_config(void)
{
    I2C_WriteByte(PWR_MGMT_1,   0x00);     //解除休眠状态
    I2C_WriteByte(SMPLRT_DIV,   0x07);     //陀螺仪采样率1kHz
    I2C_WriteByte(CONFIG,       0x02);     //设置低通滤波器
    I2C_WriteByte(GYRO_CONFIG,  0x18);     //陀螺仪量程2000deg/s
    I2C_WriteByte(ACCEL_CONFIG, 0x08);     //加速度量程4g
}


static uint16_t mpu6050_get_value(uint8_t REG_Address)
{
    uint8_t H, L;
    H = I2C_ReadByte(REG_Address);
    L = I2C_ReadByte(REG_Address+1);
    return (H << 8) + L;
}


int16_t acc1[3]={0};
int16_t gyr1[3]={0};
float acc[3]={0};
float gyr[3]={0};

#undef _DEBUG
#ifdef _DEBUG
int16_t avg_acc1[3]={0};
int16_t avg_gyr1[3]={0};
#endif

void  mpu6050_rectify_data(void)//校准偏差、换算单位
{
    static int16_t a = 76;
    static int16_t b = 76;
    static int16_t c = -2200;
    static int16_t d = 5;
    static int16_t e = -6;
    static int16_t f = 1;
    u8 i;
    acc[0] = (float)(acc1[0] + a);
    acc[1] = (float)(acc1[1] + b);
    acc[2] = (float)(acc1[2] + c);
    gyr[0] = (float)(gyr1[0] + d);
    gyr[1] = (float)(gyr1[1] + e);
    gyr[2] = (float)(gyr1[2] + f);
//a,b,c,d,e,f为校准偏差时测得的平均值，写程序的时候直接带入即可
    for(i=0; i<3; i++)
    {
        acc[i] /= 8192.0f;
        gyr[i] /= 16.384f;
#ifdef _DEBUG
        if(avg_acc1[i] == 0 && avg_gyr1[i] == 0) {
            avg_acc1[i] = acc1[i];
            avg_gyr1[i] = gyr1[i];
        } else {
            avg_acc1[i] = (acc1[i] + avg_acc1[i])/2;
            avg_gyr1[i] = (gyr1[i] + avg_gyr1[i])/2;
        }
#endif
    }
#ifdef _DEBUG
     APP_DEBUG("avg_acc1_dbg: %d %d %d \r\n", avg_acc1[0], avg_acc1[1], avg_acc1[2]);
#endif
}


const float fRad2Deg = 57.295779513f; //弧度换算角度乘的系数
const float R = 0.98f;
//const float dt = 0.005; //时间周期
float dt = 0;
float angle[3] = {0};

void ImuCalculate_Complementary(void)//计算角度
{
    u8 i;
    static float angle_last[3]={0};
    static uint64_t last_time = 0;
    float temp[3] = {0};
    
    uint64_t this_time = hal_read_TickCounter();
    dt = (this_time - last_time)/1000.0f;
    last_time = this_time;
    
    temp[0] = sqrt(acc[1]*acc[1]+acc[2]*acc[2]);
    temp[1] = sqrt(acc[0]*acc[0]+acc[2]*acc[2]);

    for(i = 0; i < 2; i++)//pitch and roll
    {
        angle[i] = R*(angle_last[i]+gyr[i]*dt) 
                    + (1-R)*fRad2Deg*atan(acc[i]/temp[i]);
        angle_last[i] = angle[i];
    }
    
    angle[2] = angle_last[2] + gyr[2]*dt;//yaw
    angle_last[2] = angle[2];
}

void mpu6050_get_acc_gyro_proc(void)
{
    acc1[0] = mpu6050_get_value(ACCEL_XOUT_H);
    acc1[1] = mpu6050_get_value(ACCEL_YOUT_H);
    acc1[2] = mpu6050_get_value(ACCEL_ZOUT_H);
    gyr1[0] = mpu6050_get_value(GYRO_XOUT_H);
    gyr1[1] = mpu6050_get_value(GYRO_YOUT_H);
    gyr1[2] = mpu6050_get_value(GYRO_ZOUT_H);
    mpu6050_rectify_data();
    ImuCalculate_Complementary();
    //APP_DEBUG("acc1: %d %d %d \r\n", acc1[0], acc1[1], acc1[2]);
    //APP_DEBUG("gyr1: %d %d %d \r\n", gyr1[0], gyr1[1], gyr1[2]);
    //APP_DEBUG("acc: %.3f %.3f %.3f \r\n", acc[0], acc[1], acc[2]);
    //APP_DEBUG("gyr: %.3f %.3f %.3f \r\n", gyr[0], gyr[1], gyr[2]);
    //APP_DEBUG("angle: %.3f %.3f %.3f \r\n", angle[0], angle[1], angle[2]);
}

void mpu6050_show(void)
{
    APP_DEBUG("angle: %.3f %.3f %.3f dt: %.3f\r\n", angle[0], angle[1], angle[2], dt);
}

float *mpu6050_angle(void)
{
    return angle;
}


/******************* (C) COPYRIGHT 2019 hxdyxd *****END OF FILE****/
