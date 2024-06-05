/*
 * Serial.h
 *
 *  Created on: Aug 15, 2018
 *      Author: taivb
 */

#ifndef SERIAL_SERIAL_H_
#define SERIAL_SERIAL_H_

#include <stddef.h>
#include <stdint.h>
#include <Print.h>


#define USART1_GPIO			GPIOA
#define USART1_RCC_GPIO 	RCC_APB2Periph_GPIOA
#define USART1_RCC_AFIO		RCC_APB2Periph_AFIO
#define USART1_PIN_TX		A9
#define USART1_PIN_RX		A10

#define USART2_GPIO			GPIOA
#define USART2_RCC_GPIO 	RCC_APB2Periph_GPIOA
#define USART2_RCC_AFIO		RCC_APB2Periph_AFIO
#define USART2_PIN_TX		A2
#define USART2_PIN_RX		A3

#define USART3_GPIO			GPIOB
#define USART3_RCC_GPIO 	RCC_APB2Periph_GPIOB
#define USART3_RCC_AFIO		RCC_APB2Periph_AFIO
#define USART3_PIN_TX		B10
#define USART3_PIN_RX		B11

#define SERIAL_RX_BUFFER_SIZE 16

class HardwareSerial : public Print
{
public:
	HardwareSerial(int USART = 1);
	virtual ~HardwareSerial();
	void begin(uint32_t baud);
	void begin(USART_TypeDef *USARTx, uint32_t baud);
	void end();
	void reset(void);
	uint8_t read(void);
	size_t write(uint8_t val);
	size_t write(uint8_t* buf, uint16_t len);
	int available(void);
	int peek(void);

    volatile uint8_t _rx_buffer_head;
    volatile uint8_t _rx_buffer_tail;
    uint8_t _rx_buffer[SERIAL_RX_BUFFER_SIZE];
private:
	USART_TypeDef* USARTnum;

	uint32_t USART_RCC_PER;
	uint32_t USART_RCC_GPIO;
	uint32_t USART_RCC_AFIO;
	GPIO_TypeDef *USART_GPIO;

	uint8_t USART_Pin_Tx;
	uint8_t USART_Pin_Rx;
};

// interrupt handler
extern void SerialEvent(void) __attribute__((weak));
extern void Serial2Event(void) __attribute__((weak));
extern void Serial3Event(void) __attribute__((weak));

extern HardwareSerial Serial, Serial2, Serial3;
#endif /* SERIAL_SERIAL_H_ */
