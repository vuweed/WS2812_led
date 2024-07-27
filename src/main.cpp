#include "stm32f10x.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* TIM1 CCR addresses */
#define TIM1_CCR1_ADDRESS ((uint32_t)0x40012C34) /* TIM1 CCR1 (Capture/Compare Register 1) address */
#define TIM1_CCR2_ADDRESS ((uint32_t)0x40012C38) /* TIM1 CCR2 (Capture/Compare Register 2) address */
#define TIM1_CCR3_ADDRESS ((uint32_t)0x40012C3C) /* TIM1 CCR3 (Capture/Compare Register 3) address */
#define TIM1_CCR4_ADDRESS ((uint32_t)0x40012C40) /* TIM1 CCR4 (Capture/Compare Register 4) address */

/* TIM2 CCR addresses */
#define TIM2_CCR1_ADDRESS ((uint32_t)0x40000034) /* TIM2 CCR1 (Capture/Compare Register 1) address */
#define TIM2_CCR2_ADDRESS ((uint32_t)0x40000038) /* TIM2 CCR2 (Capture/Compare Register 2) address */
#define TIM2_CCR3_ADDRESS ((uint32_t)0x4000003C) /* TIM2 CCR3 (Capture/Compare Register 3) address */
#define TIM2_CCR4_ADDRESS ((uint32_t)0x40000040) /* TIM2 CCR4 (Capture/Compare Register 4) address */

/* TIM3 CCR addresses */
#define TIM3_CCR1_ADDRESS ((uint32_t)0x40000434) /* TIM3 CCR1 (Capture/Compare Register 1) address */
#define TIM3_CCR2_ADDRESS ((uint32_t)0x40000438) /* TIM3 CCR2 (Capture/Compare Register 2) address */
#define TIM3_CCR3_ADDRESS ((uint32_t)0x4000043C) /* TIM3 CCR3 (Capture/Compare Register 3) address */
#define TIM3_CCR4_ADDRESS ((uint32_t)0x40000440) /* TIM3 CCR4 (Capture/Compare Register 4) address */

/* TIM4 CCR addresses */
#define TIM4_CCR1_ADDRESS ((uint32_t)0x40000834) /* TIM4 CCR1 (Capture/Compare Register 1) address */
#define TIM4_CCR2_ADDRESS ((uint32_t)0x40000838) /* TIM4 CCR2 (Capture/Compare Register 2) address */
#define TIM4_CCR3_ADDRESS ((uint32_t)0x4000083C) /* TIM4 CCR3 (Capture/Compare Register 3) address */
#define TIM4_CCR4_ADDRESS ((uint32_t)0x40000840) /* TIM4 CCR4 (Capture/Compare Register 4) address */

typedef struct
{
    uint32_t TIMx_CCR_x_ADDRESS;
    uint32_t RCC_APBxPeriph_TIM;
    uint32_t RCC_APBxPeriph_GPIO;
    uint16_t GPIO_Pin;
    uint8_t TIM_Ch;
    GPIO_TypeDef* GPIOx;
    TIM_TypeDef* TIMx;
    DMA_Channel_TypeDef* DMAy_Channelx;
    uint16_t TIM_DMABase;
    uint32_t DMAy_FLAG;
    /* data */
} WS2812_config_t;

WS2812_config_t ws2812_struct_template[4] = {
    {TIM3_CCR2_ADDRESS, RCC_APB1Periph_TIM3, RCC_APB2Periph_GPIOA, GPIO_Pin_7, 2, GPIOA, TIM3, DMA1_Channel3, TIM_DMABase_CCR2, DMA1_FLAG_TC3}, // PA7
    {TIM2_CCR1_ADDRESS, RCC_APB1Periph_TIM2, RCC_APB2Periph_GPIOA, GPIO_Pin_0, 1, GPIOA, TIM2, DMA1_Channel2, TIM_DMABase_CCR1, DMA1_FLAG_TC2}, // PA0
    {TIM1_CCR1_ADDRESS, RCC_APB2Periph_TIM1, RCC_APB2Periph_GPIOA, GPIO_Pin_8, 1, GPIOA, TIM1, DMA1_Channel5, TIM_DMABase_CCR1, DMA1_FLAG_TC5}, // PA8
    {TIM1_CCR2_ADDRESS, RCC_APB2Periph_TIM1, RCC_APB2Periph_GPIOA, GPIO_Pin_9, 2, GPIOA, TIM1, DMA1_Channel5, TIM_DMABase_CCR2, DMA1_FLAG_TC5}  // PA9
};

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
}

