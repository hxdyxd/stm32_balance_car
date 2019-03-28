/**
  * @author: hxdyxd@gmail.com
  * @date: 2018 03 26
  */
  
#include "stm32f10x.h"
#include "i2c.h"

#define DEBUG_I2C  1

#if DEBUG_I2C
    #include "app_debug.h"
    #define dbg  APP_DEBUG
#else
    #define dbg
#endif



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


#define sEE_I2C_DR_Address               (&I2C1->DR)
#define sEE_DIRECTION_TX                 0
#define sEE_DIRECTION_RX                 1 

DMA_InitTypeDef sEEDMA_InitStructure;
uint32_t sEEDataWritePointer;
uint32_t sEEDataReadPointer;

static void i2c_dma_init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    /*!< I2C DMA TX and RX channels configuration */
    /* Enable the DMA clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    /* I2C TX DMA Channel configuration */
    DMA_DeInit(DMA1_Channel6);  //I2C1 TX
    sEEDMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)sEE_I2C_DR_Address;
    sEEDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)0;   /* This parameter will be configured durig communication */
    sEEDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;    /* This parameter will be configured durig communication */
    sEEDMA_InitStructure.DMA_BufferSize = 0xFFFF;            /* This parameter will be configured durig communication */
    sEEDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    sEEDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    sEEDMA_InitStructure.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
    sEEDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    sEEDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    sEEDMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    sEEDMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel6, &sEEDMA_InitStructure);

    /* I2C RX DMA Channel configuration */
    DMA_DeInit(DMA1_Channel7);  //I2C1 RX
    DMA_Init(DMA1_Channel7, &sEEDMA_InitStructure);
    
        /* Configure and enable I2C DMA RX Channel interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;
    NVIC_Init(&NVIC_InitStructure);

    /* Enable the DMA Channels Interrupts */
    DMA_ITConfig(DMA1_Channel6, DMA_IT_TC, ENABLE);
    DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);
}

/**
  * @brief  Initializes DMA channel used by the I2C EEPROM driver.
  * @param  None
  * @retval None
  */
static void sEE_LowLevel_DMAConfig(uint32_t pBuffer, uint32_t BufferSize, uint32_t Direction)
{ 
    /* Initialize the DMA with the new parameters */
    if (Direction == sEE_DIRECTION_TX)
    {
        /* Configure the DMA Tx Channel with the buffer address and the buffer size */
        sEEDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)pBuffer;
        sEEDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
        sEEDMA_InitStructure.DMA_BufferSize = (uint32_t)BufferSize;
        DMA_Init(DMA1_Channel6, &sEEDMA_InitStructure);
    }
    else
    { 
        /* Configure the DMA Rx Channel with the buffer address and the buffer size */
        sEEDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)pBuffer;
        sEEDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
        sEEDMA_InitStructure.DMA_BufferSize = (uint32_t)BufferSize;
        DMA_Init(DMA1_Channel7, &sEEDMA_InitStructure);
    }
}


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
    
     /* Enable the sEE_I2C peripheral DMA requests */
    I2C_DMACmd(I2C1, ENABLE);
    
    i2c_dma_init();
}


int i2c_write_byte(uint8_t REG_Address, uint8_t REG_data)
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


static uint8_t i2c_read_byte(uint8_t REG_Address)
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


