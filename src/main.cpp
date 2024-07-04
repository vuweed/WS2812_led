/**
 ******************************************************************************
 * @file    TIM/TimeBase/main.c
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

#include <include.h>
#include <SPI/SPIClass.h>
#include <Serial/Serial.h>
#include <SD/SD.h>
#include <W2812/W2812.h>
/** @addtogroup STM32F10x_StdPeriph_Examples
 * @{
 */

/** @addtogroup TIM_TimeBase
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef TIM_OCInitStructure;
__IO uint16_t CCR1_Val = 40961;
__IO uint16_t CCR2_Val = 27309;
__IO uint16_t CCR3_Val = 13654;
__IO uint16_t CCR4_Val = 6826;
uint16_t PrescalerValue = 0;

#define HC595_PIN_SDA C13
#define HC595_PIN_CLK C14
#define HC595_PIN_LAT C15


#define RS485_PIN_DIR A1

#define ERR_SD_INIT 0x40	// number 0 with dot
#define ERR_NO_ID 0x79		// number 1 with dot
#define ERR_SLAVE_WAIT 0x24 // number 2 with dot
#define ERR_SD_READ 0x30	// number 3 with dot

#define RS485_rec() digitalWrite(RS485_PIN_DIR, LOW)
#define RS485_send() digitalWrite(RS485_PIN_DIR, HIGH)

#define ID_MASTER 0
#define ID_SLAVE 1

union SPIData
{
	uint32_t u32;
	uint8_t bytes[4];
};
union SPIData w, h, numOfFrames, delay2;

File root;
File myFile;
Ws2812 ports[20];
// const uint16_t pins[20] = {B3, B4, B5, B6, B7, B8, B9, B10, B11, B12, B13, B14, B15, A0, A8, A9, A10, A11, A12, A15};

// const uint16_t pins[20] = {A0, A8, A9, A10, A11, A12, A15, B15, B14, B13, B12, B11, B10, B9, B8, B7, B6, B5, B4, B3};
const uint16_t pins[20] = {B10, B11, B12, B13, B14, B15, A8, A9, A0, A12, A11, A10, A15, B3, B4, B5, B6, B7, B8, B9};
uint16_t i = 0;
uint16_t k = 0;
uint8_t _resetFlag = 0;
uint8_t _SelectedEffect = 0;
uint8_t _fileName[] = "M000";
uint8_t fileExist[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t RS485_data[10] = {'R', 'S', 'S', '0', '0', '1', 50, 0x00, 0x00, 0x00};
const uint8_t keys[18] = {0x19, 0xAA, 0x37, 0x92, 0x01, 0x12, 0x96, 0xCF, 0xF2, 0x09, 0xA5, 0xFA, 0xFB, 0xFE, 0x66, 0x9F, 0xDF, 0xF9};

uint8_t const code7seg[] = {0xC0, 0XF9, 0XA4, 0XB0, 0X99, 0X92, 0X82, 0XF8, 0X80, 0X90};
uint8_t isMaster = ID_SLAVE;
/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);

/* Private functions ---------------------------------------------------------*/

void delay3(uint16_t ms)
{
	uint16_t x, y;
	for (x = 0; x < ms; x++)
		for (y = 0; y <= 10000; y++)
			;
}

//
void HC595_write(uint8_t data2)
{
	uint8_t i;
	for (i = 8; i > 0; i--)
	{
		if ((data2 & 0x80) == 0x80)
			digitalWrite(HC595_PIN_SDA, 1);
		else
			digitalWrite(HC595_PIN_SDA, 0);
		data2 = data2 << 1;
		digitalWrite(HC595_PIN_CLK, 0);
		digitalWrite(HC595_PIN_CLK, 1);
	}
	digitalWrite(HC595_PIN_LAT, 0);
	digitalWrite(HC595_PIN_LAT, 1);
}

void HC595_displayError(uint8_t err, uint8_t dl)
{
	HC595_write(code7seg[err]);
	delay(dl);
	HC595_write(0xFF);
	delay(50);
}

