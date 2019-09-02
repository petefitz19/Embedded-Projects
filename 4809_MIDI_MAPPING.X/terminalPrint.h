/* 
 * File:   terminalPrint.h
 * Author: austin
 *
 * Created on May 31, 2019, 11:06 AM
 *	
 * Simple Library to send bytes to the terminal of a computer
 * 
 * @VERSION: 1.3
 * 
 * @NOTE: To set the baud rate change the BAUDRATE definition
 *
 */

//user defined baudrate
#define TRMNL_BAUDRATE 115200

#ifndef TERMINALPRINT_H
#define	TERMINALPRINT_H

#include <string.h>
#include <avr/io.h>
#include <stdlib.h>

#define TRMNL_BAUD_RATE(BAUD_RATE) ((float)(3333333.33333 * 64 / (16 * (float)BAUD_RATE)) + 0.5)

void TRMNL_init();
void TRMNL_sendChar(char c);
void TRMNL_sendString(char *str);
void TRMNL_sendByte(uint8_t);
void TRMNL_sendNum(uint8_t);
void TRMNL_sendHex(uint8_t);
uint8_t TRMNL_read();

#endif	/* TERMINALPRINT_H */

