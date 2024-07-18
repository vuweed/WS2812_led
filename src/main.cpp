/**
  ******************************************************************************
  * @file    TIM/DMA/main.c
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <math.h>
/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/** @addtogroup TIM_DMA
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TIM1_CCR3_Address    0x40012C3C

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;
uint16_t SRC_Buffer[3] = {0, 0, 0};
uint16_t TimerPeriod = 0;
#define MAX_LED 100
#define USE_BRIGHTNESS 0

#define PI 3.14159265


uint8_t LED_Data[MAX_LED][4];
uint8_t LED_Mod[MAX_LED][4];  // for brightness


void Set_Brightness (int brightness)  // 0-45
{
#if USE_BRIGHTNESS

    if (brightness > 45) brightness = 45;
    for (int i=0; i<MAX_LED; i++)
    {
        LED_Mod[i][0] = LED_Data[i][0];
        for (int j=1; j<4; j++)
        {
            float angle = 90-brightness;  // in degrees
            angle = angle*PI / 180;  // in rad
            LED_Mod[i][j] = (LED_Data[i][j])/(tan(angle));
        }
    }

#endif

}

// void led_move()
// {
//     int i = 0;
//     int R = 0, G = 0, B = 0;
//     for(i = 0; i < MAX_LED; i++)
//     {
//         Set_LED(i, R, G, B);
//         Set_Brightness(10);
//         WS2812_Send();
//         HAL_Delay(30);
//         if(R > 255)
//             R = 0;
//         if(G > 255)
//             G = 0;
//         if(B > 255)
//             B = 0;

//         R+=10;
//         G+=10;
//         B+=10;
//     }
// }
uint16_t pwmData[(24*MAX_LED)+50];
/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void DMA_Configuration(void);

/* Private functions ---------------------------------------------------------*/
void Set_LED (int LEDnum, int Red, int Green, int Blue)
{
    LED_Data[LEDnum][0] = LEDnum;
    LED_Data[LEDnum][1] = Green;
    LED_Data[LEDnum][2] = Red;
    LED_Data[LEDnum][3] = Blue;
}

void Wrap_buffer_led (void)
{
   uint32_t indx=0;
   uint32_t color;


   for (int i= 0; i<MAX_LED; i++)
   {
#if USE_BRIGHTNESS
       color = ((LED_Mod[i][1]<<16) | (LED_Mod[i][2]<<8) | (LED_Mod[i][3]));
#else
       color = ((LED_Data[i][1]<<16) | (LED_Data[i][2]<<8) | (LED_Data[i][3]));
#endif

       for (int i=23; i>=0; i--)
       {
           if (color&(1<<i))
           {
               pwmData[indx] = 60;  // 2/3 of 90
           }

           else pwmData[indx] = 30;  // 1/3 of 90

           indx++;
       }

   }

   for (int i=0; i<50; i++)
   {
       pwmData[indx] = 0;
       indx++;
   }

//    HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_1, (uint32_t *)pwmData, indx);
//    HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_2, (uint32_t *)pwmData, indx);
//    while (!datasentflag){};
//    datasentflag = 0;
}
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured,
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f10x_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f10x.c file
     */

  /* System Clocks Configuration */
  RCC_Configuration();

  /* GPIO Configuration */
  GPIO_Configuration();

  /* DMA Configuration */
  DMA_Configuration();

  /* TIM1 DMA Transfer example -------------------------------------------------
  TIM1CLK = SystemCoreClock, Prescaler = 0, TIM1 counter clock = SystemCoreClock
  SystemCoreClock is set to 72 MHz for Low-density, Medium-density, High-density
  and Connectivity line devices and to 24 MHz for Low-Density Value line and
  Medium-Density Value line devices.

  The objective is to configure TIM1 channel 3 to generate complementary PWM
  signal with a frequency equal to 17.57 KHz:
     - TIM1_Period = (SystemCoreClock / 17570) - 1
  and a variable duty cycle that is changed by the DMA after a specific number of
  Update DMA request.

  The number of this repetitive requests is defined by the TIM1 Repetition counter,
  each 3 Update Requests, the TIM1 Channel 3 Duty Cycle changes to the next new
  value defined by the SRC_Buffer .
  -----------------------------------------------------------------------------*/
  /* Compute the value to be set in ARR register to generate signal frequency at 17.57 Khz */
  TimerPeriod = (SystemCoreClock / 800000 ) - 1;
  /* Compute CCR1 value to generate a duty cycle at 50% */
  SRC_Buffer[0] = (uint16_t) (((uint32_t) 5 * (TimerPeriod - 1)) / 10);
  /* Compute CCR1 value to generate a duty cycle at 37.5% */
  SRC_Buffer[1] = (uint16_t) (((uint32_t) 375 * (TimerPeriod - 1)) / 1000);
  /* Compute CCR1 value to generate a duty cycle at 25% */
  SRC_Buffer[2] = (uint16_t) (((uint32_t) 25 * (TimerPeriod - 1)) / 100);

  /* TIM1 Peripheral Configuration --------------------------------------------*/
  /* Time Base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 2;

  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);



  /* Channel 3 Configuration in PWM mode */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = SRC_Buffer[0];
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
  //config led
  int i = 0;
    for(i = 0; i < MAX_LED; i++)
    {
        Set_LED(i, 255, 0, 0);
    }

    Wrap_buffer_led();
    TIM_OCInitStructure.TIM_Pulse = pwmData[0];
  TIM_OC3Init(TIM1, &TIM_OCInitStructure);



  /* TIM1 Update DMA Request enable */
  TIM_DMACmd(TIM1, TIM_DMA_Update, ENABLE);

  /* TIM1 counter enable */
  TIM_Cmd(TIM1, ENABLE);

  /* Main Output Enable */
  TIM_CtrlPWMOutputs(TIM1, ENABLE);

  while (1)
  {}
}

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void RCC_Configuration(void)
{
  /* TIM1, GPIOA and GPIOB clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 | RCC_APB2Periph_GPIOA |
                         RCC_APB2Periph_GPIOB, ENABLE);
  /* DMA clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
}

/**
  * @brief  Configure the TIM1 Pins.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* GPIOA Configuration: Channel 3 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* GPIOB Configuration: Channel 3N as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**
  * @brief  Configures the DMA.
  * @param  None
  * @retval None
  */
void DMA_Configuration(void)
{
  DMA_InitTypeDef DMA_InitStructure;

  /* DMA1 Channel5 Config */
  DMA_DeInit(DMA1_Channel5);

  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)TIM1_CCR3_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)pwmData;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = (24*MAX_LED)+50;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

  DMA_Init(DMA1_Channel5, &DMA_InitStructure);

  /* DMA1 Channel5 enable */
  DMA_Cmd(DMA1_Channel5, ENABLE);
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  while (1)
  {}
}

#endif

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
