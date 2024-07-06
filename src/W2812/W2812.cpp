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
void Ws2812::sendData(uint8_t b, uint8_t g, uint8_t r)
{
	sendByte(g);
	sendByte(r);
	sendByte(b);
}

void Ws2812::clearAll()
{
	uint16_t i;
	for (i = 0; i < _numOfLeds; ++i)
	{
		sendData(0x00, 0x00, 0x00);
	}
}

void Ws2812::clearAll(uint16_t numOfLeds)
{
	uint16_t i;
	__disable_irq();
	for (i = 0; i < numOfLeds; ++i)
	{
		sendData(0x00, 0x00, 0x00);
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
		sendData(_leds[i3], _leds[i3 + 1], _leds[i3 + 2]);
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
void Ws2812::setPixel(uint16_t Pixel, uint8_t green, uint8_t red, uint8_t blue) {
    _leds[Pixel * 3] = (green * _brightness) / 255;
    _leds[Pixel * 3 + 1] = (red * _brightness) / 255;
    _leds[Pixel * 3 + 2] = (blue * _brightness) / 255;
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