void INT_Plus_FUNC()
{
	if (_resetFlag == 0)
	{
		if (_SelectedEffect == 9)
			_SelectedEffect = 0;
		else
			_SelectedEffect++;
		while (fileExist[_SelectedEffect] == 0)
		{
			++_SelectedEffect;
			if (_SelectedEffect == 10)
			{
				_SelectedEffect = 0;
			}
		}
		_resetFlag = 1;
		delay3(300);
	}
}

void INT_Minus_FUNC()
{
	if (_resetFlag == 0)
	{
		if (_SelectedEffect == 0)
			_SelectedEffect = 9;
		else
			_SelectedEffect--;

		while (fileExist[_SelectedEffect] == 0)
		{
			--_SelectedEffect;
			if (_SelectedEffect == 255)
			{
				_SelectedEffect = 9;
			}
		}
		_resetFlag = 1;
		delay3(300);
	}
}

void GPIO_begin(void)
{
	pinMode(HC595_PIN_LAT, OUTPUT);
	pinMode(HC595_PIN_CLK, OUTPUT);
	pinMode(HC595_PIN_SDA, OUTPUT);
	pinMode(RS485_PIN_DIR, OUTPUT); // RS485 DIR

	if (isMaster == ID_MASTER)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	 // enable AFIO before
		GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); // remap to use B3B4A15

		RS485_send();

		attachInterrupt(B2, FALLING, INT_Plus_FUNC);  // button config interrupt
		attachInterrupt(B1, FALLING, INT_Minus_FUNC); // button config interrupt

		analogEnable(VR_PIN); // VR config ADC
	}
	else
	{
		//		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);// enable AFIO before
		//		GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);// remap to use B3B4A15

		RS485_rec();
	}
}

