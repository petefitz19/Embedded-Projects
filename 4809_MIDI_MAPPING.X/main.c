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
#include "SH1106_driver.h"
#include "PicoPixel.h"

enum{NORMAL, FLIPPED, MIRRORED, RANDOM, RANDOM_88};

int main(void){
    
    TWI0_init(2);                                                               //SDA is on pin PC2 and SCL is on pin PC3
    
    sei();                                                                      //Enable global Interrupts
    
    SH1106_init(SH1106_EXTERNALVCC);                                            //Initialize the OLED w/ external power supply
    SH1106_setFont(&Picopixel);                                                 //Set font to Picopixel
    SH1106_setCursor(2,15);                                                     //Cursor Starting Position
    SH1106_setTextSize(3);                                                      //Default Font size x3
    SH1106_display();                                                           //Display the Startup Screen
    _delay_ms(2000);                                                            //Wait ~2 secs
    SH1106_clearDisplay();                                                      //Clear the display
    SH1106_display();                                                           //Display the cleared display
    
    TRMNL_init();                                                               //Initialize Terminal (Testing Purposes))
    
    USART0_init(0, 1);                                                          //A4 = TX, A5 = RX, Interrupts enabled
    
    RTC_init();                                                                 //Initialize the Real time Clock (Debouncing)
    RTC_periodicIntSet(1);                                                      //Set the periodic interrupt to trigger every 1 clock cycle
    
    /* LED MIDI INDICATOR INITIALIZATION */
    PORTF.DIRSET = PIN5_bm;
    PORTF.OUTSET = PIN5_bm;
    
    /* BUTTON INPUT INITIALIZATION */
    PORTF.DIRCLR = PIN2_bm;
    PORTF.PIN2CTRL = PORT_PULLUPEN_bm;
    PORTF.DIRCLR = PIN3_bm;
    PORTF.PIN3CTRL = PORT_PULLUPEN_bm;
    
    MIDI_displayHandler();
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