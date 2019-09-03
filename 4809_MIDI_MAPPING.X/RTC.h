/* 
 * File:   RTC.h
 * Author: austin
 *
 * Created on July 18, 2019, 10:19 AM
 * 
 * @DESCRIPTION: Library to use the RTC peripheral. Allows the user to set variable
 *              periodic interrupts and also read in the time.
 * 
 * @NOTE: The clock will reset after 65536 milliseconds
 * 
 * VERSION: 1.1
 */

#ifndef RTC_H
#define	RTC_H

#include <avr/io.h>
#include <util/atomic.h>

void RTC_init();
void RTC_periodicIntSet(uint16_t interruptTime);
void RTC_periodicIntISR(uint16_t interruptTime);
void RTC_periodicIntDisable();
uint16_t RTC_getTime();

#endif	/* RTC_H */

