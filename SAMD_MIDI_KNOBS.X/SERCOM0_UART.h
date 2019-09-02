/* 
 * File:   SERCOM0_UART.h
 * Author: austin
 *
 * Created on August 5, 2019, 3:33 PM
 * 
 *  @VERSION 1.0
 */

#ifndef SERCOM0_UART_H
#define	SERCOM0_UART_H

#include <proc/SAMD21A/samd21.h>

void SERCOM0_UART_init(uint8_t muxOption, uint32_t baud);
void SERCOM0_UART_sendByte(uint8_t byte);
void SERCOM0_UART_sendBytes(uint8_t *data, uint32_t size);
uint8_t SERCOM0_UART_hasData();
uint16_t SERCOM0_UART_getData();

#endif	/* SERCOM0_UART_H */

