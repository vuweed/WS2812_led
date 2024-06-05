/*
 * SPI.cpp
 *
 *  Created on: Aug 15, 2018
 *      Author: taivb
 */
#include <include.h>
#include <GPIO/GPIO.h>
#include <SPI/SPIClass.h>

static volatile voidFuncPtr receiveEvent[2] =
{ NULL, NULL };

SPIClass::SPIClass(int SPI)
{

	switch (SPI)
	{
	case 1:
		SPInum = SPI1;

		SPI_RCC_PER = RCC_APB2Periph_SPI1;

		SPI_RCC_GPIO = SPI1_RCC_GPIO;
		SPI_RCC_AFIO = SPI1_RCC_AFIO;
		SPI_GPIO = SPI1_GPIO;

		SPI_Pin_NSS = getDigitalPin(SPI1_PIN_NSS);
		SPI_Pin_SCK = getDigitalPin(SPI1_PIN_SCK);
		SPI_Pin_MISO = getDigitalPin(SPI1_PIN_MISO);
		SPI_Pin_MOSI = getDigitalPin(SPI1_PIN_MOSI);
		break;

	case 2:
		SPInum = SPI2;

		SPI_RCC_PER = RCC_APB1Periph_SPI2;

		SPI_RCC_GPIO = SPI2_RCC_GPIO;
		SPI_RCC_AFIO = SPI2_RCC_AFIO;
		SPI_GPIO = SPI2_GPIO;

		SPI_Pin_NSS = getDigitalPin(SPI2_PIN_NSS);
		SPI_Pin_SCK = getDigitalPin(SPI2_PIN_SCK);
		SPI_Pin_MISO = getDigitalPin(SPI2_PIN_MISO);
		SPI_Pin_MOSI = getDigitalPin(SPI2_PIN_MOSI);
		break;

	default:
		break;
	}

	this->SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	this->SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	this->SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	this->SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	this->SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	this->SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	this->SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	this->SPI_InitStructure.SPI_CRCPolynomial = 7;
}

SPIClass::~SPIClass()
{
	// TODO Auto-generated destructor stub
}

void SPIClass::SPISettings(uint32_t clockDiv, uint16_t bitOrder,
		uint16_t SPI_DataMode)
{
	this->SPI_InitStructure.SPI_FirstBit = bitOrder;
	this->SPI_InitStructure.SPI_CPHA = SPI_DataMode & 0x0001;
	this->SPI_InitStructure.SPI_CPOL = SPI_DataMode & 0x0002;
	this->SPI_InitStructure.SPI_BaudRatePrescaler = clockDiv;
}

void SPIClass::beginTransaction()
{
	SPI_Init(SPInum, &SPI_InitStructure);
}

void SPIClass::endTransaction(void)
{

}

/* SPI1
 * STM32F1		pin		ARDUINO UNO
 *
 * 	 A4---------NSS---------10
 * 	 A5---------SCK---------13
 * 	 A6-------- MISO--------12
 * 	 A7---------MOSI--------11
 *
 * 	 mode: SPI_Mode_Master, SPI_Mode_Slave
 */
void SPIClass::begin(uint16_t mode, uint8_t pin_ss)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// enable clock
	if (SPInum == SPI1)
	{
		RCC_APB2PeriphClockCmd(SPI_RCC_PER, ENABLE);
	}
	else if (SPInum == SPI2)
	{
		RCC_APB1PeriphClockCmd(SPI_RCC_PER, ENABLE);
	}
	RCC_APB2PeriphClockCmd(SPI_RCC_GPIO | SPI_RCC_AFIO, ENABLE);

	/* GPIO config */
// Master
	if (mode == SPI_Mode_Master)
	{
		//GPIo pin MOSI SCK config
		GPIO_InitStructure.GPIO_Pin = (1 << SPI_Pin_MOSI) | (1 << SPI_Pin_SCK);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(SPI_GPIO, &GPIO_InitStructure);

		//GPIO pin SS config
		GPIO_InitStructure.GPIO_Pin = (1 << pin_ss);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(SPI_GPIO, &GPIO_InitStructure);

		//GPIO pin MISO config
		GPIO_InitStructure.GPIO_Pin = (1 << SPI_Pin_MISO);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(SPI_GPIO, &GPIO_InitStructure);
	}
// Slaver
	else if (mode == SPI_Mode_Slave)
	{
		//GPIo pin MOSI SCK config
		GPIO_InitStructure.GPIO_Pin = (1 << SPI_Pin_MOSI) | (1 << SPI_Pin_SCK);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(SPI_GPIO, &GPIO_InitStructure);

		//GPIO pin SS config
		GPIO_InitStructure.GPIO_Pin = (1 << SPI_Pin_NSS);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(SPI_GPIO, &GPIO_InitStructure);

		//GPIO pin MISO config
		GPIO_InitStructure.GPIO_Pin = (1 << SPI_Pin_MISO);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(SPI_GPIO, &GPIO_InitStructure);
	}
	/* end gpio config */
	//SPI config
	this->SPI_InitStructure.SPI_Mode = mode;
	SPI_Init(SPInum, &this->SPI_InitStructure);

	// enable SPI1
	SPI_Cmd(SPInum, ENABLE);
}
/*
 * SPi transfer function
 * use to tranfer 1 byte and receive 1 byte from slave
 * parameter: data - 1byte data to send
 */
