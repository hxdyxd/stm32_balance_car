#include "leds.h"

#define LED_NUM     (3)

struct sGpio LED_GPIO[LED_NUM] = {
	//RED
	{
		.RCC_APB2Periph = RCC_APB2Periph_GPIOA,
		.GPIOx = GPIOA,
		.GPIO_Pin = GPIO_Pin_3,
	},
	//GREEN
	{
		.RCC_APB2Periph = RCC_APB2Periph_GPIOA,
		.GPIOx = GPIOA,
		.GPIO_Pin = GPIO_Pin_12,
	},
	//BELL
	{
		.RCC_APB2Periph = RCC_APB2Periph_GPIOA,
		.GPIOx = GPIOA,
		.GPIO_Pin = GPIO_Pin_4,
	},	
};

void DisableJtag_enableSWD()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA, ENABLE);  //?a??AFIO����?��
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); //???1JTAG1|?��
}

/**
  * @brief  ����ָ������
  * @retval None
  */
static void LED_GPIO_Configuration(void)
{
	int i;
    GPIO_InitTypeDef GPIO_InitStruct;
	
	for(i = 0; i < LED_NUM; i++) {
		RCC_APB2PeriphClockCmd(LED_GPIO[i].RCC_APB2Periph, ENABLE);
		//DIO
		GPIO_InitStruct.GPIO_Pin =  LED_GPIO[i].GPIO_Pin;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(LED_GPIO[i].GPIOx, &GPIO_InitStruct);
		GPIO_ResetBits(LED_GPIO[i].GPIOx, LED_GPIO[i].GPIO_Pin);
	}
}

void leds_init(void)
{
	DisableJtag_enableSWD();
	LED_GPIO_Configuration();
}

void led_on(int id)
{
	GPIO_ResetBits(LED_GPIO[id].GPIOx, LED_GPIO[id].GPIO_Pin);
}

void led_off(int id)
{
	GPIO_SetBits(LED_GPIO[id].GPIOx, LED_GPIO[id].GPIO_Pin);
}

void led_rev(int id)
{
	GPIO_WriteBit(LED_GPIO[id].GPIOx, LED_GPIO[id].GPIO_Pin, (BitAction)!GPIO_ReadOutputDataBit(LED_GPIO[id].GPIOx, LED_GPIO[id].GPIO_Pin) );
}