int8_t i2c_write_poll(uint8_t addr, uint8_t reg, uint8_t len, uint8_t * data)
{
    int sEETimeout;
    sEEDataWritePointer = len;
    
    /*!< While the bus is busy */
    sEETimeout = sEE_LONG_TIMEOUT;
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
    {
        if((sEETimeout--) == 0) {
            dbg("While the bus is busy \r\n");
            return (-1);
        }
    }
    
    /*!< Send START condition */
    I2C_GenerateSTART(I2C1,ENABLE);
    
    /*!< Test on EV5 and clear it */
    sEETimeout = sEE_FLAG_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if((sEETimeout--) == 0) {
            dbg("Test on EV5 \r\n");
            return (-1);
        }
    }
    
    I2C_Send7bitAddress(I2C1, SlaveAddress, I2C_Direction_Transmitter);
    /*!< Test on EV6 and clear it */
    sEETimeout = sEE_FLAG_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if((sEETimeout--) == 0) {
            dbg("Test on EV6 \r\n");
            return (-1);
        }
    }
    
    I2C_SendData(I2C1, reg);
    
    /*!< Test on EV8 and clear it */
    sEETimeout = sEE_FLAG_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if((sEETimeout--) == 0) {
            dbg("Test on EV8 \r\n");
            return (-1);
        }
    }
    
    while(len--) {
    
        I2C_SendData(I2C1, *data++);
        
        
        /*!< Test on EV8 and clear it */
        sEETimeout = sEE_FLAG_TIMEOUT;
        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        {
            if((sEETimeout--) == 0) {
                dbg("Test on EV8 \r\n");
                return (-1);
            }
        }
    }
    
    I2C_GenerateSTOP(I2C1, ENABLE);
    
    /* Wait to make sure that STOP control bit has been cleared */
    sEETimeout = sEE_FLAG_TIMEOUT;
    while(I2C1->CR1 & I2C_CR1_STOP)
    {
        if((sEETimeout--) == 0) {
            dbg("Wait to make sure that STOP \r\n");
            return (-1);
        }
    }
    
    return 0;
}

static int8_t i2c_read_bytes(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
    int sEETimeout;
    sEEDataReadPointer = len;
    
    /*!< While the bus is busy */
    sEETimeout = sEE_LONG_TIMEOUT;
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
    {
        if((sEETimeout--) == 0) {
            dbg("While the bus is busy \r\n");
            return (-1);
        }
    }
    
    /*!< Send START condition */
    I2C_GenerateSTART(I2C1, ENABLE);
    
    /*!< Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
    sEETimeout = sEE_FLAG_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if((sEETimeout--) == 0) {
            dbg("Test on EV5 \r\n");
            return (-1);
        }
    }
    
    /*!< Send EEPROM address for write */
    I2C_Send7bitAddress(I2C1, SlaveAddress, I2C_Direction_Transmitter);
    
    /*!< Test on EV6 and clear it */
    sEETimeout = sEE_FLAG_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if((sEETimeout--) == 0) {
            dbg("Test on EV6 \r\n");
            return (-1);
        }
    }
    
    /*!< Send the Slave's internal address to read from: Only one byte address */
    I2C_SendData(I2C1, reg);
    
    /*!< Test on EV8 and clear it */
    sEETimeout = sEE_FLAG_TIMEOUT;
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF) == RESET)
    {
        if((sEETimeout--) == 0) {
            dbg("Test on EV8 \r\n");
            return (-1);
        }
    }
    
    /*!< Send STRAT condition a second time */
    I2C_GenerateSTART(I2C1, ENABLE);
    /*!< Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
    sEETimeout = sEE_FLAG_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if((sEETimeout--) == 0) {
            dbg("Test on EV5 \r\n");
            return (-1);
        }
    }
    
    /*!< Send Slave address for read */
    I2C_Send7bitAddress(I2C1, SlaveAddress, I2C_Direction_Receiver);
    
    
    /* More than one Byte Master Reception procedure (DMA) -----------------*/
    sEETimeout = sEE_FLAG_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        if((sEETimeout--) == 0) {
            dbg("Test on EV6 \r\n");
            return (-1);
        }
    }
    
    /* Configure the DMA Rx Channel with the buffer address and the buffer size */
    sEE_LowLevel_DMAConfig((uint32_t)buf, (uint16_t)(len), sEE_DIRECTION_RX);
    
    /* Inform the DMA that the next End Of Transfer Signal will be the last one */
    I2C_DMALastTransferCmd(I2C1, ENABLE); 
    
    /* Enable the DMA Rx Channel */
    DMA_Cmd(DMA1_Channel7, ENABLE);
    
//    dbg("start dma recv \r\n");
    sEETimeout = 1000000;
    while(sEEDataReadPointer) {
        if((sEETimeout--) == 0) {
            dbg("[I2C] Recv timeout \r\n");
            return (-1);
        }
    }
    
    return 0;
}