uint8_t SPIClass::transfer(uint8_t data)
{
	// wait for Transfer line is free
	while (SPI_I2S_GetFlagStatus(SPInum, SPI_I2S_FLAG_TXE) == RESET)
		;

	/* Check the parameters */
	assert_param(IS_SPI_ALL_PERIPH(SPInum));

	/* Write in the DR register the data to be sent */
	SPInum->DR = data;

	// wait for receive line is free
	while (SPI_I2S_GetFlagStatus(SPInum, SPI_I2S_FLAG_RXNE) == RESET)
		;

	/* Check the parameters */
	assert_param(IS_SPI_ALL_PERIPH(SPInum));

	/* Return the data in the DR register */
	return SPInum->DR;
}
void SPIClass::transfer(void *buf, size_t count)
{
	if (count == 0)
		return;
	uint8_t *p = (uint8_t *) buf;

	// wait for Transfer line is free
	while (SPI_I2S_GetFlagStatus(SPInum, SPI_I2S_FLAG_TXE) == RESET)
		;

	/* Check the parameters */
	assert_param(IS_SPI_ALL_PERIPH(SPInum));

	/* Write in the DR register the data to be sent */
	SPInum->DR = *p;
	while (--count > 0)
	{
		uint8_t out = *(p + 1);
		// wait for receive line is free
		while (SPI_I2S_GetFlagStatus(SPInum, SPI_I2S_FLAG_RXNE) == RESET)
			;
		/* Check the parameters */
		assert_param(IS_SPI_ALL_PERIPH(SPInum));
		uint8_t in = SPInum->DR;
		SPInum->DR = out;
		*p++ = in;
	}
	while (SPI_I2S_GetFlagStatus(SPInum, SPI_I2S_FLAG_RXNE) == RESET)
		;
	/* Check the parameters */
	assert_param(IS_SPI_ALL_PERIPH(SPInum));
	*p = SPInum->DR;
}
uint8_t SPIClass::read(void)
{
	/* Check the parameters */
	assert_param(IS_SPI_ALL_PERIPH(SPInum));
	/* Return the data in the DR register */
	return SPInum->DR;
}

void SPIClass::write(uint8_t val)
{
	/* Check the parameters */
	assert_param(IS_SPI_ALL_PERIPH(SPInum));
	/* Return the data in the DR register */
	SPInum->DR = val;
}

void SPIClass::setBitOrder(uint16_t bitOrder)
{
	this->SPI_InitStructure.SPI_FirstBit = bitOrder;
	SPI_Init(SPInum, &SPI_InitStructure);
}

void SPIClass::setDataMode(uint16_t SPI_DataMode)
{
	this->SPI_InitStructure.SPI_CPHA = SPI_DataMode & 0x0001;
	this->SPI_InitStructure.SPI_CPOL = SPI_DataMode & 0x0002;
	SPI_Init(SPInum, &SPI_InitStructure);
}
/**
 * @brief  Configures the data size for the selected SPI.
 * @param  SPIx: where x can be 1, 2 or 3 to select the SPI peripheral.
 * @param  SPI_DataSize: specifies the SPI data size.
 *   This parameter can be one of the following values:
 *     @arg SPI_DataSize_16b: Set data frame format to 16bit
 *     @arg SPI_DataSize_8b: Set data frame format to 8bit
 * @retval None
 */
void SPIClass::setDataSize(uint16_t SPI_DataSize)
{
	/* Check the parameters */
	assert_param(IS_SPI_ALL_PERIPH(SPInum));
	assert_param(IS_SPI_DATASIZE(SPI_DataSize));
	/* Clear DFF bit */
	SPInum->CR1 &= (uint16_t) ~SPI_DataSize_16b;
	/* Set new DFF bit value */
	SPInum->CR1 |= SPI_DataSize;
}
void SPIClass::setClockDivider(uint16_t clockDiv)
{
	this->SPI_InitStructure.SPI_BaudRatePrescaler = clockDiv;
	SPI_Init(SPInum, &this->SPI_InitStructure);
}
// sets function called on slave write
void SPIClass::attachInterrupt(void (*function)(void))
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* 1 bit for pre-emption priority, 3 bits for subpriority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	/* Configure and enable SPI_MASTER interrupt -------------------------------*/
	if (SPInum == SPI1)
	{
		NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		receiveEvent[0] = function;
	}
	else if (SPInum == SPI2)
	{
		NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

		receiveEvent[1] = function;
	}
	NVIC_Init(&NVIC_InitStructure);

	SPI_I2S_ITConfig(SPInum, SPI_I2S_IT_RXNE, ENABLE);
}

void SPIClass::detachInterrupt()
{
	SPI_I2S_ITConfig(SPInum, SPI_I2S_IT_RXNE, DISABLE);
}

void SPI1_IRQHandler(void)
{
	if (SPI_I2S_GetITStatus(SPI1, SPI_I2S_IT_RXNE) != RESET)
	{
		SPI_I2S_ReceiveData(SPI1);
		if (receiveEvent[0] != NULL)
			receiveEvent[0]();
	}
}

void SPI2_IRQHandler(void)
{
	if (SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_RXNE) != RESET)
	{
		SPI_I2S_ReceiveData(SPI2);
		if (receiveEvent[1] != NULL)
			receiveEvent[1]();
	}
}

SPIClass SPI = SPIClass();
