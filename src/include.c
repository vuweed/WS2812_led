/*
 * include.c
 *
 *  Created on: Aug 10, 2018
 *      Author: taivb
 */

#include "include.h"

uint32_t tim_cnt = 0;
uint32_t ticker;
volatile uint32_t us;

void SysTick_Init(void)
{
	if (SysTick_Config(SystemCoreClock / 1000)) //1ms per interrupt
	{
		while(1);
	}
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	NVIC_SetPriority(SysTick_IRQn, 0);
	ticker = 0;
}

uint32_t millis(void)
{
	return ticker;
}

uint32_t micros(void)
{
	/* us = ticker*1000+(SystemCoreClock/1000-SysTick->VAL)/80; */
	us = ticker * 1000 + 1000 - SysTick->VAL / 80;
	return us;
}

void delay(uint32_t nTime)
{
	uint32_t curTime = ticker;
	while ((nTime - (ticker - curTime)) > 0);
}

void delayMicroseconds(uint32_t nTime)
{
	uint32_t curTime = micros();
	while((nTime-(micros()-curTime)) > 0);
}

void SysTick_Handler(void)
{
	ticker++;
}

void randomSeed(unsigned long seed)
{
  if (seed != 0) {
    srandom(seed);
  }
}

long random1(long howbig)
{
  if (howbig == 0) {
    return 0;
  }
  return random() % howbig;
}

long random2(long howsmall, long howbig)
{
  if (howsmall >= howbig) {
    return howsmall;
  }
  long diff = howbig - howsmall;
  return random1(diff) + howsmall;
}

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/*
void Timer2Init()
{
	TIM_TimeBaseInitTypeDef TIM_InitStrucuture;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	// Enable Timer Clock 2
//	TIM_InitStrucuture.TIM_ClockDivision = 512;
	TIM_InitStrucuture.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_InitStrucuture.TIM_Period = 999; // Number of tick
	TIM_InitStrucuture.TIM_Prescaler = 71; // Frequency divide
//	TIM_InitStrucuture.TIM_RepetitionCounter = 5;
	TIM_TimeBaseInit(TIM2, &TIM_InitStrucuture);

	// Clear update flag
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);

	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

	TIM_Cmd(TIM2, ENABLE);

	// Config interrupt request for timer 2
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;

	NVIC_Init(&NVIC_InitStructure);
}

void TIM2_IRQHandler() {
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
		if (tim_cnt != 0x00) {
			tim_cnt--;
		}
		TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	}
}
*/
