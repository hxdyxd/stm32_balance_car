/* 
 *
 *2018 09 21 & hxdyxd
 *
 */


#ifndef __data_interface_hal_H__
#define __data_interface_hal_H__

#include <stdint.h>

#include "leds.h"
#include "keys.h"

//led id
#define LED_G       (0)
#define LED_R       (1)
#define LED_BELL    (2)

/*******************************************************************************
* Function Name  : data_interface_hal_init.
* Description    :Hardware adaptation layer initialization.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void data_interface_hal_init(void);



inline uint64_t hal_read_TickCounter(void);


#endif
