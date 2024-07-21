/*
 * GPIO.c
 *
 *  Created on: Aug 8, 2018
 *      Author: taivb
 */



#include "../include.h"
#include "GPIO.h"

GPIO_InitTypeDef GPIO_InitStructure;
DMA_InitTypeDef DMA_InitStructure;
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef TIM_OCInitStructure;
/*
 * get GPIO function
 * return GPIO_TypeDef from GPIO_PINx
 * GPIO_PINx: A0-C15
 */
GPIO_TypeDef *getDigitalGPIO(uint8_t GPIO_PINx)
{
	return (GPIO_TypeDef *)((GPIO_PINx / 16)*0x0400 + 0x40010800);
}

/*
 * get GPIO function
 * return Pin number from GPIO_PINx
 * GPIO_PINx: A0-C15
 */
uint8_t getAnalogPin(uint8_t GPIO_PINx)
{
	if (GPIO_PINx == 16)
	{
		return 0;
	}
	else if (GPIO_PINx == 17)
	{
		return 1;
	}
	return GPIO_PINx;
}

/*
 * get GPIO function
 * return Pin number from GPIO_PINx
 * GPIO_PINx: A0-C15
 */
uint8_t getDigitalPin(uint8_t GPIO_PINx)
{
	return GPIO_PINx % 16;
}

/*
 * pin configuration function
 * mode: INPUT, OUT_PUT, INPUT_PULLUP, INPUT_PULLDOWN
 * GPIO_PINx: A0 - C15
 */
void pinMode(uint8_t GPIO_PINx, uint8_t mode)
{
	GPIO_TypeDef* GPIOx = getDigitalGPIO(GPIO_PINx);
	uint8_t PINx = getDigitalPin(GPIO_PINx);

	GPIO_InitTypeDef  GPIO_InitStructure;

	/* Enable the GPIO_AHPclock Clock */
	if(GPIOx == GPIOA)	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	else if(GPIOx == GPIOB)	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	else if(GPIOx == GPIOC)	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	else if(GPIOx == GPIOD)	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	else if(GPIOx == GPIOE)	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	else if(GPIOx == GPIOF)	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);

	/* Configure the GPIO PINx */
	GPIO_InitStructure.GPIO_Mode = (GPIOMode_TypeDef)mode;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;

	GPIO_InitStructure.GPIO_Pin = (1<<PINx);
	GPIO_Init(GPIOx, &GPIO_InitStructure);
}// end pinMode function


/*
 * write state on pin function
 * GPIO_PINx: A0 - C15
 * val = HIGH(1), LOW(0)
 */
void digitalWrite(uint8_t GPIO_PINx, uint8_t val)
{
	GPIO_TypeDef* GPIOx = getDigitalGPIO(GPIO_PINx);
	uint8_t PINx = getDigitalPin(GPIO_PINx);

	if (val == HIGH)	GPIOx->BSRR = (1<<PINx);
	else	GPIOx->BRR = (1<<PINx);
}

/*
 * digital toggle pin function
 * toggle a pin 0 to 1 or 1 to 0
 * GPIO_PINx: A0-C15
 */
void digitalToggle(uint8_t GPIO_PINx)
{
	GPIO_TypeDef* GPIOx = getDigitalGPIO(GPIO_PINx);
	uint8_t PINx = getDigitalPin(GPIO_PINx);
	GPIOx->ODR ^= (1<<PINx);
}

/*
 * digital read function
 * read pin state
 * GPIO_PINx: A0-C15
 */
uint8_t digitalRead(uint8_t GPIO_PINx)
{
	GPIO_TypeDef* GPIOx = getDigitalGPIO(GPIO_PINx);
	uint8_t PINx = getDigitalPin(GPIO_PINx);

	uint8_t bitstatus = 0x00;

	/* Check the parameters */
	assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
	assert_param(IS_GET_());

	if ((GPIOx->IDR & (1<<PINx)) != (uint32_t)Bit_RESET)  bitstatus = (uint8_t)Bit_SET;
	else   bitstatus = (uint8_t)Bit_RESET;
	return bitstatus;
}

/*
 * analog initialization
 * initialize pin analog mode
 * channel 0 ~ A0
 * channel 1 ~ A1
 * channel 2 ~ A2
 * channel 3 ~ A3
 * channel 4 ~ A4
 * channel 5 ~ A5
 * channel 6 ~ A6
 * channel 7 ~ A7
 * channel 8 ~ B0
 * channel 9 ~ B1
 */
