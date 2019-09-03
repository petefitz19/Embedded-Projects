/* 
 * File:   MIDI_handling.h
 * Author: pete
 *
 * Created on August 29, 2019, 8:45 PM
 */

#ifndef MIDI_HANDLING_H
#define	MIDI_HANDLING_H

#include <avr/io.h>

void MIDI_messageReceive_ISR();
void MIDI_buttonDebounce_ISR();
void MIDI_transmitHandler();
void swap(uint8_t *a, uint8_t *b);
void randomize(uint8_t arr[], uint8_t n);

#endif	/* MIDI_HANDLING_H */

