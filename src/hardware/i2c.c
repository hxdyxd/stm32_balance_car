/**
  * @author: hxdyxd@gmail.com
  * @date: 2018 03 26
  */
  
#include "stm32f10x.h"
#include "i2c.h"

#define SlaveAddress  (0xd0)
#define sEE_FLAG_TIMEOUT 0x1000
#define sEE_LONG_TIMEOUT (sEE_FLAG_TIMEOUT*10)

/**
  * @brief  Start critical section: these callbacks should be typically used
  *         to disable interrupts when entering a critical section of I2C communication
  *         You may use default callbacks provided into this driver by uncommenting the 
  *         define USE_DEFAULT_CRITICAL_CALLBACK.
  *         Or you can comment that line and implement these callbacks into your 
  *         application.
  * @param  None.
  * @retval None.
  */
#define sEE_ExitCriticalSection_UserCallback  __enable_irq
#define sEE_EnterCriticalSection_UserCallback __disable_irq


void i2c_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    I2C_InitTypeDef I2C_InitStructure;
    //开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    
    //初始化GPIO
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    GPIO_PinRemapConfig(GPIO_Remap_I2C1, ENABLE);
    //初始化I2C，开启I2C1
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = 400000;
    I2C_Init(I2C1, &I2C_InitStructure);
    I2C_Cmd(I2C1, ENABLE);
    //I2C_AcknowledgeConfig(I2C1, ENABLE);
}

int i2c_write(uint8_t REG_Address, uint8_t REG_data)
{
    int sEETimeout;
    /*!< While the bus is busy */
    sEETimeout = sEE_LONG_TIMEOUT;
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
    {
        if((sEETimeout--) == 0) return (0);
    }
    
    /*!< Send START condition */
    I2C_GenerateSTART(I2C1,ENABLE);
    
    /*!< Test on EV5 and clear it */
    sEETimeout = sEE_FLAG_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if((sEETimeout--) == 0) return (0);
    }
    
    I2C_Send7bitAddress(I2C1, SlaveAddress, I2C_Direction_Transmitter);
    /*!< Test on EV6 and clear it */
    sEETimeout = sEE_FLAG_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if((sEETimeout--) == 0) return (0);
    }
    
    I2C_SendData(I2C1, REG_Address);
    
    /*!< Test on EV8 and clear it */
    sEETimeout = sEE_FLAG_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if((sEETimeout--) == 0) return (0);
    }
    
    I2C_SendData(I2C1, REG_data);
    
    
    /*!< Test on EV8 and clear it */
    sEETimeout = sEE_FLAG_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if((sEETimeout--) == 0) return (0);
    }
    
    I2C_GenerateSTOP(I2C1, ENABLE);
    
    /* Wait to make sure that STOP control bit has been cleared */
    sEETimeout = sEE_FLAG_TIMEOUT;
    while(I2C1->CR1 & I2C_CR1_STOP)
    {
        if((sEETimeout--) == 0) return (0);
    }
    
    return 0;
}


uint8_t i2c_read(uint8_t REG_Address)
{
    int sEETimeout;
    
    /*!< While the bus is busy */
    sEETimeout = sEE_LONG_TIMEOUT;
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
    {
    if((sEETimeout--) == 0) return (0);
    }
    
    /*!< Send START condition */
    I2C_GenerateSTART(I2C1, ENABLE);
    
    /*!< Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
    sEETimeout = sEE_FLAG_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if((sEETimeout--) == 0) return (0);
    }
    
    /*!< Send EEPROM address for write */
    I2C_Send7bitAddress(I2C1, SlaveAddress, I2C_Direction_Transmitter);
    
    /*!< Test on EV6 and clear it */
    sEETimeout = sEE_FLAG_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if((sEETimeout--) == 0) return (0);
    }
    
    /*!< Send the Slave's internal address to read from: Only one byte address */
    I2C_SendData(I2C1, REG_Address);
    
    /*!< Test on EV8 and clear it */
    sEETimeout = sEE_FLAG_TIMEOUT;
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF) == RESET)
    {
        if((sEETimeout--) == 0) return (0);
    }
    
    /*!< Send STRAT condition a second time */
    I2C_GenerateSTART(I2C1, ENABLE);
    /*!< Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
    sEETimeout = sEE_FLAG_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if((sEETimeout--) == 0) return (0);
    }
    
    /*!< Send Slave address for read */
    I2C_Send7bitAddress(I2C1, SlaveAddress, I2C_Direction_Receiver);
    
    /* Wait on ADDR flag to be set (ADDR is still not cleared at this level */
    sEETimeout = sEE_FLAG_TIMEOUT;
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_ADDR) == RESET)
    {
        if((sEETimeout--) == 0) return (0);
    }
    
     /*!< Disable Acknowledgement */
    I2C_AcknowledgeConfig(I2C1, DISABLE);
    
    /* Call User callback for critical section start (should typically disable interrupts) */
    sEE_EnterCriticalSection_UserCallback();
    
    /* Clear ADDR register by reading SR1 then SR2 register (SR1 has already been read) */
    (void)I2C1->SR2;
    
    /*!< Send STOP Condition */
    I2C_GenerateSTOP(I2C1, ENABLE);
    
    /* Call User callback for critical section end (should typically re-enable interrupts) */
    sEE_ExitCriticalSection_UserCallback();
    
    sEETimeout = sEE_FLAG_TIMEOUT;
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET)
    {
        if((sEETimeout--) == 0) return (0);
    }
    
    //在当前字节传输或在当前起始条件发出后产生停止条件
    /* Wait to make sure that STOP control bit has been cleared */
    sEETimeout = sEE_FLAG_TIMEOUT;
    while(I2C1->CR1 & I2C_CR1_STOP)
    {
        if((sEETimeout--) == 0) return (0);
    }
    
    /*!< Re-Enable Acknowledgement to be ready for another reception */
    I2C_AcknowledgeConfig(I2C1, ENABLE);
    
    return I2C_ReceiveData(I2C1);//读出寄存器数据
}


/******************* (C) COPYRIGHT 2019 hxdyxd *****END OF FILE****/
