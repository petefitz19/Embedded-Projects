/*
 * File:   MIDI_handling.c
 * Author: pete
 *
 * Created on August 29, 2019, 8:44 PM
 * 
 * TO DO: Refer to MIDI_buttonDebounce_ISR() to toggle between modes with the rotary encoder
 */

#include <stdbool.h>
#include <time.h>
#include "MIDI_handling.h"
#include "USART0.h"
#include "terminalPrint.h"
#include "RTC.h"

#define TIMECNT 10

enum{C, Db, D, Eb, E, F, Gb, G, Ab, A, Bb, B};
enum{NORMAL, FLIPPED, MIRRORED, RANDOM, RANDOM_88};

uint8_t flag = false;

uint8_t mode = FLIPPED;

uint8_t state;
uint8_t incomingByte;

uint8_t statusByte;
uint8_t dataByte1;
uint8_t dataByte2;

bool shuffled = false;
bool shuffled_88 = false;

uint8_t buttonOn = 0;
uint8_t val = 0;
uint8_t prevVal = 1;

/* Variables used for de-bouncing the buttons */
uint8_t currentState = 0;       // bitmap for the current state of the port
uint8_t prevState = 0;          //bitmap for the previous state of the port
uint8_t prevButtonState2 = 0;    //the past state of the 2nd button used for increasing the mode
uint8_t prevButtonState3 = 0;    //the past state of the 3rd button used for decreasing the mode
uint8_t timeSame = 0;

uint8_t noteMap_Normal[128] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
    12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
    24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 
    36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
    48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
    60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71,
    72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83,
    84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
    96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107,
    108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
    120, 121, 122, 123, 124, 125, 126, 127,
};

uint8_t noteMap_Flipped[128] = {
    0, 0, 0, 127, 126, 125, 124, 123, 122, 121, 120, 119,
    118, 117, 116, 115, 114, 113, 112, 111, 110, 109, 108, 107,
    106, 105, 104, 103, 102, 101, 100, 99, 98, 97, 96, 95,
    94, 93, 92, 91, 90, 89, 88, 87, 86, 85, 84, 83,
    82, 81, 80, 79, 78, 77, 76, 75, 74, 73, 72, 71,
    70, 69, 68, 67, 66, 65, 64, 63, 62, 61, 60, 59,
    58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47,
    46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35,
    34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23,
    22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11,
    10, 9, 8, 7, 6, 5, 4, 3,
};

uint8_t noteMap_Mirrored[128] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
    12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
    24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 
    36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
    48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
    60, 61, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53,
    52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41,
    40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29,
    28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17,
    16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 
    4, 3, 2, 1, 0, 0, 0, 0,
};

uint8_t noteMap_Random[128] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
    12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
    24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 
    36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
    48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
    60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71,
    72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83,
    84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
    96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107,
    108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
    120, 121, 122, 123, 124, 125, 126, 127,
};

uint8_t noteMap_Random_88[128] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 21, 22, 23,
    24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 
    36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
    48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
    60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71,
    72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83,
    84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
    96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107,
    108, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
};

void MIDI_messageReceive_ISR(){
    incomingByte = USART0_read_ISR();
    if (state == 0) {
      if (incomingByte > 0x7F && incomingByte < 0xF0){ // this reads all statusbytes
          statusByte = incomingByte;                   // incoming byte is greater than 01111111 and less than 11110000
          state = 1; //statusByte state
      }
      else {
        state = 0;
      }
    }

    else if (state == 1) { //check for note on/off messages
      if ((statusByte >= 0x80) && (statusByte < 0x90)) { //if statusByte is a NoteOff message
        dataByte1 = incomingByte;
        PORTF.OUTSET = PIN5_bm;
        state = 2;
      }
      else if ((statusByte >= 0x90) && (statusByte < 0xA0)) { //if statusByte is a NoteOn message
        dataByte1 = incomingByte;
        PORTF.OUTCLR = PIN5_bm;
        state = 2;
      }
      else {
        state = 0;
      }
    }

    else if(state == 2){
      dataByte2 = incomingByte;
      //getMessage(statusByte, dataByte1, dataByte2);
      state = 0;
      flag = true;
    }
}

void MIDI_buttonDebounce_ISR(){
    currentState = PORTF.IN & 0x7F;     //TO DO: CHECK WITH 0xC
    
    if(currentState == prevState){
        timeSame++;
    }
    else{
        timeSame = 0;
    }
    
    if(timeSame > TIMECNT){
        timeSame = 0;
//        buttonState = currentState & 0x3F;
        if(!(currentState & PIN3_bm) && (prevButtonState3)){
            //toggles the led on and off
            if(buttonOn){
                PORTF.OUTCLR = PIN5_bm;
                buttonOn = 0;
            }
            else{
                PORTF.OUTSET = PIN5_bm;
                buttonOn = 1;
            }
            mode++;
            if(mode > 4){
                mode = 0;
            }
        }
        else if(!(currentState & PIN2_bm) && (prevButtonState2)){
            //toggles the led on and off
            if(buttonOn){
                PORTF.OUTCLR = PIN5_bm;
                buttonOn = 0;
            }
            else{
                PORTF.OUTSET = PIN5_bm;
                buttonOn = 1;
            }
            mode--;
            if(mode == 255){
                mode = 4;
            }
        }
        TRMNL_sendNum(mode);
        TRMNL_sendString("\r\n");
        prevButtonState2 = currentState &  PIN2_bm;
        prevButtonState3 = currentState &  PIN3_bm;
    }
    prevState = currentState;
    
    RTC_periodicIntISR(1);
}


void MIDI_transmitHandler(){
    if(flag){
        flag = false;
        if(mode == NORMAL){
            USART0_sendByte(statusByte);
            USART0_sendByte(dataByte1);
            USART0_sendByte(dataByte2);
        }
        else if(mode == FLIPPED){
            USART0_sendByte(statusByte);
            USART0_sendByte(noteMap_Flipped[dataByte1]);
            USART0_sendByte(dataByte2);
        }
        else if(mode == MIRRORED){
            USART0_sendByte(statusByte);
            USART0_sendByte(noteMap_Mirrored[dataByte1]);
            USART0_sendByte(dataByte2);
        }
        else if(mode == RANDOM){
            if(!shuffled){
                randomize(noteMap_Random, 128);
                shuffled = true;
            }
            USART0_sendByte(statusByte);
            USART0_sendByte(noteMap_Random[dataByte1]);
            USART0_sendByte(dataByte2);  
        }
        else if(mode == RANDOM_88){
            if(!shuffled_88){
                uint8_t temp[88];
                uint8_t count = 0;
                for(uint8_t i = 21; i < 109; i++){
                    temp[count] = i;
                    count++;
                }
                randomize(temp, 88);
                count = 0;
                for(uint8_t i = 21; i < 109; i++){
                    noteMap_Random_88[i] = temp[count];
                    count++;
                }
                shuffled_88 = true;
            }
            USART0_sendByte(statusByte);
            USART0_sendByte(noteMap_Random_88[dataByte1]);
            USART0_sendByte(dataByte2); 
        }
        else{
            USART0_sendByte(statusByte);
            USART0_sendByte(dataByte1);
            USART0_sendByte(dataByte2);
        }
    }
}

void swap(uint8_t *a, uint8_t *b){
    uint8_t temp = *a;
    *a = *b;
    *b = temp;
}

void randomize(uint8_t arr[], uint8_t n){
    srand(time(NULL));
    
    for(uint8_t i = n - 1; i > 0; i--){
        uint8_t j = rand() % (i + 1);
        swap(&arr[i], &arr[j]);
    }
}




