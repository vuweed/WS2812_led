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
uint16_t capture = 0;
uint8_t black_count = 0;
int blackout_val = 0;
int user_brightness = 0;
int aging_counter = 0;
extern bool button_state;
extern uint8_t g_brightness;
extern __IO uint16_t CCR1_Val;
extern __IO uint16_t CCR2_Val;
extern __IO uint16_t CCR3_Val;
extern __IO uint16_t CCR4_Val;

long map2(long x, long in_min, long in_max, long out_min, long out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
int aging_counter_2 = 0;
void SysTick_Handler(void)
{
	ticker++;
	static uint8_t sys_count = 0;
	sys_count++;
	if (sys_count > 50)
	{
	    sys_count = 0;
	    // if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)
	      // {
	          for (int i = 0; i < 64; i++) // create a for loop to read
	          {
	              sound_value += analogRead(SOUND_SENSOR);
	          } // read the sound sensor

	          sound_value >>= 6; // bitshift operation
	          //     Serial.println(sound_value); //print the value of sound sensor

	          // mode selection using Blank_out Vr
	          blackout_val = map2(analogRead(BLANK_OUT_VR), 0, 4095, 0, 10);

	          if(0 == blackout_val)
	          {
	              aging_counter_2++;
	              if (aging_counter_2 > 100)
	              {
	                  aging_counter_2 = 0;
	                  button_state = false;
	              }
	          }
	          else
	          {
	              aging_counter_2 = 0;
	              button_state = true;
	          }

	          // 1st mode - react with sound
	          if (button_state == false)
	          {
	              static int old_brightness = 0;
	              g_brightness = map2(sound_value, 0, 4095, 1, 255);
	              if (g_brightness >= 50)
	              {
	                  g_brightness = 255;
	                  old_brightness = 255;
	              }
	              else if (g_brightness <= 10)
	              {
	                  g_brightness = 0;
	                  old_brightness = 20;
	              }
	              else
	              {
	                  g_brightness = old_brightness;
	                  //        g_brightness = map2(sound_value, 0, 4095, 1, 255);
	              }
	          }
	          else // 2nd mode - running with a delay Vr
	          {
	  //            if (STATE_1 == state_flag)
	  //            {
	                  g_brightness = map2(sound_value, 0, 4095, 1, 255);
	                  if (g_brightness >= 5)
	                  {
	                      aging_counter = 0;
	                      user_brightness = 255;
	                  }
	                  else if (g_brightness <= 5)
	                  {
	                      blackout_val = map2(analogRead(BLANK_OUT_VR), 0, 4095, 0, 10);
	                      aging_counter++;
	                      if (aging_counter >= blackout_val * 100)
	                      {
	                          aging_counter = 0;
	                          user_brightness = 0;
	                          state_flag = PROCESSING_1;
	                      }
	                  }
	  //            }
	          }

	          // TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);

	          // // /* Pin PC.06 toggling with frequency = 73.24 Hz */
	          // capture = TIM_GetCapture1(TIM2);
	          // TIM_SetCompare1(TIM2, capture + CCR1_Val);
	      // }

	}

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