void analogEnable(uint8_t ADC_PINx)
{
	GPIO_TypeDef* GPIOx = getDigitalGPIO(ADC_PINx);
	uint8_t PINx = getAnalogPin(ADC_PINx);

	ADC_InitTypeDef ADC_InitStructure;// Reset ADC init structure parameters values
	GPIO_InitTypeDef GPIO_InitStructure;

	// Enable clock for ADC1 and maybe GPIOA
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	/* set parameter for GPIO_ADC */
	GPIO_InitStructure.GPIO_Pin = 1 << PINx;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOx, &GPIO_InitStructure);

	/* Initialize the ADC_Mode member */
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	/* initialize the ADC_ScanConvMode member */
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	/* Initialize the ADC_ContinuousConvMode member */
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	/* Initialize the ADC_ExternalTrigConv member */
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	/* Initialize the ADC_DataAlign member */
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	/* Initialize the ADC_NbrOfChannel member */
	ADC_InitStructure.ADC_NbrOfChannel = 1;

	ADC_Init(ADC1, &ADC_InitStructure);

	/* Enable ADC1*/
	ADC_Cmd(ADC1, ENABLE);
	/* Enable temperature sensor */
	ADC_TempSensorVrefintCmd(ENABLE);
	/* Reset thanh ghi Calib */
	ADC_ResetCalibration(ADC1);

	/* Wait for calib status on */
	while(ADC_GetResetCalibrationStatus(ADC1));

	ADC_StartCalibration(ADC1);

	while(ADC_GetCalibrationStatus(ADC1));
	/* Start conversion */
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/*
 * analog disable
 *
 */
void analogDisable()
{
	ADC_DeInit(ADC1);
}
/*
 * analog Read
 * Read analog value of one specificate pin
 * Sample time = 55.5 us
 * channel 0 ~ A0
 * channel 1 ~ A1
 * channel 2 ~ A2
 * channel 3 ~ A3
 * channel 4 ~ A4
 * channel 5 ~ A5
 * channel 6 ~ A6
 * channel 7 ~ A7
 * channel 8 ~ B0
 * channel 9 ~ B1
 */
uint16_t analogRead(uint8_t GPIO_PINx)
{
	/* Set ADC channel number */
	if (GPIO_PINx == 16)
	{
		ADC_RegularChannelConfig(ADC1, 8, 1,ADC_SampleTime_55Cycles5);
		ADC_RegularChannelConfig(ADC1, 8, 1,ADC_SampleTime_55Cycles5);
	}
	else if (GPIO_PINx == 17)
	{
		ADC_RegularChannelConfig(ADC1, 9, 1,ADC_SampleTime_55Cycles5);
		ADC_RegularChannelConfig(ADC1, 9, 1,ADC_SampleTime_55Cycles5);
	}
	else
	{
		ADC_RegularChannelConfig(ADC1, GPIO_PINx, 1,ADC_SampleTime_55Cycles5);
		ADC_RegularChannelConfig(ADC1, GPIO_PINx, 1,ADC_SampleTime_55Cycles5);
	}
	/* Check the parameters */
	assert_param(IS_ADC_ALL_PERIPH(ADC1));
	/* Return the selected ADC conversion value */
	return (uint16_t) ADC1->DR;
}

/*
 * PWM write out function
 * frequency output = 18 KHz
 * max PWMvalue = 40000
 *
 * don't care about Timer 1, it very flexible
 *
	PA0 Timer2_Chanel 1		PA6 Timer3_Chanel 1		PB6 Timer4_Chanel 1
	PA1 Timer2_Chanel 2		PA7 Timer3_Chanel 2		PB7 Timer4_Chanel 2
	PA2 Timer2_Chanel 3		PB0 Timer3_Chanel 3		PB8 Timer4_Chanel 3
	PA3 Timer2_Chanel 4		PB1 Timer3_Chanel 4		PB9 Timer4_Chanel 4
 */
