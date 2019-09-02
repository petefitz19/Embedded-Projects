/*
 * File:   terminalPrint.c
 * Author: austin
 *
 * Created on May 31, 2019, 11:14 AM
 * 
 * @VERSION 1.3
 */

#include "terminalPrint.h"

/* @NAME: TRMNL_init
 * 
 * @DESCRIPTION: Initializes the usart3 module so that it is possible
 *               to print to a serial terminal
 * 
 * @NOTE: BAUDRATE is defined in the header file
 */
void TRMNL_init(){
    PORTB.DIR &= ~PIN1_bm; //sets the rx pin as input
    PORTB.DIR |= PIN0_bm; //sets the tx pin as output
    
    USART3.BAUD = (uint16_t)TRMNL_BAUD_RATE(TRMNL_BAUDRATE); //sets the baud rate
    USART3.CTRLB |= USART_TXEN_bm; //enables tx
    USART3.CTRLB |= USART_RXEN_bm; //enables rx
}

/* @NAME: TRMNL_sendChar
 * 
 * @DESCRITPTION: sends a character to the terminal
 * 
 * @PARAM: 
 *          c: character to send to the terminal
 *
 */
void TRMNL_sendChar(char c)
{
    while (!(USART3.STATUS & USART_DREIF_bm)){ //waits for the send register to become available
        ;
    }
    USART3.TXDATAL = c; //puts the character in the send register
}

/* @NAME: TRMNL_sendString
 * 
 * @DESCRIPTION: sends a string to the terminal
 * 
 * @PARAM:
 *          *str: pointer to the string
 *
 */
void TRMNL_sendString(char *str){
    for(size_t i = 0; i < strlen(str); i++){
        TRMNL_sendChar(str[i]);
    }
}

/* @NAME: TRMNL_sendByte
 * 
 * @DESCRIPTION: takes a number and outputs it out as an ascii number as a binary number
 * 
 * @PARAM:
 *          byte: number to convert to a binary number and send to the terminal
 *
 */
void TRMNL_sendByte(uint8_t byte){
    char buff[9] = {0};
    itoa(byte, buff, 2);
    TRMNL_sendString(buff);
}

/* @NAME: TRMNL_sendNum
 * 
 * @DESCRIPTION: takes a number and outputs it out as an ascii number as a decimal number
 * 
 * @PARAM:
 *          num: number to be converted and sent to the terminal
 *
 */
void TRMNL_sendNum(uint8_t num){
    char buff[4] = {0};
    itoa(num, buff, 10);
    TRMNL_sendString(buff);
}

/* @NAME: TRMNL_sendHex
 * 
 * @DESCRITPION: takes a number and outputs it out as an ascii number as a hex number
 * 
 * @PARAM:
 *          num: number to be converted to a hex and sent out to the terminal
 *
 */
void TRMNL_sendHex(uint8_t num){
    char buff [3] = {0};
    itoa(num, buff, 16);
    TRMNL_sendString("0x");
    TRMNL_sendString(buff);
}

/* @NAME: TRMNL_read
 * 
 * @DESCRIPTION: reads a value from the terminal and returns it
 *
 */
uint8_t TRMNL_read(){
    while(!(USART3.STATUS & USART_RXCIF_bm)){ //waits for the data to be received
        ;
    }

    return USART3.RXDATAL; //returns the value of the receive register
}

