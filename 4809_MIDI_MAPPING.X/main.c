/*
 * File:   main.c
 * Author: pete
 *
 * Created on August 17, 2019, 9:55 PM
 */

#define F_CPU 3333333
#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include "USART0.h"
#include "MIDI_handling.h"
#include "terminalPrint.h"
#include "RTC.h"

enum{NORMAL, FLIPPED, MIRRORED, RANDOM, RANDOM_88};

int main(void){
    TRMNL_init();
    
    sei();
    
    USART0_init(0, 1);
    
    RTC_init();
    RTC_periodicIntSet(1);
    
    PORTF.DIRSET = PIN5_bm;
    PORTF.OUTSET = PIN5_bm;
    
    PORTF.DIRCLR = PIN2_bm;
    PORTF.PIN2CTRL = PORT_PULLUPEN_bm;
    
    PORTF.DIRCLR = PIN3_bm;
    PORTF.PIN3CTRL = PORT_PULLUPEN_bm;
    
    while (1) {
        MIDI_transmitHandler();
    }
}

ISR(USART0_RXC_vect){
    MIDI_messageReceive_ISR();
}

ISR(RTC_CNT_vect){
    MIDI_buttonDebounce_ISR();
}