void analogWrite(uint8_t GPIO_PINx, uint16_t val)
{
	GPIO_TypeDef* GPIOx = getDigitalGPIO(GPIO_PINx);
	uint8_t PINx = getDigitalPin(GPIO_PINx);

	TIM_TypeDef *TIMx;
	uint16_t channel = 0;
	/* get Timer & channel from PINx */
	switch ((uint32_t)GPIOx)
	{
		case GPIOA_BASE:
			switch (PINx)
			{
				case 0:
					TIMx = TIM2;
					channel = 1;
					break;

				case 1:
					TIMx = TIM2;
					channel = 2;
					break;

				case 2:
					TIMx = TIM2;
					channel = 3;
					break;

				case 3:
					TIMx = TIM2;
					channel = 4;
					break;

				case 6:
					TIMx = TIM3;
					channel = 1;
					break;

				case 7:
					TIMx = TIM3;
					channel = 2;
					break;
				default:
					break;
			}
			break;

			case GPIOB_BASE:
				switch (PINx)
				{
					case 0:
						TIMx = TIM3;
						channel = 3;
						break;

					case 1:
						TIMx = TIM3;
						channel = 4;
						break;

					case 6:
						TIMx = TIM4;
						channel = 1;
						break;

					case 7:
						TIMx = TIM4;
						channel = 2;
						break;

					case 8:
						TIMx = TIM4;
						channel = 3;
						break;

					case 9:
						TIMx = TIM4;
						channel = 4;
						break;
				}
				break;
		default:
			break;
	}

	/* TIM_SetComparex */
	/* Check the parameters */
	assert_param(IS_TIM_LIST6_PERIPH(TIMx));
	switch (channel)
	{
		case 1:
			/* Set the Capture Compare1 Register value */
			TIMx->CCR1 = val;
			break;

		case 2:
			/* Set the Capture Compare2 Register value */
			TIMx->CCR2 = val;
			break;

		case 3:
			/* Set the Capture Compare3 Register value */
			TIMx->CCR3 = val;
			break;

		case 4:
			/* Set the Capture Compare4 Register value */
			TIMx->CCR4 = val;
			break;
		default:
			break;
	}
}
/*
 * PWM initialize function
 * frequency output = 18 KHz
 * max PWMvalue = 40000
 *
 * don't care about Timer 1, it very flexible
 *
	PA0 Timer2_Chanel 1		PA6 Timer3_Chanel 1		PB6 Timer4_Chanel 1
	PA1 Timer2_Chanel 2		PA7 Timer3_Chanel 2		PB7 Timer4_Chanel 2
	PA2 Timer2_Chanel 3		PB0 Timer3_Chanel 3		PB8 Timer4_Chanel 3
	PA3 Timer2_Chanel 4		PB1 Timer3_Chanel 4		PB9 Timer4_Chanel 4
 */
void analogInit(uint8_t GPIO_PINx)
{

	GPIO_TypeDef* GPIOx = getDigitalGPIO(GPIO_PINx);
	uint8_t PINx = getDigitalPin(GPIO_PINx);

	TIM_TimeBaseInitTypeDef TIM_InitStrucuture;
	TIM_OCInitTypeDef	TIM_OCInitStructure;

	TIM_TypeDef *TIMx;
	uint16_t channel = 0;
	/* get Timer & channel from PINx	*/
	switch ((uint32_t)GPIOx)
	{
		case GPIOA_BASE:
			switch (PINx)
			{
				case 0:
					TIMx = TIM2;
					channel = 1;
					break;

				case 1:
					TIMx = TIM2;
					channel = 2;
					break;

				case 2:
					TIMx = TIM2;
					channel = 3;
					break;

				case 3:
					TIMx = TIM2;
					channel = 4;
					break;

				case 6:
					TIMx = TIM3;
					channel = 1;
					break;

				case 7:
					TIMx = TIM3;
					channel = 2;
					break;
				default:
					break;
			}
			break;

			case GPIOB_BASE:
				switch (PINx)
				{
					case 0:
						TIMx = TIM3;
						channel = 3;
						break;

					case 1:
						TIMx = TIM3;
						channel = 4;
						break;

					case 6:
						TIMx = TIM4;
						channel = 1;
						break;

					case 7:
						TIMx = TIM4;
						channel = 2;
						break;

					case 8:
						TIMx = TIM4;
						channel = 3;
						break;

					case 9:
						TIMx = TIM4;
						channel = 4;
						break;
				}
				break;
		default:
			break;
	}

	switch ((uint32_t) TIMx)
		{
			case TIM1_BASE:
				//RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM1, ENABLE);
				break;

			case TIM2_BASE:
				RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
				break;

			case TIM3_BASE:
				RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
				break;

			case TIM4_BASE:
				RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
				break;

			default:
				break;
		}


	/*Cau hinh ch�n*/
	pinMode(GPIO_PINx, AF_PP);

	/* Enable Timer Clock 4 */
	TIM_InitStrucuture.TIM_ClockDivision = 0;
	TIM_InitStrucuture.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_InitStrucuture.TIM_Period = 45;// Number of tick

	/* f_clock = f_system / prescaler */
	/* motor driver need maximum f_pwm = (f_system/prescaler) / period */
	/* eg. period = 4000, prescaler = f_system/(f_pwm*period) = 72MHz/(18KHz*4000) = */
	// in that case is 72MHz / 3600 = 20KHz
	TIM_InitStrucuture.TIM_Prescaler = 1;// Frequency divide
	TIM_InitStrucuture.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIMx, &TIM_InitStrucuture);


	/* cau hinh channel4 o mode PWM */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 1000;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