void refresh_strip(WS2812_config_t* ws2812_struct_template)
{

    /* TIM3 and GPIOA clock enable */
    if (RCC_APB2Periph_TIM1 == ws2812_struct_template->RCC_APBxPeriph_TIM)
    {
        RCC_APB2PeriphClockCmd(ws2812_struct_template->RCC_APBxPeriph_TIM, ENABLE);
    }
    else
    {
        RCC_APB1PeriphClockCmd(ws2812_struct_template->RCC_APBxPeriph_TIM, ENABLE);
    }
    //if
    RCC_APB2PeriphClockCmd(ws2812_struct_template->RCC_APBxPeriph_GPIO, ENABLE);

    /* DMA clock enable */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    /* GPIOA Configuration: Channel 2 as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = ws2812_struct_template->GPIO_Pin;
    GPIO_Init(ws2812_struct_template->GPIOx, &GPIO_InitStructure);
    /* TIM3 DeInit */
    TIM_DeInit(ws2812_struct_template->TIMx);

    /* DMA1 Channel2 Config */
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ws2812_struct_template->TIMx_CCR_x_ADDRESS;
    DMA_DeInit(ws2812_struct_template->DMAy_Channelx);
    DMA_Init(ws2812_struct_template->DMAy_Channelx, &DMA_InitStructure);

    TIM_TimeBaseInit(ws2812_struct_template->TIMx, &TIM_TimeBaseStructure);
    if (1 == ws2812_struct_template->TIM_Ch)
    {
            TIM_OC1Init(ws2812_struct_template->TIMx, &TIM_OCInitStructure);
    }
    else if (2 == ws2812_struct_template->TIM_Ch)
    {
        TIM_OC2Init(ws2812_struct_template->TIMx, &TIM_OCInitStructure);
    }
     else if (3 == ws2812_struct_template->TIM_Ch)
    {
        TIM_OC3Init(ws2812_struct_template->TIMx, &TIM_OCInitStructure);
    }
    else
    {
        TIM_OC4Init(ws2812_struct_template->TIMx, &TIM_OCInitStructure);
    }
    TIM_DMAConfig(ws2812_struct_template->TIMx, ws2812_struct_template->TIM_DMABase, TIM_DMABurstLength_1Transfer);

    Wrap_buffer_led();

    /* ws2812_struct_template->TIMx DMA Update enable */
    TIM_DMACmd(ws2812_struct_template->TIMx, TIM_DMA_Update, ENABLE);

    /* ws2812_struct_template->TIMx enable */
    TIM_Cmd(ws2812_struct_template->TIMx, ENABLE);

    /* ws2812_struct_template->TIMx PWM Outputs Enable */
    TIM_CtrlPWMOutputs(ws2812_struct_template->TIMx, ENABLE);

    /* DMA1 Channel2 enable */
    DMA_Cmd(ws2812_struct_template->DMAy_Channelx, ENABLE);

    /* Wait until DMA1 Channel2 end of Transfer */
    while (!DMA_GetFlagStatus(ws2812_struct_template->DMAy_FLAG))
    {
    }

    // Disable the timer and DMA after the condition is met
    TIM_CtrlPWMOutputs(ws2812_struct_template->TIMx, DISABLE);
    DMA_Cmd(ws2812_struct_template->DMAy_Channelx, DISABLE);
    TIM_Cmd(ws2812_struct_template->TIMx, DISABLE);
    TIM_DMACmd(ws2812_struct_template->TIMx, TIM_DMA_Update, DISABLE);
}

/**
 * @brief  Main program
 * @param  None
 * @retval None
 */
int main(void)
{

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;


    // DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)TIM3_CCR2_ADDRESS;
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

    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = 90 - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = (uint16_t)(SystemCoreClock / 72000000) - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

    /* TIM Configuration in PWM Mode */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 90 - 1;

    /* Infinite loop */
    while (1)
    {
        static int cnt = 0;
        if (cnt >= MAX_LED)
        {
            cnt = 0;
            for (int i = 0; i < MAX_LED; i++)
            {
                Set_LED(i, 0, 0, 0);
            }
            refresh_strip(ws2812_struct_template + 2);
            for (int i = 0; i < 2000000; i++)
            {
            }
        }

        Set_LED(cnt, 0, 255, 0);
        cnt++;

        __disable_irq();
        refresh_strip(ws2812_struct_template + 2);
        __enable_irq();
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
