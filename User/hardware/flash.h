/*
 * hxdyxd@gmail.com, 2018
 */

#ifndef __FLASH_H__
#define __FLASH_H__

#include "stm32f10x.h"
#include "stdio.h"

#define FLASH_Page_Size    ((uint32_t) 0x00000400) //FLASHҳ�Ĵ�С
//#define Index_Start_Addr   ((uint32_t) 0x08005000) //���ݴ洢��־��ʼ��ַ
//#define Index_End_Addr     ((uint32_t) 0x08005400) //���ݴ洢��־������ַ
#define FLASH_Start_Addr   ((uint32_t) 0x0800FC00) //���ݴ洢��ʼ��ַ  1K
#define FLASH_End_Addr     ((uint32_t) 0x08010000) //���ݴ洢������ַ
#define DATA_32            ((uint32_t) 0x12345678) //Ҫ��д������

uint32_t xfs_sava_cfg(const unsigned char *cfg, uint32_t len);

#endif

