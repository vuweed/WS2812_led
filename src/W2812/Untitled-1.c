/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "math.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;
DMA_HandleTypeDef hdma_tim1_ch1;
DMA_HandleTypeDef hdma_tim1_ch2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

#define MAX_LED 100
#define USE_BRIGHTNESS 1


uint8_t LED_Data[MAX_LED][4];
uint8_t LED_Mod[MAX_LED][4];  // for brightness

int datasentflag=0;

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
    HAL_TIM_PWM_Stop_DMA(&htim1, TIM_CHANNEL_1);
    datasentflag=1;
}

void Set_LED (int LEDnum, int Red, int Green, int Blue)
{
    LED_Data[LEDnum][0] = LEDnum;
    LED_Data[LEDnum][1] = Green;
    LED_Data[LEDnum][2] = Red;
    LED_Data[LEDnum][3] = Blue;
}
static void Set_LED_Green(int LEDnum)
{
    LED_Data[LEDnum][0] = LEDnum;
    LED_Data[LEDnum][1] = 255;
    LED_Data[LEDnum][2] = 0;
    LED_Data[LEDnum][3] = 0;
}
static void Set_LED_Blue(int LEDnum)
{
    LED_Data[LEDnum][0] = LEDnum;
    LED_Data[LEDnum][1] = 0;
    LED_Data[LEDnum][2] = 0;
    LED_Data[LEDnum][3] = 255;
}
static void Set_LED_Red(int LEDnum)
{
    LED_Data[LEDnum][0] = LEDnum;
    LED_Data[LEDnum][1] = 0;
    LED_Data[LEDnum][2] = 255;
    LED_Data[LEDnum][3] = 0;
}
static void Set_LED_X1(int LEDnum)
{
    LED_Data[LEDnum][0] = LEDnum;
    LED_Data[LEDnum][1] = 255;
    LED_Data[LEDnum][2] = 255;
    LED_Data[LEDnum][3] = 0;
}
static void Set_LED_X2(int LEDnum)
{
    LED_Data[LEDnum][0] = LEDnum;
    LED_Data[LEDnum][1] = 0;
    LED_Data[LEDnum][2] = 255;
    LED_Data[LEDnum][3] = 255;
}
void led_effect_1()
{
    int i = 0;
    for(i = 0; i < MAX_LED; i++)
    {
        Set_LED_Green(i);
        Set_Brightness(10);
        WS2812_Send();
//        HAL_Delay(10);

        Set_LED_Blue(i+1);
        Set_Brightness(20);
        WS2812_Send();
        HAL_Delay(10);

        Set_LED_Red(i+2);
        Set_Brightness(30);
        WS2812_Send();
        HAL_Delay(10);

        Set_LED_X1(i+3);
        Set_Brightness(40);
        WS2812_Send();
        HAL_Delay(10);

        Set_LED_X2(i+4);
        Set_Brightness(45);
        WS2812_Send();
        HAL_Delay(10);
//        HAL_Delay(900);
    }
}
void led_effect_2()
{
    int i = 0;
    for(i = 0; i < MAX_LED; i++)
    {
        Set_LED_Green(i);

//        HAL_Delay(10);

        Set_LED_Blue(i+1);
//        Set_Brightness(20);
//        WS2812_Send();
//        HAL_Delay(10);

        Set_LED_Red(i+2);
//        Set_Brightness(30);
//        WS2812_Send();
//        HAL_Delay(10);

        Set_LED_X1(i+3);
//        Set_Brightness(40);
//        WS2812_Send();
//        HAL_Delay(10);

        Set_LED_X2(i+4);
//        Set_Brightness(45);
//        WS2812_Send();
//        HAL_Delay(10);
//        HAL_Delay(900);
        Set_Brightness(45);
        WS2812_Send();
        HAL_Delay(100);
    }
    Reset_LED();
    HAL_Delay(100);

}

void led_effect_3()
{
    int i = 0;
    for(i = 0; i < MAX_LED; i++)
    {
        Set_LED_Red(i);

//        HAL_Delay(10);

        Set_LED_Blue(i+1);
//        Set_Brightness(20);
//        WS2812_Send();
//        HAL_Delay(10);

        Set_LED_Green(i+2);
//        Set_Brightness(30);
//        WS2812_Send();
//        HAL_Delay(10);

        Set_LED_X1(i+3);
//        Set_Brightness(40);
//        WS2812_Send();
//        HAL_Delay(10);

        Set_LED_X2(i+4);
//        Set_Brightness(45);
//        WS2812_Send();
//        HAL_Delay(10);
//        HAL_Delay(900);
        Set_Brightness(45);
        WS2812_Send();
        HAL_Delay(100);
    }
    Reset_LED();
    HAL_Delay(100);

}
#define PI 3.14159265

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

uint16_t pwmData[(24*MAX_LED)+50];

void WS2812_Send (void)
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

    HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_1, (uint32_t *)pwmData, indx);
    HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_2, (uint32_t *)pwmData, indx);
    while (!datasentflag){};
    datasentflag = 0;
}

void Reset_LED (void)
{
    for (int i=0; i<MAX_LED; i++)
    {
        LED_Data[i][0] = i;
        LED_Data[i][1] = 0;
        LED_Data[i][2] = 0;
        LED_Data[i][3] = 0;
    }
}


// ported from the arduino code for 8 LEDs located at ->>>>  https://adrianotiger.github.io/Neopixel-Effect-Generator/

uint16_t effStep = 0;