/*
 * NOTE:
 *
 *  .pulse < period
 *
 *  if TIM_OCPolarity_Low: pulse is the time that pwm = 1
 *  else if TIM_OCPolarity_High:  pulse is the time that pwm = 0
 */
	switch (channel)
	{
		case 1:
			TIM_OC1Init(TIMx, &TIM_OCInitStructure);
			TIM_OC1PreloadConfig(TIMx, TIM_OCPreload_Enable);
			break;

		case 2:
			TIM_OC2Init(TIMx, &TIM_OCInitStructure);
			TIM_OC2PreloadConfig(TIMx, TIM_OCPreload_Enable);
			break;

		case 3:
			TIM_OC3Init(TIMx, &TIM_OCInitStructure);
			TIM_OC3PreloadConfig(TIMx, TIM_OCPreload_Enable);
			break;

		case 4:
			TIM_OC4Init(TIMx, &TIM_OCInitStructure);
			TIM_OC4PreloadConfig(TIMx, TIM_OCPreload_Enable);
			break;

		default:
			break;
	}
	TIM_ARRPreloadConfig(TIMx, ENABLE);

	/* Enable timer 4 */
	TIM_Cmd(TIMx, ENABLE);
}

/*
 * analog read with DMA for 8 channel
 */
void ADC_DMA_Init(uint16_t ADC_values[], uint8_t len)
{
	DMA_InitTypeDef DMA_InitStructure;
	/* Enable peripheral clocks ------------------------------------------------*/
	/* Enable DMA1 clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div6); //clock for ADC (max 14MHz, 72/6=12MHz)
	/* cau hinh DMA1 channel 1*/
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;// ADC address
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC_values;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = len;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	/* cho phep DMA1 channel 1 hoat dong */
	DMA_Cmd(DMA1_Channel1, ENABLE);

	ADC_InitTypeDef ADC_InitStructure;/* Reset ADC init structure parameters values */
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable clock for ADC1 and maybe GPIOA */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	/* set parameter for GPIO_ADC */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 |GPIO_Pin_3 |  GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Initialize the ADC_Mode member */
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	/* initialize the ADC_ScanConvMode member */
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	/* Initialize the ADC_ContinuousConvMode member */
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	/* Initialize the ADC_ExternalTrigConv member */
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	/* Initialize the ADC_DataAlign member */
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	/* Initialize the ADC_NbrOfChannel member */
	ADC_InitStructure.ADC_NbrOfChannel = len;
	ADC_Init(ADC1, &ADC_InitStructure);

	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1,ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2,ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3,ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4,ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 5,ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 6,ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 7,ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 8,ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 9,ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 10,ADC_SampleTime_55Cycles5);

	/* Enable ADC_DMA */
	ADC_DMACmd(ADC1, ENABLE);
	/* Enable ADC1*/
	ADC_Cmd(ADC1, ENABLE);

	/* Reset thanh ghi Calib */
	ADC_ResetCalibration(ADC1);

	/* Wait for calibration status on */
	while(ADC_GetResetCalibrationStatus(ADC1));

	ADC_StartCalibration(ADC1);

	while(ADC_GetCalibrationStatus(ADC1));
	/* Start conversion */
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
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
		// Wrap_buffer_led();
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