/*
 * Interrupt.h
 *
 *  Created on: Aug 16, 2018
 *      Author: ASUS PC
 */

#ifndef INTERRUPT_INTERRUPT_H_
#define INTERRUPT_INTERRUPT_H_


/* Private define */
#define RISING 	0x08
#define FALLING	0x0C
#define FALLING_RISING	0x10

#ifdef __cplusplus
extern "C"{
#endif

void attachInterrupt(uint8_t GPIO_PINx, uint8_t mode, void (*userFunc)(void));

#ifdef __cplusplus
}
#endif

#endif /* INTERRUPT_INTERRUPT_H_ */