uint8_t rainbow_effect_left() {
    // Strip ID: 0 - Effect: Rainbow - LEDS: 8
    // Steps: 13 - Delay: 54
    // Colors: 3 (255.0.0, 0.255.0, 0.0.255)
    // Options: rainbowlen=8, toLeft=true,
//  if(millis() - strip_0.effStart < 54 * (strip_0.effStep)) return 0x00;

  float factor1, factor2;
  uint16_t ind;
  for(uint16_t j=0;j<8;j++) {
    ind = effStep + j * 1.625;
    switch((int)((ind % 13) / 4.333333333333333)) {
      case 0: factor1 = 1.0 - ((float)(ind % 13 - 0 * 4.333333333333333) / 4.333333333333333);
              factor2 = (float)((int)(ind - 0) % 13) / 4.333333333333333;
              /************ chnaged here *********/

              Set_LED(j, 255 * factor1 + 0 * factor2, 0 * factor1 + 255 * factor2, 0 * factor1 + 0 * factor2);

              WS2812_Send();
              break;
      case 1: factor1 = 1.0 - ((float)(ind % 13 - 1 * 4.333333333333333) / 4.333333333333333);
              factor2 = (float)((int)(ind - 4.333333333333333) % 13) / 4.333333333333333;
              Set_LED(j, 0 * factor1 + 0 * factor2, 255 * factor1 + 0 * factor2, 0 * factor1 + 255 * factor2);
              WS2812_Send();
              break;
      case 2: factor1 = 1.0 - ((float)(ind % 13 - 2 * 4.333333333333333) / 4.333333333333333);
              factor2 = (float)((int)(ind - 8.666666666666666) % 13) / 4.333333333333333;
              Set_LED(j, 0 * factor1 + 255 * factor2, 0 * factor1 + 0 * factor2, 255 * factor1 + 0 * factor2);
              WS2812_Send();
              break;
    }
  }
  if(effStep >= 13) {effStep=0; return 0x03; }
  else effStep++;
  return 0x01;
}



uint8_t rainbow_effect_right() {
    // Strip ID: 0 - Effect: Rainbow - LEDS: 8
    // Steps: 14 - Delay: 30
    // Colors: 3 (255.0.0, 0.255.0, 0.0.255)
    // Options: rainbowlen=8, toLeft=false,
//  if(millis() - strip_0.effStart < 30 * (strip_0.effStep)) return 0x00;
  float factor1, factor2;
  uint16_t ind;
  for(uint16_t j=0;j<8;j++) {
    ind = 14 - (int16_t)(effStep - j * 1.75) % 14;
    switch((int)((ind % 14) / 4.666666666666667)) {
      case 0: factor1 = 1.0 - ((float)(ind % 14 - 0 * 4.666666666666667) / 4.666666666666667);
              factor2 = (float)((int)(ind - 0) % 14) / 4.666666666666667;
              Set_LED(j, 255 * factor1 + 0 * factor2, 0 * factor1 + 255 * factor2, 0 * factor1 + 0 * factor2);
              WS2812_Send();
              break;
      case 1: factor1 = 1.0 - ((float)(ind % 14 - 1 * 4.666666666666667) / 4.666666666666667);
              factor2 = (float)((int)(ind - 4.666666666666667) % 14) / 4.666666666666667;
              Set_LED(j, 0 * factor1 + 0 * factor2, 255 * factor1 + 0 * factor2, 0 * factor1 + 255 * factor2);
              WS2812_Send();
              break;
      case 2: factor1 = 1.0 - ((float)(ind % 14 - 2 * 4.666666666666667) / 4.666666666666667);
              factor2 = (float)((int)(ind - 9.333333333333334) % 14) / 4.666666666666667;
              Set_LED(j, 0 * factor1 + 255 * factor2, 0 * factor1 + 0 * factor2, 255 * factor1 + 0 * factor2);
              WS2812_Send();
              break;
    }
  }
  if(effStep >= 14) {effStep = 0; return 0x03; }
  else effStep++;
  return 0x01;
}
void led_move()
{
    int i = 0;
    int R = 0, G = 0, B = 0;
    for(i = 0; i < MAX_LED; i++)
    {
        Set_LED(i, R, G, B);
        Set_Brightness(10);
        WS2812_Send();
        HAL_Delay(30);
        if(R > 255)
            R = 0;
        if(G > 255)
            G = 0;
        if(B > 255)
            B = 0;

        R+=10;
        G+=10;
        B+=10;
    }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  Set_LED(0, 255, 0, 0);
  Set_LED(1, 255, 0, 0);
  Set_LED(2, 0, 0, 255);

  Set_LED(3, 46, 89, 128);

  Set_LED(4, 156, 233, 100);
  Set_LED(5, 102, 0, 235);
  Set_LED(6, 47, 38, 77);

  Set_LED(7, 255, 200, 0);

  Set_Brightness(10);
  WS2812_Send();
//  Reset_LED();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

//      rainbow_effect_right();
//      rainbow_effect_left();
//        HAL_Delay (30);
      led_move();
      led_effect_2();
      led_effect_3();
//        for (int i=0; i<46; i++)
//        {
//            Set_Brightness(i);
//            WS2812_Send();
//            HAL_Delay (5);
//        }
////
//        for (int i=45; i>=0; i--)
//        {
//            Set_Brightness(i);
//            WS2812_Send();
//            HAL_Delay (5);
//        }
  }

  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 90-1;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
    sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
