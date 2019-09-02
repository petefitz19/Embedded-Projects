/* 
 * File:   SAMD21_TRMNL.h
 * Author: austin
 *
 * Created on August 5, 2019, 1:00 PM
 * 
 * @VERSION 1.0
 * 
 * @DESCRIPTION: Simple library to allow the user to send data to the terminal using the SAMD21 XPLAINED PRO
 */

#ifndef SAMD21_TRMNL_H
#define	SAMD21_TRMNL_H

#include <proc/SAMD21A/samd21.h>
#include <string.h>
#include <stdlib.h>

void TRMNL_init(uint32_t);
void TRMNL_sendChar(char c);
void TRMNL_sendString(char *str);
void TRMNL_sendWord(uint32_t);
void TRMNL_sendNum(uint32_t);
void TRMNL_sendHex(uint32_t);
uint8_t TRMNL_read();


#endif	/* SAMD21_TRMNL_H */

