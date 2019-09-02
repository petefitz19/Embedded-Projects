/*
 * File:   main.c
 * Author: pete
 *
 * Created on August 26, 2019, 4:28 PM
 */


#include "ADC.h"
#include "SAMD21_TRMNL.h"
#include "SERCOM0_UART.h"
#include "samd21ClockSet.h"

#define Y_MIDPOINT 1013
#define X_MIDPOINT 1052

#define Y_PORT    1
#define Y_PIN     9
#define Y_MUXPOS  3

#define X_PORT    1
#define X_PIN     8
#define X_MUXPOS  2

uint8_t dir1 = 0;
uint8_t dir2 = 0;

uint8_t prev_dir1 = 0;
uint8_t prev_dir2 = 0;

void getPosition();

int main(void) {
    setFreq48MHz();
    
    SERCOM0_UART_init(0, 31250);
    
    TRMNL_init(115200);
    
    ADC_addChannel(Y_PORT, Y_PIN);
    ADC_addChannel(X_PORT, X_PIN);
    ADC_init();
    
    while (1) {
        getPosition();
    }
}

void getPosition(void){
    dir1 = ADC_read(Y_MUXPOS) >> 8;
    dir2 = ADC_read(X_MUXPOS) >> 8;
    
    dir1 = (dir1 - 13) * (127.0/234.0);
    dir2 = (dir2 - 13) * (127.0/234.0);
    if(dir1 != prev_dir1){
        SERCOM0_UART_sendByte(0xB0);
        SERCOM0_UART_sendByte(0x01);
        SERCOM0_UART_sendByte(dir1);
        prev_dir1 = dir1;
    }
    if(dir2 != prev_dir2){
        TRMNL_sendString("Two: ");
        TRMNL_sendNum(dir2);
        TRMNL_sendString("\r\n");
        prev_dir2 = dir2;
    }
}

