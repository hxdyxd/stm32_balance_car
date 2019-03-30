/* Uart_rx 2019 03 30 END */
/* By hxdyxd */

#ifndef __USART_RX_H__
#define __USART_RX_H__

#include "stm32f10x.h"

#define USART_RX_MAX_NUM  (255)
#define USART_RX_TIMEOUT_MS  (10)



void usart_rx_isr_init(void);
void usart_rx_proc( void (* usart_rx_callback)(uint8_t *, uint8_t) );



//unsigned char *usart_rx_get_buffer_ptr(void);
//uint8_t usart_rx_get_length(void);
//void usart_rx_release(void);


#endif
/******************* (C) COPYRIGHT 2019 03 30 hxdyxd *****END OF FILE****/