long map2(long x, long in_min, long in_max, long out_min, long out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

uint8_t *cryption(uint8_t *data, uint32_t len)
{
	for (uint32_t j = 0; j < len; ++j)
	{
		data[j] = data[j] ^ keys[j % 18];
	}
	return data;
}
/**
 * @brief  Main program
 * @param  None
 * @retval None
 */
int sound_value = 0;
int state_flag = STATE_1;
bool toggle_all_led_flag = false;
int count_for_loop_2 = 0;
int count_for_loop = 0;

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

	/* NVIC Configuration */
	NVIC_Configuration();

	/* GPIO Configuration */
//	GPIO_Configuration();
	pinMode(HC595_PIN_LAT, OUTPUT);
	pinMode(HC595_PIN_CLK, OUTPUT);
	pinMode(HC595_PIN_SDA, OUTPUT);

	/* ---------------------------------------------------------------
	  TIM2 Configuration: Output Compare Timing Mode:
	  TIM2 counter clock at 6 MHz
	  CC1 update rate = TIM2 counter clock / CCR1_Val = 146.48 Hz
	  CC2 update rate = TIM2 counter clock / CCR2_Val = 219.7 Hz
	  CC3 update rate = TIM2 counter clock / CCR3_Val = 439.4 Hz
	  CC4 update rate = TIM2 counter clock / CCR4_Val = 878.9 Hz
	--------------------------------------------------------------- */

	/* Compute the prescaler value */
	PrescalerValue = (uint16_t)(SystemCoreClock / 12000000) - 1;

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 65535;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	/* Prescaler configuration */
	TIM_PrescalerConfig(TIM2, PrescalerValue, TIM_PSCReloadMode_Immediate);

	/* Output Compare Timing Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	/* TIM IT enable */
	TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);
	SysTick_Init();

	/* TIM2 enable counter */
	TIM_Cmd(TIM2, ENABLE);
	// analogEnable(VR_PIN);
	 for(i = 0; i < 10; ++i)
	 {
	 	HC595_write(code7seg[i]);
	 	delay(150);
	 }

	 //  init SD card
	 while (!SD.begin(SPI1_PIN_NSS))
	 {
	 	HC595_write(ERR_SD_INIT);
	 	delay(100);
	 }

	 Serial2.begin(115200);
	 //check file exist list
	 for(uint8_t ii = 0; ii < 10; ++ii)
	 {
	 	_fileName[3] = ii + 48;
	 	root = SD.open("/");
	 	while(1)
	 	{
	 		File entry =  root.openNextFile();
	 		if (! entry)
	 		{
	 			// no more files
	 			break;
	 		}
	 		if (entry.isDirectory() == false)
	 		{
	 			if(strstr(entry.name(), (const char *)_fileName))
	 			{
	 				isMaster = ID_MASTER;
	 				fileExist[ii] = 1;
	 				entry.close();
	 				break;
	 			}
	 		}
	 		entry.close();
	 	}
	 	root.close();
	 }

	GPIO_begin();

	// init W2812 channels
	for (i = 0; i < 20; ++i)
	{
		ports[i].begin(pins[i]);
		ports[i].clearAll(300);
	}


	while(1)
	{
		RESET:// reset when button is pressed
		switch (isMaster)
		{
			case ID_MASTER:
				 _resetFlag = 0;// clear reset flag
				 HC595_write(code7seg[_SelectedEffect]);// show selected effect to 7segs
				 _fileName[3] = _SelectedEffect + 48;// update filename base on selected effect
				 RS485_data[5] = _SelectedEffect + 48;// update RS485's filename base on selected effect


				 // delay2.u32 = analogRead(B0);
				 // Serial.print("1: ");
				 // Serial.println(delay2.u32);
				 delay2.u32 = map2(delay2.u32, 0, 4096, 20, 200);
				 delay2.u32=40;
				 // update delay value to RS485 data
				 RS485_data[6] = delay2.bytes[0];
				 RS485_data[7] = delay2.bytes[1];
				 RS485_data[8] = delay2.bytes[2];
				 RS485_data[9] = delay2.bytes[3];

				 Serial2.write(RS485_data, 10);// send data to RS485

				 // check file with substring
				 root = SD.open("/");


				 ///////////////////////////////////////////////////
				            while(count_for_loop < 100)
                            {
                                count_for_loop++;
                 					myFile =  root.openNextFile();
                 					if (! myFile)
                 					{
                 						// no more files
                 						break;
                 					}
                 					if (myFile.isDirectory() == false)
                 					{
                 						if(strstr(myFile.name(), (const char *)_fileName))
                 						{
                 //                          // read parameters:
                 							myFile.readBytes(w.bytes , 4);
                 							myFile.readBytes(h.bytes , 4);
                 							myFile.readBytes(numOfFrames.bytes , 4);
                 							for(k = 0; k < 4; ++k)
                 							{
                 								w.bytes[k] = w.bytes[k] ^ keys[k % 18];
                 								h.bytes[k] = h.bytes[k] ^ keys[k % 18];
                 								numOfFrames.bytes[k] = numOfFrames.bytes[k] ^ keys[k % 18];
                 							}
                 							// set up W2812 parameter
                 							for (i = 0; i < w.u32; ++i)
                 							{
                 								ports[i].setLED(h.u32);
                 							}

                 							// display to every channels
                 							for (uint32_t frame = 0; frame < numOfFrames.u32; ++frame)
                 							{
                 								for (i = 0; i < w.u32; ++i)
                 								{
                 									if (_resetFlag == 1)
                 									{
                 										myFile.close();
                 										root.close();
                 										goto RESET;
                 									}
                 									myFile.readBytes(ports[i]._leds ,h.u32 * 3);
                 									ports[i]._leds = cryption(ports[i]._leds ,h.u32 * 3);
                 								}
                 								for (i = 0; i < w.u32; ++i)
                 								{
                 									if (_resetFlag == 1)
                 									{
                 										myFile.close();
                 										root.close();
                 										goto RESET;
                 									}
                 									ports[i].showStrip();
                 								}

                 								// delay and check _resetFlag
                 								// Serial.print("2: ");
                 								// Serial.println(delay2.u32);
                 								for (i = 0; i < delay2.u32; ++i)
                 								{
                 									if (_resetFlag == 1)
                 									{
                 										myFile.close();
                 										root.close();
                 										goto RESET;
                 									}
                 									delay(1);
                 								}
                 							}
                 							// close the file:
                 							myFile.close();
                 						}
                 					}
                 					myFile.close();


                            }
                            count_for_loop = 0;
				///////////////////////////////////////////////////
				while(1)
				{
				    if (toggle_all_led_flag == false)
				    {
                        if(state_flag ==  PROCESSING_1)
                        {
//                        	count_for_loop = 0;
                            while(count_for_loop_2 < 1000)
                            {
                                count_for_loop_2++;
                 					myFile =  root.openNextFile();
                 					if (! myFile)
                 					{
        								if(count_for_loop_2 >= 999)
        								{
        									count_for_loop = 0;
        								}
//        								delay(1000);

                 						// no more files
                 						break;
                 					}
                 					if (myFile.isDirectory() == false)
                 					{
                 						if(strstr(myFile.name(), (const char *)_fileName))
                 						{
                 //                          // read parameters:
                 							myFile.readBytes(w.bytes , 4);
                 							myFile.readBytes(h.bytes , 4);
                 							myFile.readBytes(numOfFrames.bytes , 4);
                 							for(k = 0; k < 4; ++k)
                 							{
                 								w.bytes[k] = w.bytes[k] ^ keys[k % 18];
                 								h.bytes[k] = h.bytes[k] ^ keys[k % 18];
                 								numOfFrames.bytes[k] = numOfFrames.bytes[k] ^ keys[k % 18];
                 							}
                 							// set up W2812 parameter
                 							for (i = 0; i < w.u32; ++i)
                 							{
                 								ports[i].setLED(h.u32);
                 							}

                 							// display to every channels
                 							for (uint32_t frame = 0; frame < numOfFrames.u32; ++frame)
                 							{
                 								for (i = 0; i < w.u32; ++i)
                 								{
                 									if (_resetFlag == 1)
                 									{
                 										myFile.close();
                 										root.close();
                 										goto RESET;
                 									}
                 									myFile.readBytes(ports[i]._leds ,h.u32 * 3);
                 									ports[i]._leds = cryption(ports[i]._leds ,h.u32 * 3);
                 								}
                 								for (i = 0; i < w.u32; ++i)
                 								{
                 									if (_resetFlag == 1)
                 									{
                 										myFile.close();
                 										root.close();
                 										goto RESET;
                 									}
                 									ports[i].showStrip();
                 								}

                 								// delay and check _resetFlag
                 								// Serial.print("2: ");
                 								// Serial.println(delay2.u32);
                 								for (i = 0; i < delay2.u32; ++i)
                 								{
                 									if (_resetFlag == 1)
                 									{
                 										myFile.close();
                 										root.close();
                 										goto RESET;
                 									}
                 									delay(1);
                 								}
                 							}
                 							// close the file:
                 							myFile.close();
                 						}
                 					}
                 					myFile.close();


                            }
                            if(count_for_loop_2 > 999)
                            {
                            	count_for_loop_2 = 0;
                            	state_flag = STATE_1;
                            }
                        }
				    }
				    else
				    {
						while(count_for_loop < 6)
						{
							count_for_loop++;
							//turn off all led strips
							myFile =  root.openNextFile();
							if (! myFile)
							{
								if(count_for_loop >= 5)
								{
									count_for_loop = 0;
								}
								delay(1000);

								// no more files
								break;
							}
							if (myFile.isDirectory() == false)
							{
								if(strstr(myFile.name(), (const char *)_fileName))
								{
			//                          // read parameters:
									myFile.readBytes(w.bytes , 4);
									myFile.readBytes(h.bytes , 4);
									myFile.readBytes(numOfFrames.bytes , 4);
									for(k = 0; k < 4; ++k)
									{
										w.bytes[k] = w.bytes[k] ^ keys[k % 18];
										h.bytes[k] = h.bytes[k] ^ keys[k % 18];
										numOfFrames.bytes[k] = numOfFrames.bytes[k] ^ keys[k % 18];
									}
									// set up W2812 parameter
									for (i = 0; i < w.u32; ++i)
									{
										ports[i].setLED(h.u32);
									}

									// display to every channels
									for (uint32_t frame = 0; frame < numOfFrames.u32; ++frame)
									{
										for (i = 0; i < w.u32; ++i)
										{
											if (_resetFlag == 1)
											{
												myFile.close();
												root.close();
												goto RESET;
											}
											// myFile.readBytes(ports[i]._leds ,h.u32 * 3);
											// ports[i]._leds = cryption(ports[i]._leds ,h.u32 * 3);
											ports[i].setAll(0,0,0);
										}
										for (i = 0; i < w.u32; ++i)
										{
											if (_resetFlag == 1)
											{
												myFile.close();
												root.close();
												goto RESET;
											}
											ports[i].showStrip();
										}

										// delay and check _resetFlag
										// Serial.print("2: ");
										// Serial.println(delay2.u32);
										for (i = 0; i < delay2.u32; ++i)
										{
											if (_resetFlag == 1)
											{
												myFile.close();
												root.close();
												goto RESET;
											}
											delay(1);
										}
									}
									// close the file:
									myFile.close();
								}
							}
							myFile.close();
						delay(1000);
						}
//						if(coun)
//						count_for_loop = 0;
				    }

				}
				root.close();

				break;
			case ID_SLAVE:
				// RS485_rec();
				// wait for RS485 signal
				while (_resetFlag == 0)
				{
					HC595_write(ERR_SLAVE_WAIT);
					// delay and check _resetFlag
					for (i = 0; i < 100; ++i)
					{
						if (_resetFlag == 1)
						{
							//myFile.close();
							goto RESET;
						}
						delay(1);
					}
				}
				_SelectedEffect = _fileName[3]-48;
				HC595_write(code7seg[_SelectedEffect]);// show selected effect to 7segs
				_resetFlag = 0;

				// check file with substring
				root = SD.open("/");
				while(1)
				{
					myFile =  root.openNextFile();
					if (! myFile)
					{
						// no more files
						break;
					}
					if (myFile.isDirectory() == false)
					{
						if(strstr(myFile.name(), (const char *)_fileName))
						{
//							// read parameters:
							myFile.readBytes(w.bytes , 4);
							myFile.readBytes(h.bytes , 4);
							myFile.readBytes(numOfFrames.bytes , 4);
							for(k = 0; k < 4; ++k)
							{
								w.bytes[k] = w.bytes[k] ^ keys[k % 18];
								h.bytes[k] = h.bytes[k] ^ keys[k % 18];
								numOfFrames.bytes[k] = numOfFrames.bytes[k] ^ keys[k % 18];
							}
							// set up W2812 parameter
							for (i = 0; i < w.u32; ++i)
							{
								ports[i].setLED(h.u32);
							}

							// display to every channels
							for (uint32_t frame = 0; frame < numOfFrames.u32; ++frame)
							{
								for (i = 0; i < w.u32; ++i)
								{
									if (_resetFlag == 1)
									{
										myFile.close();
										root.close();
										goto RESET;
									}
									myFile.readBytes(ports[i]._leds ,h.u32 * 3);
									ports[i]._leds = cryption(ports[i]._leds ,h.u32 * 3);
								}
								for (i = 0; i < w.u32; ++i)
								{
									if (_resetFlag == 1)
									{
										myFile.close();
										root.close();
										goto RESET;
									}
									ports[i].showStrip();
								}

								// delay and check _resetFlag
								for (i = 0; i < delay2.u32; ++i)
								{
									if (_resetFlag == 1)
									{
										myFile.close();
										root.close();
										goto RESET;
									}
									delay(1);
								}
							}
							// close the file:
							myFile.close();
						}
					}
					myFile.close();
				}
				root.close();

				break;
			default:
				break;
		}
	}
}

/**
 * @brief  Configures the different system clocks.
 * @param  None
 * @retval None
 */
void RCC_Configuration(void)
{
	/* PCLK1 = HCLK/4 */
	RCC_PCLK1Config(RCC_HCLK_Div4);

	/* TIM2 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	/* GPIOC clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
}

/**
 * @brief  Configure the GPIO Pins.
 * @param  None
 * @retval None
 */
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* GPIOC Configuration:Pin6, 7, 8 and 9 as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/**
 * @brief  Configure the nested vectored interrupt controller.
 * @param  None
 * @retval None
 */
void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable the TIM2 global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure);
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

/**
 * @}
 */

/**
 * @}
 */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
