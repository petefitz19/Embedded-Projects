/*
 * File:   USART_0.h
 * Author: Pete
 *
 * Created on June 28, 2019, 11:46 AM
 * 
 * @VERSION 1.1
 */

#ifndef USART0_H
#define	USART0_H

//user defined baud rate
#define USART0_BAUDRATE 31250

#include <avr/io.h>


#define USART0_BAUD_RATE(BAUD_RATE) ((float)(3333333.33333 * 64 / (16 * (float)BAUD_RATE)) + 0.5)

void USART0_init(uint8_t, uint8_t);
void USART0_sendByte(uint8_t);
void USART0_sendBytes(uint8_t*, uint8_t);
uint8_t USART0_read();
uint8_t USART0_read_ISR();

#endif	/* USART_0_H */

