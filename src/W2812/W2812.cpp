/**
 * WS2812.c
 * Created: 2023/03/30
 * Author: TaiVB
 */

#include <include.h>
#include <W2812/W2812.h>



Ws2812::Ws2812()
{

}

Ws2812::~Ws2812()
{
	// TODO Auto-generated destructor stub
}



void Ws2812::begin(uint16_t pinData)
{
	_pinData = pinData;
	pinMode(_pinData, OUTPUT);
	_pinGPIO = getDigitalGPIO(_pinData);
	_pinNum = getDigitalPin(_pinData);
	_leds = new uint8_t[3 * 300];
}

void Ws2812::setLED(uint16_t numOfLeds)
{
	_numOfLeds = numOfLeds;
}

void Ws2812::sendOne(void)
{
	_pinGPIO->BSRR = (1<<_pinNum);
	_nop5();
	_nop5();
	_nop5();
	_nop5();
	_nop5();
	_nop5();
	_nop5();
	_nop5();
	_nop5();
	_pinGPIO->BRR = (1<<_pinNum);
}

void Ws2812::sendZero(void)
{
	_pinGPIO->BSRR = (1<<_pinNum);
	_nop5();
	_nop5();
	_nop5();
	_nop5();
	_pinGPIO->BRR = (1<<_pinNum);
	_nop5();
	_nop5();
	_nop5();
	_nop5();
}
void Ws2812::sendByte(uint8_t dat)
{
	if (dat & 0x80)
	{
		_pinGPIO->BSRR = (1<<_pinNum);
		nop1H();
		_pinGPIO->BRR = (1<<_pinNum);
		nop1L();
	}
	if ((dat & 0x80) == 0)
	{
		_pinGPIO->BSRR = (1<<_pinNum);
		nop0H();
		_pinGPIO->BRR = (1<<_pinNum);
		nop0L();
	}
	if (dat & 0x40)
	{
		_pinGPIO->BSRR = (1<<_pinNum);
		nop1H();
		_pinGPIO->BRR = (1<<_pinNum);
		nop1L();
	}
	if ((dat & 0x40) == 0)
	{
		_pinGPIO->BSRR = (1<<_pinNum);
		nop0H();
		_pinGPIO->BRR = (1<<_pinNum);
		nop0L();
	}
	if (dat & 0x20)
	{
		_pinGPIO->BSRR = (1<<_pinNum);
		nop1H();
		_pinGPIO->BRR = (1<<_pinNum);
		nop1L();
	}
	if ((dat & 0x20) == 0)
	{
		_pinGPIO->BSRR = (1<<_pinNum);
		nop0H();
		_pinGPIO->BRR = (1<<_pinNum);
		nop0L();
	}
	if (dat & 0x10)
	{
		_pinGPIO->BSRR = (1<<_pinNum);
		nop1H();
		_pinGPIO->BRR = (1<<_pinNum);
		nop1L();
	}
	if ((dat & 0x10) == 0)
	{
		_pinGPIO->BSRR = (1<<_pinNum);
		nop0H();
		_pinGPIO->BRR = (1<<_pinNum);
		nop0L();
	}
	if (dat & 0x08)
	{
		_pinGPIO->BSRR = (1<<_pinNum);
		nop1H();
		_pinGPIO->BRR = (1<<_pinNum);
		nop1L();
	}
	if ((dat & 0x08) == 0)
	{
		_pinGPIO->BSRR = (1<<_pinNum);
		nop0H();
		_pinGPIO->BRR = (1<<_pinNum);
		nop0L();
	}
	if (dat & 0x04)
	{
		_pinGPIO->BSRR = (1<<_pinNum);
		nop1H();
		_pinGPIO->BRR = (1<<_pinNum);
		nop1L();
	}
	if ((dat & 0x04) == 0)
	{
		_pinGPIO->BSRR = (1<<_pinNum);
		nop0H();
		_pinGPIO->BRR = (1<<_pinNum);
		nop0L();
	}
	if (dat & 0x02)
	{
		_pinGPIO->BSRR = (1<<_pinNum);
		nop1H();
		_pinGPIO->BRR = (1<<_pinNum);
		nop1L();
	}
	if ((dat & 0x02) == 0)
	{
		_pinGPIO->BSRR = (1<<_pinNum);
		nop0H();
		_pinGPIO->BRR = (1<<_pinNum);
		nop0L();
	}
	if (dat & 0x01)
	{
		_pinGPIO->BSRR = (1<<_pinNum);
		nop1H();
		_pinGPIO->BRR = (1<<_pinNum);
		nop1L();
	}
	if ((dat & 0x01) == 0)
	{
		_pinGPIO->BSRR = (1<<_pinNum);
		nop0H();
		_pinGPIO->BRR = (1<<_pinNum);
		nop0L();
	}
}
// void Ws2812::sendData(uint8_t b, uint8_t g, uint8_t r)
// {
// 	sendByte(g);
// 	sendByte(r);
// 	sendByte(b);
// }


