/**
 * WS2812.h
 * Created: 2023/03/30
 * Author: TaiVB
 */

#ifndef WS2812_H
#define WS2812_H


#define ADDR_EFF	0x3000
#define ADDR_COLOR	0x3001
#define DELAY		200/NUM_LEDS * _delay / 10 + 1
#define DELAY2		200/NUM_LEDS * _delay / 100 + 1

#define COLOR_RED			3,		239,	7
#define COLOR_VIOLET		64,		160,    198
#define COLOR_ORANGE		153,	255,	0
#define COLOR_YELLOW		235, 	255, 	0
#define COLOR_GREEN			185, 	29, 	84
#define COLOR_BLUE  		13, 	21, 	247
#define COLOR_INDIGO		0,		110,	255
#define COLOR_WHITE		    244,	253,	220

#define _nop_() asm("nop")
#define _nop5() asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");
#define _nop10() _nop5();_nop5();
#define _nop15() _nop10();_nop5();
#define _nop20() _nop15();_nop5();
#define _nop25() _nop20();_nop5();
#define _nop30() _nop25();_nop5();
#define _nop35() _nop30();_nop5();
#define _nop40() _nop35();_nop5();
#define _nop45() _nop40();_nop5();
#define _nop50() _nop45();_nop5();

#define nop1H() _nop45()
#define nop1L() _nop10()
#define nop0H() _nop15()
#define nop0L() _nop40()


/**********************Common function**********************************/
class Ws2812
{
public:
	uint8_t *_leds;
	uint8_t _brightness;
	Ws2812();
	virtual ~Ws2812();
	// config
	void begin(uint16_t pinData);
	//
	void setLED(uint16_t numOfLeds);
	//
	void sendByte(uint8_t dat);
	//
	void sendData(uint8_t b, uint8_t g, uint8_t r);
	// Apply LED color changes
	void showStrip();
	// Set a LED color (not yet visible)
	void setPixel(uint16_t Pixel, uint8_t green, uint8_t red, uint8_t blue);
	// Set all LEDs to a given color and apply it (visible)
	void setAll(uint8_t green, uint8_t red, uint8_t blue);
	// clean all led
	void clearAll();
	// clean all led 2
	void clearAll(uint16_t numOfLeds);
	//
	void setBrightness(uint8_t brightness);

	void setAllCustom(void);

	void send(uint8_t b,uint8_t g,uint8_t r);
private:
	GPIO_TypeDef* _pinGPIO;
	uint16_t _pinData;
	uint8_t _pinNum;
	uint16_t _numOfLeds;
	//
	void sendOne(void);
	//
	void sendZero(void);
};

#endif
