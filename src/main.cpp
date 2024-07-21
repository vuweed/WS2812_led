#include "stm32f10x.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TIM1_DMAR_ADDRESS ((uint32_t)0x40012C34) /* TIM CCR1 (Capture/Compare Register 1) address */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
GPIO_InitTypeDef GPIO_InitStructure;
DMA_InitTypeDef DMA_InitStructure;
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef TIM_OCInitStructure;
#define MAX_LED 120
#define USE_BRIGHTNESS 0
#define DMA_BUF_SIZE (3 * ((24 * MAX_LED) + 50))
uint16_t pwmData[((24 * MAX_LED) + 50)];
// uint16_t SRC_Buffer[DMA_BUF_SIZE] = {0};
// #define MAX_LED 1
#define USE_BRIGHTNESS 0

#define PI 3.14159265

uint8_t LED_Data[MAX_LED][4];
uint8_t LED_Mod[MAX_LED][4]; // for brightness
int pulse_indx = 0;
int R = 0, G = 0, B = 0;
#define BUFFER_SIZE 12
uint16_t SRC_Buffer[BUFFER_SIZE] = {30, 60, 30, 90, 60, 30, 90, 0, 30, 90, 0, 30};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void Set_LED(int LEDnum, int Red, int Green, int Blue)
{
	LED_Data[LEDnum][0] = LEDnum;
	LED_Data[LEDnum][1] = Green;
	LED_Data[LEDnum][2] = Red;
	LED_Data[LEDnum][3] = Blue;
}

void Wrap_buffer_led(void)
{
	uint32_t indx = 0;
	uint32_t color;

	for (int i = 0; i < MAX_LED; i++)
	{
#if USE_BRIGHTNESS
		color = ((LED_Mod[i][1] << 16) | (LED_Mod[i][2] << 8) | (LED_Mod[i][3]));
#else
		color = ((LED_Data[i][1] << 16) | (LED_Data[i][2] << 8) | (LED_Data[i][3]));
#endif

		for (int i = 23; i >= 0; i--)
		{
			if (color & (1 << i))
			{
				pwmData[indx] = 60; // 2/3 of 90 = 1
			}

			else
				pwmData[indx] = 30; // 1/3 of 90 = 0

			indx++;
		}
	}

	for (int i = 0; i < 50; i++)
	{
		pwmData[indx] = 0;
		indx++;
	}

	// for(int i = 0; i < indx; i++)
	// {
	// 	SRC_Buffer[3*i] = TIM_TimeBaseStructure.TIM_Period; //90
	// 	SRC_Buffer[1 + 3*i] = 0;
	// 	SRC_Buffer[2 + 3*i] = pwmData[i];
	// }
	// SRC_Buffer[1] = 2;
	// // update 1st index
	// SRC_Buffer[2] = pwmData[0];
	//    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)pwmData;
	//    HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_1, (uint32_t *)pwmData, indx);
	//    HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_2, (uint32_t *)pwmData, indx);
	//    while (!datasentflag){};
	//    datasentflag = 0;
}



void refresh_strip()
{
		/* TIM1 DeInit */
		TIM_DeInit(TIM1);

		/* DMA1 Channel5 Config */
		DMA_DeInit(DMA1_Channel5);
		DMA_Init(DMA1_Channel5, &DMA_InitStructure);
		TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
		TIM_OC1Init(TIM1, &TIM_OCInitStructure);
		TIM_DMAConfig(TIM1, TIM_DMABase_CCR1, TIM_DMABurstLength_1Transfer);
		
		// for (int i = 0; i < MAX_LED; i++)
		// {
		// 	Set_LED(i, R, G, B);
		// }
		Wrap_buffer_led();
		/* TIM1 DMA Update enable */
		TIM_DMACmd(TIM1, TIM_DMA_Update, ENABLE);

		/* TIM1 enable */
		TIM_Cmd(TIM1, ENABLE);

		/* TIM1 PWM Outputs Enable */
		TIM_CtrlPWMOutputs(TIM1, ENABLE);

		/* DMA1 Channel5 enable */
		DMA_Cmd(DMA1_Channel5, ENABLE);

		/* Wait until DMA1 Channel5 end of Transfer */
		while (!DMA_GetFlagStatus(DMA1_FLAG_TC5))
		{
		}

		// Disable the timer and DMA after the condition is met
		TIM_CtrlPWMOutputs(TIM1, DISABLE);
		DMA_Cmd(DMA1_Channel5, DISABLE);
		TIM_Cmd(TIM1, DISABLE);
		TIM_DMACmd(TIM1, TIM_DMA_Update, DISABLE);
}

/**
 * @brief  Main program
 * @param  None
 * @retval None
 */
int main(void)
{
	/* TIM1 and GPIOA clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 | RCC_APB2Periph_GPIOA, ENABLE);

	/* DMA clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	/* GPIOA Configuration: Channel 1 as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//  /* TIM1 DeInit */
	//  TIM_DeInit(TIM1);
	//
	//  /* DMA1 Channel5 Config */
	//  DMA_DeInit(DMA1_Channel5);

	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)TIM1_DMAR_ADDRESS;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)pwmData;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = ((24 * MAX_LED) + 50);
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	//   DMA_Init(DMA1_Channel5, &DMA_InitStructure);

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 90;
	TIM_TimeBaseStructure.TIM_Prescaler = (uint16_t)(SystemCoreClock / 72000000) - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	//   TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	/* TIM Configuration in PWM Mode */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 90;
	//   TIM_OC1Init(TIM1, &TIM_OCInitStructure);

	/* TIM1 DMAR Base register and DMA Burst Length Config */
	//   TIM_DMAConfig(TIM1, TIM_DMABase_CCR1, TIM_DMABurstLength_1Transfer);

	/* Infinite loop */
	while (1)
	{
		static int cnt = 0;
		if(cnt >= MAX_LED)
		{
			cnt = 0;
			for (int i = 0; i < MAX_LED; i++)
			{
				Set_LED(i, 0 , 0, 0);
			}
			refresh_strip();
			for(int i = 0; i < 2000000; i++)
			{

			}
		}

		Set_LED(cnt, 255 , 0, 0);
		cnt++;
		__disable_irq;
		refresh_strip();
		__enable_irq;
		// for(int i = 0; i < 100; i++)
		// {

		// }
	}
}

#ifdef USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
	   ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	while (1)
	{
	}
}
#endif
