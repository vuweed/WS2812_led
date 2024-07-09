/**
  ******************************************************************************
  * @file    stm32f1xx_it.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    11-February-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_it.h"
#include <include.h>
/** @addtogroup IO_Toggle
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint16_t capture = 0;
extern uint8_t g_brightness;
extern __IO uint16_t CCR1_Val;
extern __IO uint16_t CCR2_Val;
extern __IO uint16_t CCR3_Val;
extern __IO uint16_t CCR4_Val;


long map2(long x, long in_min, long in_max, long out_min, long out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M Processor Exceptions Handlers                          */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
//void SysTick_Handler(void)
//{}

/******************************************************************************/
/*            STM32F10x Peripherals Interrupt Handlers                        */
/******************************************************************************/

/**
  * @brief  This function handles TIM2 global interrupt request.
  * @param  None
  * @retval None
  */
uint8_t black_count = 0;
int blackout_val = 0;
void TIM2_IRQHandler(void)
{

  static int old_brightness = 0;
  if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)
  {
    // sound_value = analogRead(VR_PIN);
    for (int i = 0; i < 64; i++) //create a for loop to read
    { sound_value += analogRead(VR_PIN);  } //read the sound sensor

    sound_value >>= 6; //bitshift operation
//     Serial.println(sound_value); //print the value of sound sensor
    static uint32_t count_val = 0;


    blackout_val = analogRead(B1);
    black_count = map2(blackout_val, 0, 4095, 0, 200);
    if(STATE_1 == state_flag)
    {
      g_brightness = map2(sound_value, 0, 4095, 1, 255);
    }
    if (PROCESSING_1 == state_flag)
    {
      if(++count_val > black_count)
      {
        count_val = 0;
        state_flag = STATE_1;
      }
    }
    if(g_brightness >= 50)
    {

        g_brightness = 255;
        old_brightness = 255;
    }
    else if (g_brightness <= 5)
    {
        static int aging_counter = 0;
        aging_counter++;
        if(aging_counter >= 10)
        {
          aging_counter = 0;
          g_brightness = 0;
          old_brightness = 0;
          state_flag = PROCESSING_1;
        }
    }
    else
    {
      g_brightness = old_brightness;
//        g_brightness = map2(sound_value, 0, 4095, 1, 255);
    }
//    if(g_brightness < 50)
//    {
//        g_brightness = 0;
//    }
//    else if((g_brightness > 200) && (g_brightness <= 255))
//    {
//        g_brightness = 255;
//    }
//    else
//    {
//        g_brightness = 0;
//    }
    // if(sound_value < 1000)
    // {
    //   g_brightness = map2(sound_value, 0, 4096, 0, 255);
    // }
    // else
    // {
    //   g_brightness = 0;
    // }
      // if(false == toggle_all_led_flag)
      // {
      //     if(STATE_1 == state_flag)
      //     {
      //         sound_value = analogRead(VR_PIN);
      //     }

      // }
      // else
      // {
      //     sound_value = analogRead(VR_PIN);
      // }
      // if(sound_value < 700)
      // {
      //   //turn on led
      //   state_flag = PROCESSING_1;
      //   toggle_all_led_flag = false;

      // }
      // else
      // {
      //   toggle_all_led_flag = true;
      // }



     TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);

    // /* Pin PC.06 toggling with frequency = 73.24 Hz */
//     GPIO_WriteBit(GPIOC, GPIO_Pin_13, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_13)));
     capture = TIM_GetCapture1(TIM2);
     TIM_SetCompare1(TIM2, capture + CCR1_Val);
  }

}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_md.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
