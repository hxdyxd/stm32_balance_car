/* Uart 2018 06 27 END */
/* By hxdyxd */

#ifndef  _INTERFACE_USART_H_
#define  _INTERFACE_USART_H_

#include "stm32f10x.h"
#include "systick.h"

#define USART_TX_TIMEOUT_MS  (10)


void interface_usart_init(void);
void interface_usart_write(const unsigned char *p, int len);
void interface_usart_write_wait(void);

#endif
