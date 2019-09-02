/* 
 * File:   SAMD21_TRMNL.c
 * Author: austin
 *
 * Created on August 5, 2019, 1:01 PM
 * 
 * @VERSION 1.0
 * 
 * @DESCRIPTION: Simple library to allow the user to send data to the terminal using the SAMD21 XPLAINED PRO
 */

#define F_CPU 48000000	// 48MHz

#include "SAMD21_TRMNL.h"


/* @NAME: TRMNL_init
 * 
 * @DESCRIPTION: Initializes the SERCOM3 module so that it is possible
 *               to print to a serial terminal
 * 
 * @PARAM:
 *          baud: desired baud rate
 */
void TRMNL_init(uint32_t baud) {
	
	// FBAUD = ( fREF/ S) (1 ? BAUD/65,536)
	// FBAUD = baud frequency
	// fref ? SERCOM generic clock frequency
	// S ? Number of samples per bit
	// BAUD ? BAUD register value
	uint64_t br = (uint64_t)65536 * (F_CPU - 16 * baud) / F_CPU;	// Variable for baud rate

	PORT->Group[0].DIRSET.reg = (1 << 22);							// Set TX Pin direction to output
	PORT->Group[0].PINCFG[22].reg |= PORT_PINCFG_INEN;				// Set TX Pin config for input enable (required for usart)
	PORT->Group[0].PINCFG[22].reg |= PORT_PINCFG_PMUXEN;			// enable PMUX
	PORT->Group[0].PMUX[22>>1].bit.PMUXE = PORT_PMUX_PMUXE_C_Val;	// Set the PMUX bit (if pin is even, PMUXE, if odd, PMUXO)
	
	PORT->Group[0].DIRCLR.reg = (1 << 23);							// Set RX Pin direction to input
	PORT->Group[0].PINCFG[23].reg |= PORT_PINCFG_INEN;				// Set RX Pin config for input enable
	PORT->Group[0].PINCFG[23].reg &= ~PORT_PINCFG_PULLEN;			// enable pullup/down resistor
	PORT->Group[0].PINCFG[23].reg |= PORT_PINCFG_PMUXEN;			// enable PMUX
	PORT->Group[0].PMUX[23>>1].bit.PMUXO = PORT_PMUX_PMUXE_C_Val;	// Set the PMUX bit (if pin is even, PMUXE, if odd, PMUXO)
	
	PM->APBCMASK.reg |= PM_APBCMASK_SERCOM3;						// Set the PMUX for SERCOM3 and turn on module in PM

	// Generic clock ?SERCOM3_GCLK_ID_CORE? uses GCLK Generator 0 as source (generic clock source can be
	// changed as per the user needs), so the SERCOM3 clock runs at 8MHz from OSC8M
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(SERCOM3_GCLK_ID_CORE) | GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(0);

	// By setting the DORD bit LSB is transmitted first and setting the RXPO bit as 1
	// corresponding SERCOM PAD[1] will be used for data reception, PAD[0] will be used as TxD
	// pin by setting TXPO bit as 0, 16x over-sampling is selected by setting the SAMPR bit as
	// 0, Generic clock is enabled in all sleep modes by setting RUNSTDBY bit as 1,
	// USART clock mode is selected as USART with internal clock by setting MODE bit into 1.
	SERCOM3->USART.CTRLA.reg = SERCOM_USART_CTRLA_DORD | SERCOM_USART_CTRLA_MODE_USART_INT_CLK | SERCOM_USART_CTRLA_RXPO(1/*PAD1*/) | SERCOM_USART_CTRLA_TXPO(0/*PAD0*/);
	
	// 8-bits size is selected as character size by setting the bit CHSIZE as 0,
	// TXEN bit and RXEN bits are set to enable the Transmitter and receiver
	SERCOM3->USART.CTRLB.reg = SERCOM_USART_CTRLB_RXEN | SERCOM_USART_CTRLB_TXEN | SERCOM_USART_CTRLB_CHSIZE(0/*8 bits*/);
	
	// baud register value corresponds to the device communication baud rate
	SERCOM3->USART.BAUD.reg = (uint16_t)br;

	// SERCOM3 peripheral enabled
	SERCOM3->USART.CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;
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
	while (!(SERCOM3->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_DRE));
	SERCOM3->USART.DATA.reg = c;
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

/* @NAME: TRMNL_sendWord
 * 
 * @DESCRIPTION: takes a number and outputs it out as an ascii number as a binary number
 * 
 * @PARAM:
 *          byte: number to convert to a binary number and send to the terminal
 *
 */
void TRMNL_sendWord(uint32_t word){
    char buff[33] = {0};
    itoa(word, buff, 2);
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
void TRMNL_sendNum(uint32_t num){
    char buff[11] = {0};
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
void TRMNL_sendHex(uint32_t num){
    char buff [9] = {0};
    itoa(num, buff, 16);
    TRMNL_sendString("0x");
    TRMNL_sendString(buff);
}
