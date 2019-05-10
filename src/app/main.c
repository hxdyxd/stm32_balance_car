/**
  * @author: hxdyxd@gmail.com
  * @date: 2018 03 24
  */
#include <string.h>

#include "stm32f10x.h"
#include "soft_timer.h"
#include "app_debug.h"
#include "type.h"
#include "control.h"
#include "mpu6050.h"

//hal
#include "data_interface_hal.h"

uint8_t g_led_red_flag = 0;


#define BLUETOOTH_S_NOT_INIT   (0)
#define BLUETOOTH_S_NAME       (1)
#define BLUETOOTH_S_WAIT_CONN  (2)
#define BLUETOOTH_S_CONNECTED  (3)

#define BLUETOOTH_S_PIN        (4)

static uint8_t bluetooth_init_state = BLUETOOTH_S_NOT_INIT;
char bluetooth_buf[255];


void led_proc(void)
{
    led_rev(LED_G);
    if(Pitch > 30 || -Pitch > 30) {
        led_off(LED_BELL);
        g_led_red_flag = 1;
        soft_timer_delete(SOFT_TIMER_CONTROL_MOTOR);
        motor_set(0, 0);
    }
    if(g_led_red_flag) {
        led_rev(LED_R);
    } else {
        led_off(LED_R);
    }
}

void key_press_proc(int8_t id)
{
    if(g_led_red_flag) {
        //开启电机控制
        control_reset();
        soft_timer_create(SOFT_TIMER_CONTROL_MOTOR, 1, 1, control_motor_proc, 10);
        g_led_red_flag = 0;
        //关闭蜂鸣器
        led_on(LED_BELL);
    }
}

void sensors_proc(void)
{
    MPU6050_Pose();
    encoder_get_pulse();
}

void sensors_show_proc(void)
{
    APP_DEBUG("mpu6050: %.3f %.3f %.3f \r\n", Pitch, Roll, Yaw);
    APP_DEBUG("encoder: %d %d \r\n", g_s32LeftMotorPulseSigma, g_s32RightMotorPulseSigma);
}



void bluetooth_init_proc(void)
{
    uint8_t str_len;
    switch(bluetooth_init_state) {
    case BLUETOOTH_S_NOT_INIT:
        interface_usart_write( (uint8_t *)"AT\r\n", 4);
        APP_DEBUG("AT\r\n");
        bluetooth_init_state = BLUETOOTH_S_NAME;
        soft_timer_create(SOFT_TIMER_BLUETOOTH_INIT, 1, 0, bluetooth_init_proc, 100);
        break;
    
    case BLUETOOTH_S_NAME:
        str_len = sprintf(bluetooth_buf, "AT+NAME%s\r\n", BLUETOOTH_NAME);
        interface_usart_write( (uint8_t *)bluetooth_buf, str_len);
        APP_DEBUG("%s", bluetooth_buf);
        bluetooth_init_state = BLUETOOTH_S_PIN;
        soft_timer_create(SOFT_TIMER_BLUETOOTH_INIT, 1, 0, bluetooth_init_proc, 100);
        break;
    
    case BLUETOOTH_S_PIN:
        str_len = sprintf(bluetooth_buf, "AT+PIN\r\n");
        interface_usart_write( (uint8_t *)bluetooth_buf, str_len);
        APP_DEBUG("%s", bluetooth_buf);
        bluetooth_init_state = BLUETOOTH_S_WAIT_CONN;
        soft_timer_create(SOFT_TIMER_BLUETOOTH_INIT, 1, 0, bluetooth_init_proc, 100);
        break;
    
    case BLUETOOTH_S_WAIT_CONN:
        str_len = sprintf(bluetooth_buf, "AT+NAME\r\n");
        interface_usart_write( (uint8_t *)bluetooth_buf, str_len);
        APP_DEBUG("%s", bluetooth_buf);
    
        APP_DEBUG("wait connect\r\n");
        soft_timer_delete(SOFT_TIMER_BLUETOOTH_INIT);
        bluetooth_init_state = BLUETOOTH_S_NOT_INIT;
        break;
    default:
        APP_ERROR("bluetooth error \r\n");
        soft_timer_delete(SOFT_TIMER_BLUETOOTH_INIT);
    }
}

void uart_receive_proc(uint8_t *buf, uint8_t len)
{
    APP_DEBUG("%.*s (%d)\r\n", len, (char *)buf, len);
    int str_len = 0;
    char cmd = '0';
    int value = 0;
    
    buf[len] = '\0';
    int ret_num = sscanf( (char *)buf, "%c %d", &cmd, &value);
    if(ret_num < 1) {
        return;
    }
    
    switch(cmd) {
        case 'w':
            //forward
            control_speed_set(value);
            break;
        case 'a':
            //left
            control_direction_set(value);
            break;
        case 'o':
            //turn on motor control
            control_reset();
            soft_timer_create(SOFT_TIMER_CONTROL_MOTOR, 1, 1, control_motor_proc, 5);
            g_led_red_flag = 0;
            //关闭蜂鸣器
            led_on(LED_BELL);
            break;
        case 'f':
            //turn off motor
            motor_set(0, 0);
            //turn off motor control
            soft_timer_delete(SOFT_TIMER_CONTROL_MOTOR);
            g_led_red_flag = 1;
            break;
        case 'p':
            //show pid value
            str_len = sprintf(bluetooth_buf, "pid: %.3f, %.3f %.3f, %.3f %.3f \r\n", gf_dir_kp, gf_ang_kp, gf_ang_kd, gf_spd_kp, gf_spd_ki);
            interface_usart_write( (uint8_t *)bluetooth_buf, str_len);
            break;
        case 'm':
            //show sensors info
            str_len = sprintf(bluetooth_buf, "accel: %d %d %d \r\n", accel[0], accel[1], accel[2]);
            interface_usart_write( (uint8_t *)bluetooth_buf, str_len);
            str_len = sprintf(bluetooth_buf, "gyro: %d %d %d \r\n", gyro[0], gyro[1], gyro[2]);
            interface_usart_write( (uint8_t *)bluetooth_buf, str_len);
            str_len = sprintf(bluetooth_buf, "direction: %.3f %.3f %.3f \r\n", Pitch, Roll, Yaw);
            interface_usart_write( (uint8_t *)bluetooth_buf, str_len);
            break;
        default:
            ;
    }
}


int main(void)
{
    data_interface_hal_init();
    PRINTF("\r\n\r\n Build , %s %s \r\n", __DATE__, __TIME__);
    
    soft_timer_init();
    
    soft_timer_create(SOFT_TIMER_LED, 1, 1, led_proc, 100);
    soft_timer_create(SOFT_TIMER_MPU6050, 1, 1, sensors_proc, 5);
    soft_timer_create(SOFT_TIMER_MPU6050_SHOW, 1, 1, sensors_show_proc, 1000);
    
//    soft_timer_create(SOFT_TIMER_CONTROL_MOTOR, 1, 1, control_motor_proc, 5);
    
    soft_timer_create(SOFT_TIMER_BLUETOOTH_INIT, 1, 0, bluetooth_init_proc, 100);
    
    led_off(LED_BELL);
    MPU6050_Init();
    led_on(LED_BELL);
    
    APP_DEBUG("init success \r\n");
    motor_set(0, 0);
    
    
    while(1)
    {
        soft_timer_proc();   //timer
        keys_proc(key_press_proc);  //key
        usart_rx_proc(uart_receive_proc);
    }
    
}

/******************* (C) COPYRIGHT 2018 hxdyxd *****END OF FILE****/
