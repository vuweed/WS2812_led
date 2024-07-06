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
extern __IO uint16_t CCR1_Val;
extern __IO uint16_t CCR2_Val;
extern __IO uint16_t CCR3_Val;
extern __IO uint16_t CCR4_Val;

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
void TIM2_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)
  {
//    if((STATE_1 != state_flag))
//    {
      static uint32_t count = 0;
      count++;
      if(count > 150)
      {
        count = 0;
        for (int i = 0; i < 20; ++i)
        {
          ports[i].clearAll();
        }
      }
      if(false == toggle_all_led_flag)
      {
          if(STATE_1 == state_flag)
          {
              sound_value = analogRead(VR_PIN);
          }

      }
      else
      {
          sound_value = analogRead(VR_PIN);
      }
      if(sound_value < 700)
      {
        //turn on led
        state_flag = PROCESSING_1;
        toggle_all_led_flag = false;

      }
      else
      {
//        state_flag = PROCESSING_1;
        toggle_all_led_flag = true;
      }

//    }


    // if(state_flag == STATE_1)
    // {
    //     sound_value = analogRead(VR_PIN);
    //     if(sound_value < 700)
    //     {
    //         state_flag = PROCESSING_1;
    //         toggle_all_led_flag = false;

    //     }
    //     else
    //     {
    //       state_flag = PROCESSING_2;
    //       toggle_all_led_flag = true;
    //     }
    // }
    // else if(state_flag == STATE_2)
    // {
    //     sound_value = analogRead(VR_PIN);
    //     if(sound_value < 700)
    //     {
    //         state_flag = PROCESSING_1;
    //         toggle_all_led_flag = false;

    //     }
    //     else
    //     {
    //       state_flag = PROCESSING_2;
    //       toggle_all_led_flag = true;
    //     }
    // }

     TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);

    // /* Pin PC.06 toggling with frequency = 73.24 Hz */
//     GPIO_WriteBit(GPIOC, GPIO_Pin_13, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_13)));
     capture = TIM_GetCapture1(TIM2);
     TIM_SetCompare1(TIM2, capture + CCR1_Val);
  }
  // else if (TIM_GetITStatus(TIM2, TIM_IT_CC2) != RESET)
  // {
  //   // TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);

  //   // /* Pin PC.07 toggling with frequency = 109.8 Hz */
  //   // GPIO_WriteBit(GPIOC, GPIO_Pin_7, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_7)));
  //   // capture = TIM_GetCapture2(TIM2);
  //   // TIM_SetCompare2(TIM2, capture + CCR2_Val);
  // }
  // else if (TIM_GetITStatus(TIM2, TIM_IT_CC3) != RESET)
  // {
  //   // TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);

  //   // /* Pin PC.08 toggling with frequency = 219.7 Hz */
  //   // GPIO_WriteBit(GPIOC, GPIO_Pin_8, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_8)));
  //   // capture = TIM_GetCapture3(TIM2);
  //   // TIM_SetCompare3(TIM2, capture + CCR3_Val);
  // }
  // else
  // {
  //   // TIM_ClearITPendingBit(TIM2, TIM_IT_CC4);

  //   // /* Pin PC.09 toggling with frequency = 439.4 Hz */
  //   // GPIO_WriteBit(GPIOC, GPIO_Pin_9, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_9)));
  //   // capture = TIM_GetCapture4(TIM2);
  //   // TIM_SetCompare4(TIM2, capture + CCR4_Val);
  // }
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
