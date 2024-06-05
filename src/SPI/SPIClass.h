/*
 * SPI.h
 *
 *  Created on: Aug 15, 2018
 *      Author: taivb
 */

#ifndef SPI_SPICLASS_H_
#define SPI_SPICLASS_H_
/* Private define */

// SPI1 define PIN/GPIO
#define SPI1_GPIO			GPIOA
#define SPI1_RCC_GPIO 		RCC_APB2Periph_GPIOA
#define SPI1_RCC_AFIO		RCC_APB2Periph_AFIO

#define SPI1_MODE_MASTER	0x00
#define SPI1_MODE_SLAVE		0x01

#define SPI1_PIN_NSS		A4
#define SPI1_PIN_SCK		A5
#define SPI1_PIN_MISO		A6
#define SPI1_PIN_MOSI		A7

// SPI2 define PIN/GPIO
#define SPI2_GPIO			GPIOB
#define SPI2_RCC_GPIO 		RCC_APB2Periph_GPIOB
#define SPI2_RCC_AFIO		RCC_APB2Periph_AFIO

#define SPI2_MODE_MASTER	0x00
#define SPI2_MODE_SLAVE		0x01

#define SPI2_PIN_NSS		B12
#define SPI2_PIN_SCK		B13
#define SPI2_PIN_MISO		B14
#define SPI2_PIN_MOSI		B15

#define SPI_CLOCK_DIV2 		((uint16_t)0x0000)
#define SPI_CLOCK_DIV4 		((uint16_t)0x0008)
#define SPI_CLOCK_DIV8 		((uint16_t)0x0010)
#define SPI_CLOCK_DIV16 	((uint16_t)0x0018)
#define SPI_CLOCK_DIV32 	((uint16_t)0x0020)
#define SPI_CLOCK_DIV64 	((uint16_t)0x0028)
#define SPI_CLOCK_DIV128 	((uint16_t)0x0030)
#define SPI_CLOCK_DIV256 	((uint16_t)0x0038)

#ifndef LSBFIRST
#define LSBFIRST 0x0080
#endif
#ifndef MSBFIRST
#define MSBFIRST 0x0000
#endif

#define SPI_MODE0 0x0000
#define SPI_MODE1 0x0001
#define SPI_MODE2 0x0002
#define SPI_MODE3 0x0003

class SPIClass
{
public:

	SPI_InitTypeDef SPI_InitStructure;
	void SPISettings(uint32_t clockDiv, uint16_t bitOrder, uint16_t SPI_DataMode);
	void beginTransaction();
	void endTransaction(void);
	SPIClass(int SPInum = 1);
	virtual ~SPIClass();
	void begin(uint16_t mode = SPI_Mode_Master, uint8_t pin_ss = A4);
	//void begin(SPI_TypeDef *SPIx, uint16_t mode);
	uint8_t read(void);
	void write(uint8_t val);
	uint8_t transfer(uint8_t data);
	void setBitOrder(uint16_t bitOrder);
	void setDataMode(uint16_t SPI_DataMode);
	void setDataSize(uint16_t SPI_DataSize);
	void setClockDivider(uint16_t clockDiv);
	void attachInterrupt(void (*function)(void));
	void detachInterrupt();

	void transfer(void *buf, size_t count);
	uint8_t SPI_Pin_NSS;
	uint8_t SPI_Pin_SCK;
	uint8_t SPI_Pin_MISO;
	uint8_t SPI_Pin_MOSI;
private:
	SPI_TypeDef* SPInum;

	uint32_t SPI_RCC_PER;
	uint32_t SPI_RCC_GPIO;
	uint32_t SPI_RCC_AFIO;
	GPIO_TypeDef *SPI_GPIO;

};

extern SPIClass SPI;

#endif /* SPI_SPICLASS_H_ */
