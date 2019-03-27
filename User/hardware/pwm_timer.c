/*
 * hxdyxd@gmail.com, 2018
 */

#include "pwm_timer.h"

/*
 * 4通道PWM输出
 */
void pwm_4ch_timer_init(void)
{
	GPIO_InitTypeDef          GPIO_InitStructure;
	TIM_OCInitTypeDef         TIM_OCInitStructure;
	TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
    //输入半桥工作在PWM模式
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    //输入left posi
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;  //TIM3_CH1
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    //输入left neg
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;  //TIM3_CH2
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    //输出right posi
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;  //TIM3_CH3
	GPIO_Init(GPIOB, &GPIO_InitStructure);
    //输出right neg
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;  //TIM3_CH4
	GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    
	TIM_TimeBaseStructure.TIM_Prescaler = 1 - 1;
	TIM_TimeBaseStructure.TIM_Period = PERIOD - 1;  //7200, 10k
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	/*
	 *	PWM1, 计数值小于CCR为有效
	 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	//TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	
	//有效电平极性配置
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	//TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	
	//空闲电平极性配置, Only TIM1 & TIM8
	//TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	//TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
    
    TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);   //CCR值不立即生效
	
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
	TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
}

void pwm_timer_set(uint8_t id, uint16_t ccr)
{
	static __IO uint16_t * const TIMx_CCRn[4] = {
		&(TIM3->CCR1), 
		&(TIM3->CCR2), 
		&(TIM3->CCR3), 
		&(TIM3->CCR4),
	};
	*TIMx_CCRn[id] = ccr;
}



/******************* (C) COPYRIGHT 2019 03 25 hxdyxd *****END OF FILE****/
