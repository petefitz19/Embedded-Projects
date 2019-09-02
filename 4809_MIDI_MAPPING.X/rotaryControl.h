/* 
 * File:   rotaryControl.h
 * Author: austin
 *
 * Created on June 19, 2019, 2:21 PM
 * @VERSION: 1.3
 * 
 * @NOTE: PINA and PINB must be on the same port. PINB must also be a higher pin number than
 *        PINA. To change the PORT and the pin numbers changes the defines: PORT, PINA, PINB.
 *        Change the port and pins in the .C file.
 */

#ifndef ROTARYCONTROL_H
#define	ROTARYCONTROL_H

#include <avr/io.h>

void ROTARY_init(uint8_t);
int8_t ROTARY_ISR();

#endif	/* ROTARYCONTROL_H */

