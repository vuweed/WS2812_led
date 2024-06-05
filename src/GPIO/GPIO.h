/*
 * GPIO.h
 *
 *  Created on: Aug 8, 2018
 *      Author: taivb
 */

#ifndef GPIO_GPIO_H_
#define GPIO_GPIO_H_

// pin definition
#define	A0		0
#define	A1		1
#define	A2		2
#define	A3		3
#define	A4		4
#define	A5		5
#define	A6		6
#define	A7		7
#define	A8		8
#define	A9		9
#define	A10		10
#define	A11		11
#define	A12		12
#define	A13		13
#define	A14		14
#define	A15		15

#define	B0		16
#define	B1		17
#define	B2		18
#define	B3		19
#define	B4		20
#define	B5		21
#define	B6		22
#define	B7		23
#define	B8		24
#define	B9		25
#define	B10		26
#define	B11		27
#define	B12		28
#define	B13		29
#define	B14		30
#define	B15		31

#define	C0		32
#define	C1		33
#define	C2		34
#define	C3		35
#define	C4		36
#define	C5		37
#define	C6		38
#define	C7		39
#define	C8		40
#define	C9		41
#define	C10		42
#define	C11		43
#define	C12		44
#define	C13		45
#define	C14		46
#define	C15		47

// pin mode definition
#define INPUT				0x04
#define OUTPUT				0x10
#define INPUT_PULLUP		0x48
#define INPUT_PULLDOWN 		0x28
#define AF_PP				0x18
#define AF_OD		 		0x1C


#define LOW					0x00
#define HIGH				0x01

#define ADC1_DR_Address    ((uint32_t)0x4001244C)

#ifdef __cplusplus
extern "C"{
#endif

//Private function
GPIO_TypeDef *getDigitalGPIO(uint8_t PINx);
uint8_t getDigitalPin(uint8_t PINx);

// digital Function
void pinMode(uint8_t GPIO_PINx, uint8_t mode);
uint8_t digitalRead(uint8_t GPIO_PINx);
void digitalToggle(uint8_t GPIO_PINx);
void digitalWrite(uint8_t GPIO_PINx, uint8_t val);

// analog Function
void analogEnable(uint8_t ADC_PINx);
void analogDisable();
uint16_t analogRead(uint8_t GPIO_PINx);

// ADC_DMA
void ADC_DMA_Init(uint16_t ADC_values[], uint8_t len);

// PWM output functions
void analogWrite(uint8_t GPIO_PINx, uint16_t val);
void analogInit(uint8_t GPIO_PINx);


#ifdef __cplusplus
} // extern "C"
#endif


// Library function
uint8_t GPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
uint16_t GPIO_ReadInputData(GPIO_TypeDef* GPIOx);
uint8_t GPIO_ReadOutputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
uint16_t GPIO_ReadOutputData(GPIO_TypeDef* GPIOx);

void GPIO_SetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void GPIO_ResetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void GPIO_WriteBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, BitAction BitVal);
void GPIO_Write(GPIO_TypeDef* GPIOx, uint16_t PortVal);
void GPIO_ToggleBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);



#endif /* GPIO_GPIO_H_ */
