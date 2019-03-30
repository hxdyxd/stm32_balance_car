/* Uart_rx 2019 03 30 END */
/* By hxdyxd */

#include "usart_rx.h"
#include "systick.h"
#include "app_debug.h"

#define TickCounter  (clk_count)


volatile uint8_t USART_RX_FLAG = 0;
volatile uint8_t usart_rx_counter = 0;
uint8_t usart_buffer[USART_RX_MAX_NUM + 1];
uint64_t usart_rx_timer = 0;

void usart_rx_isr_init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
}

void usart_rx_proc( void (* usart_rx_callback)(uint8_t *, uint8_t) )
{
    //if( !USART_RX_FLAG ) {
        if(usart_rx_counter != 0 && (TickCounter - usart_rx_timer) > USART_RX_TIMEOUT_MS ) {
            //timeout detect
            USART_RX_FLAG = 1;
            usart_rx_callback(usart_buffer, usart_rx_counter);
            usart_rx_counter = 0;
            USART_RX_FLAG = 0;
            //APP_WARN("RXD\r\n");
        }
    //}
}

//unsigned char *usart_rx_get_buffer_ptr(void)
//{
//    return usart_buffer;
//}

//uint8_t usart_rx_get_length(void)
//{
//    if( USART_RX_FLAG ) {
//        return usart_rx_counter;
//    } else {
//        return 0;
//    }
//}

//void usart_rx_release(void)
//{
//    if( USART_RX_FLAG ) {
//        usart_rx_counter = 0;
//        USART_RX_FLAG = 0;
//    }
//}


void USART3_IRQHandler(void)
{
    if(USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
    {
        if( USART_RX_FLAG == 0 ) {
            
            if(usart_rx_counter != 0 && (TickCounter - usart_rx_timer) > USART_RX_TIMEOUT_MS ) {
                //timeout detect
                USART_RX_FLAG = 1;
            } else if(usart_rx_counter >= USART_RX_MAX_NUM) {
                //usart_rx_counter = 0;
                USART_RX_FLAG = 1;
            } else {
                usart_buffer[ usart_rx_counter++ ] = USART3->DR;
                usart_rx_timer = TickCounter;
            }
        }
        
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    }
}

/******************* (C) COPYRIGHT 2019 03 30 hxdyxd *****END OF FILE****/
