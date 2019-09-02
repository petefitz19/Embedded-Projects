/*
 * File:   USART_0.c
 * Author: Pete
 *
 * Created on June 28, 2019, 11:46 AM
 * 
 * @VERSION 1.1
 */

#include "USART0.h"

/* @NAME: USART0_init
 * 
 * @DESCRIPTION: Initializes the USART0 module
 * 
 * @NOTE: BAUDRATE is defined in the header file
 * 
 * @PARAM: 
 *          mux: allows the user to select what pins they want
 * 
 *          IE: allows the user to select if they want interrupts enabled (1) or not (0)
 */
void USART0_init(uint8_t mux, uint8_t IE){
    if(mux == 1){
        PORTMUX.USARTROUTEA |= PORTMUX_USART0_ALT1_gc;
        PORTA.DIRSET = PIN4_bm; //sets the tx pin as output
        PORTA.DIRCLR = PIN5_bm; //sets the rx pin as input
    }
    else{
        PORTMUX.USARTROUTEA |= PORTMUX_USART0_DEFAULT_gc;
        PORTA.DIRSET = PIN0_bm; //sets the tx pin as output
        PORTA.DIRCLR = PIN1_bm; //sets the rx pin as input
    }
    USART0.BAUD = (uint16_t)USART0_BAUD_RATE(USART0_BAUDRATE); //sets the baud rate
    if(IE){
        USART0.CTRLA = USART_RXCIE_bm;
    }
    USART0.CTRLB |= USART_TXEN_bm; //enables tx
    USART0.CTRLB |= USART_RXEN_bm; //enables rx
}

/* @NAME: USART0_sendByte
 * 
 * @DESCRIPTION: takes a byte and sends it on the tx line
 * 
 * @PARAM:
 *          byte: the byte the user wishes to transmit
 */
void USART0_sendByte(uint8_t byte){
    while (!(USART0.STATUS & USART_DREIF_bm)){ //Waits for the data register to be empty
        ;
    }        
    USART0.TXDATAL = byte;
}

/* @NAME: USART0_sendByte
 * 
 * @DESCRIPTION: takes multiple bytes and sends them on the tx line
 * 
 * @PARAM:
 *          data: the array of bytes the user wishes to transmit
 * 
 *          size: the size of the data array
 */
void USART0_sendBytes(uint8_t *data, uint8_t size){
    for(uint8_t i = 0; i < size; i++){
        USART0_sendByte(data[i]);
    }
}

/* @NAME: USART0_read
 * 
 * @DESCRIPTION: reads a value from the terminal and returns it
 *
 */
uint8_t USART0_read(){
    while(!(USART0.STATUS & USART_RXCIF_bm)){ //waits for the data to be received
        ;
    }

    return USART0.RXDATAL; //returns the value of the receive register
}

/* @NAME: USART0_read_ISR()
 * 
 * @DESCRIPTION: This function should be used within the ISR in main to return the 
 *               value of the receive register upon an interrupt
 *
 */
uint8_t USART0_read_ISR(){
    return USART0.RXDATAL;
}
