/*
 * interrupt.c
 *
 *  Created on: Aug 16, 2018
 *      Author: ASUS PC
 */

#include <include.h>
#include "stm32f1xx_it.h"
#include "Interrupt.h"

static volatile voidFuncPtr intFunc[16];

/*
 * attach external interrupt
 * mode: RISING, FALLING, FALLING_RISING
 *
 */
void attachInterrupt(uint8_t GPIO_PINx, uint8_t mode, void (*userFunc)(void))
{

	GPIO_TypeDef* GPIOx = getDigitalGPIO(GPIO_PINx);
	uint8_t PINx = getDigitalPin(GPIO_PINx);

	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable the GPIO_AHPclock Clock */
	if(GPIOx == GPIOA)	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	else if(GPIOx == GPIOB)	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	else if(GPIOx == GPIOC)	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	/* GPIO init */
	GPIO_InitStructure.GPIO_Pin = (1<<PINx);
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOx, &GPIO_InitStructure);

	/* mapping */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);// enable AFIO before config portsource
	if(GPIOx == GPIOA)	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, PINx);
	else if(GPIOx == GPIOB)	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, PINx);
	else if(GPIOx == GPIOC)	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, PINx);


	/* Clear the the EXTI line interrupt pending bit */
	EXTI_ClearITPendingBit(1<<PINx);
	/* EXTI line Configuration */
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Line = (1<<PINx);
	EXTI_InitStructure.EXTI_Trigger = (EXTITrigger_TypeDef) mode;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/*NVIC Configuration*/
	if (4 < PINx && PINx < 10)
	{
		NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	}
	else if (9 < PINx && PINx < 16)
	{
		NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	}
	else
	{
		NVIC_InitStructure.NVIC_IRQChannel = PINx + 6;
	}
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	intFunc[PINx] = userFunc;
}

/**
  * @brief  This function handles EXTI0 interrupt.
  * @param  None
  * @retval None
  */
void EXTI0_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line0) != RESET)
	{
		/* Clear the EXTI line pending bit */
		EXTI_ClearITPendingBit(EXTI_Line0);
		intFunc[0]();
	}
}

/**
  * @brief  This function handles EXTI1 interrupt.
  * @param  None
  * @retval None
  */
void EXTI1_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line1) != RESET)
	{
		/* Clear the EXTI line pending bit */
		EXTI_ClearITPendingBit(EXTI_Line1);
		intFunc[1]();
	}
}
/**
  * @brief  This function handles EXTI2 interrupt.
  * @param  None
  * @retval None
  */
void EXTI2_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line2) != RESET)
	{
		/* Clear the EXTI line pending bit */
		EXTI_ClearITPendingBit(EXTI_Line2);
		intFunc[2]();
	}
}
/**
  * @brief  This function handles EXTI3 interrupt.
  * @param  None
  * @retval None
  */
void EXTI3_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line3) != RESET)
	{
		/* Clear the EXTI line pending bit */
		EXTI_ClearITPendingBit(EXTI_Line3);
		intFunc[3]();
	}
}
/**
  * @brief  This function handles EXTI4 interrupt.
  * @param  None
  * @retval None
  */
void EXTI4_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line4) != RESET)
	{
		/* Clear the EXTI line pending bit */
		EXTI_ClearITPendingBit(EXTI_Line4);
		intFunc[4]();
	}
}

/**
  * @brief  This function handles EXTI4 interrupt.
  * @param  None
  * @retval None
  */
void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line10) != RESET)
	{
		/* Clear the EXTI line pending bit */
		EXTI_ClearITPendingBit(EXTI_Line10);
		intFunc[10]();
	}
	if(EXTI_GetITStatus(EXTI_Line11) != RESET)
	{
		/* Clear the EXTI line pending bit */
		EXTI_ClearITPendingBit(EXTI_Line11);
		intFunc[11]();
	}
	if(EXTI_GetITStatus(EXTI_Line12) != RESET)
	{
		/* Clear the EXTI line pending bit */
		EXTI_ClearITPendingBit(EXTI_Line12);
		intFunc[12]();
	}
	if(EXTI_GetITStatus(EXTI_Line13) != RESET)
	{
		/* Clear the EXTI line pending bit */
		EXTI_ClearITPendingBit(EXTI_Line13);
		intFunc[13]();
	}
	if(EXTI_GetITStatus(EXTI_Line14) != RESET)
	{
		/* Clear the EXTI line pending bit */
		EXTI_ClearITPendingBit(EXTI_Line14);
		intFunc[14]();
	}
	if(EXTI_GetITStatus(EXTI_Line15) != RESET)
	{
		/* Clear the EXTI line pending bit */
		EXTI_ClearITPendingBit(EXTI_Line15);
		intFunc[15]();
	}
}

/**
  * @brief  This function handles EXTI9-5 interrupt.
  * @param  None
  * @retval None
  */

void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line5) != RESET)
	{
		// Clear the EXTI line pending bit
		EXTI_ClearITPendingBit(EXTI_Line5);
		intFunc[5]();
	}
	if(EXTI_GetITStatus(EXTI_Line6) != RESET)
	{
		// Clear the EXTI line pending bit
		EXTI_ClearITPendingBit(EXTI_Line6);
		intFunc[6]();
	}
	if(EXTI_GetITStatus(EXTI_Line7) != RESET)
	{
		// Clear the EXTI line pending bit
		EXTI_ClearITPendingBit(EXTI_Line7);
		intFunc[7]();
	}
	if(EXTI_GetITStatus(EXTI_Line8) != RESET)
	{
		// Clear the EXTI line pending bit
		EXTI_ClearITPendingBit(EXTI_Line8);
		intFunc[8]();
	}
	if(EXTI_GetITStatus(EXTI_Line9) != RESET)
	{
		// Clear the EXTI line pending bit
		EXTI_ClearITPendingBit(EXTI_Line9);
		intFunc[9]();
	}
}