int8_t i2c_write(uint8_t addr, uint8_t reg, uint8_t len, uint8_t * data)
{
    int sEETimeout;
    sEEDataWritePointer = len;
    
    /*!< While the bus is busy */
    sEETimeout = sEE_LONG_TIMEOUT;
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
    {
        if((sEETimeout--) == 0) {
            dbg("While the bus is busy \r\n");
            return (-1);
        }
    }
    
    /*!< Send START condition */
    I2C_GenerateSTART(I2C1,ENABLE);
    
    /*!< Test on EV5 and clear it */
    sEETimeout = sEE_FLAG_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if((sEETimeout--) == 0) {
            dbg("Test on EV5 \r\n");
            return (-1);
        }
    }
    
    I2C_Send7bitAddress(I2C1, SlaveAddress, I2C_Direction_Transmitter);
    /*!< Test on EV6 and clear it */
    sEETimeout = sEE_FLAG_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if((sEETimeout--) == 0) {
            dbg("Test on EV6 \r\n");
            return (-1);
        }
    }
    
    I2C_SendData(I2C1, reg);
    
    /*!< Test on EV8 and clear it */
    sEETimeout = sEE_FLAG_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if((sEETimeout--) == 0) {
            dbg("Test on EV8 \r\n");
            return (-1);
        }
    }
    
    /* Configure the DMA Tx Channel with the buffer address and the buffer size */
    sEE_LowLevel_DMAConfig((uint32_t)data, (uint8_t)(len), sEE_DIRECTION_TX);

    /* Enable the DMA Tx Channel */
    DMA_Cmd(DMA1_Channel6, ENABLE);
    
    //    dbg("start dma write \r\n");
    sEETimeout = 1000000;
    while(sEEDataWritePointer) {
        if((sEETimeout--) == 0) {
            dbg("[I2C] Write timeout \r\n");
            return (-1);
        }
    }
    
    return 0;
}


int8_t i2c_read(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
    /* If number of data to be read is 1, then DMA couldn't be used */
    /* One Byte Master Reception procedure (POLLING) ---------------------------*/
    if(len < 2) {
        *buf = i2c_read_byte(reg);
        return 0;
    } else {
        return i2c_read_bytes(addr, reg, len, buf);
    }
}



/**
  * @brief  This function handles the DMA Tx Channel interrupt Handler.
  * @param  None
  * @retval None
  */
void DMA1_Channel6_IRQHandler(void)
{
    /* Check if the DMA transfer is complete */ 
    if(DMA_GetFlagStatus(DMA1_IT_TC6) != RESET)
    {  
        /* Disable the DMA Tx Channel and Clear all its Flags */  
        DMA_Cmd(DMA1_Channel6, DISABLE);
        DMA_ClearFlag(DMA1_IT_GL6);

        /*!< Wait till all data have been physically transferred on the bus */
        uint32_t sEETimeout = sEE_LONG_TIMEOUT;
        while(!I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF))
        {
            if((sEETimeout--) == 0) {
                dbg("Test on I2C_FLAG_BTF \r\n");
                return;
            }
        }

        /*!< Send STOP condition */
        I2C_GenerateSTOP(I2C1, ENABLE);

        /* Perform a read on SR1 and SR2 register to clear eventualaly pending flags */
        (void)I2C1->SR1;
        (void)I2C1->SR2;

        /* Reset the variable holding the number of data to be written */
        sEEDataWritePointer = 0;
        //dbg("tc \r\n");
    }
}

/**
  * @brief  This function handles the DMA Rx Channel interrupt Handler.
  * @param  None
  * @retval None
  */
void DMA1_Channel7_IRQHandler(void)
{
    /* Check if the DMA transfer is complete */
    if(DMA_GetFlagStatus(DMA1_IT_TC7) != RESET)
    {      
        /*!< Send STOP Condition */
        I2C_GenerateSTOP(I2C1, ENABLE);

        /* Disable the DMA Rx Channel and Clear all its Flags */  
        DMA_Cmd(DMA1_Channel7, DISABLE);
        DMA_ClearFlag(DMA1_IT_GL7);

        /* Reset the variable holding the number of data to be read */
        sEEDataReadPointer = 0;
        //dbg("rc \r\n");
    }
}







/******************* (C) COPYRIGHT 2019 hxdyxd *****END OF FILE****/