void Ws2812::refresh_strip()
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

void Ws2812::sendData(uint16_t Pixel, uint8_t b, uint8_t g, uint8_t r)
{
	setPixel(Pixel, g, r, b);

}

void Ws2812::clearAll()
{
	uint16_t i;
	for (i = 0; i < _numOfLeds; ++i)
	{
		sendData(i, 0x00, 0x00, 0x00);
	}
}

void Ws2812::clearAll(uint16_t numOfLeds)
{
	uint16_t i;
	__disable_irq();
	for (i = 0; i < numOfLeds; ++i)
	{
		sendData(i, 0x00, 0x00, 0x00);
	}
	__enable_irq();
}

void Ws2812::showStrip()
{
	uint16_t i;
	uint16_t i3;

	__disable_irq();
	for (i = 0; i < _numOfLeds; ++i)
	{
		i3=3*i;
		sendData(i, _leds[i3], _leds[i3 + 1], _leds[i3 + 2]);
	}
	__enable_irq();
}

// Set a LED color (not yet visible)
// void Ws2812::setPixel(uint16_t Pixel, uint8_t green, uint8_t red, uint8_t blue)
// {
// 	_leds[Pixel * 3] = green;
// 	_leds[Pixel * 3 + 1] = red;
// 	_leds[Pixel * 3 + 2] = blue;
// }

void Ws2812::Wrap_buffer_led(uint16_t Pixel)
{
	uint32_t indx = 0;
	uint32_t color;

	for (Pixel = 0; Pixel < MAX_LED; Pixel++)
	{
#if USE_BRIGHTNESS
		// color = ((LED_Mod[i][1] << 16) | (LED_Mod[i][2] << 8) | (LED_Mod[i][3]));
#else
		color = ((_leds[Pixel * 3] << 16) | (_leds[Pixel * 3 + 1] << 8) | (_leds[Pixel * 3 + 2]));
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

void Ws2812::setPixel(uint16_t Pixel, uint8_t green, uint8_t red, uint8_t blue) {
    _leds[Pixel * 3] = (green * _brightness) / 255;
    _leds[Pixel * 3 + 1] = (red * _brightness) / 255;
    _leds[Pixel * 3 + 2] = (blue * _brightness) / 255;
	Wrap_buffer_led(Pixel);
	refresh_strip();
}

// Set all LEDs to a given color and apply it (visible)
void Ws2812::setAll(uint8_t green, uint8_t red, uint8_t blue)
{
	uint8_t i;
	__disable_irq();
	for (i = 0; i < _numOfLeds; ++i)
	{
		setPixel(i, green, red, blue);
	}
	__enable_irq();
}

void Ws2812::setAllCustom(void)
{
	uint8_t i;
	__disable_irq();
	for (i = 0; i < _numOfLeds; ++i)
	{
		setPixel(i, _leds[i * 3], _leds[i * 3 + 1], _leds[i * 3 + 2]);
	}
	__enable_irq();
}




void Ws2812::setBrightness(uint8_t brightness) {
    _brightness = brightness;
}