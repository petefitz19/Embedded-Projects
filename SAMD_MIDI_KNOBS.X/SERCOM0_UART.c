/* 
 * File:   SERCOM0_UART.c
 * Author: austin
 *
 * Created on August 5, 2019, 3:35 PM
 * 
 *  @VERSION 1.0
 */

#include "SERCOM0_UART.h"

#define F_CPU 48000000

/* @NAME: SERCOM0_UART_init
 * 
 * @DESCRIPTION: Initializes the USART0 module, allowing the user to input a muxOption to select
 *               the pin configuration they would like to use in addition to the baud rate for the
 *               module
 * 
 * @PARAM:
 *          muxOption: allows the user to select between pin configurations: 
 *                     0 -> (TX: PA04, RX: PA05) or 1 -> (TX: PA08, RX: PA09)
 * 
 *          baud: allows the user to set the baud rate for the module
 */
void SERCOM0_UART_init(uint8_t muxOption, uint32_t baud) {

    if(muxOption > 1){  //Makes sure the user inputs a valid muxOption
        return;
    }

	uint64_t br = (uint64_t)65536 * (F_CPU - 16 * baud) / F_CPU;	// Variable for baud rate

    
    switch(muxOption){
        case(0):
            //pins PA04 and PA05
            PORT->Group[0].DIRSET.reg = PORT_PA04;							// Set TX Pin direction to output
            PORT->Group[0].PINCFG[4].reg |= PORT_PINCFG_INEN;				// Set TX Pin config for input enable (required for usart)
            PORT->Group[0].PINCFG[4].reg |= PORT_PINCFG_PMUXEN;             // enable PMUX
            PORT->Group[0].PMUX[4>>1].bit.PMUXE = PORT_PMUX_PMUXE_D_Val;	// Set the PMUX bit (if pin is even, PMUXE, if odd, PMUXO)

            PORT->Group[0].DIRCLR.reg = PORT_PA05;							// Set RX Pin direction to input
            PORT->Group[0].PINCFG[5].reg |= PORT_PINCFG_INEN;				// Set RX Pin config for input enable
            PORT->Group[0].PINCFG[5].reg &= ~PORT_PINCFG_PULLEN;			// enable pullup/down resistor
            PORT->Group[0].PINCFG[5].reg |= PORT_PINCFG_PMUXEN;             // enable PMUX
            PORT->Group[0].PMUX[5>>1].bit.PMUXO = PORT_PMUX_PMUXE_D_Val;	// Set the PMUX bit (if pin is even, PMUXE, if odd, PMUXO)
            break;
        case(1):
            //pins PA08 and PA09
            PORT->Group[0].DIRSET.reg = PORT_PA08;							// Set TX Pin direction to output
            PORT->Group[0].PINCFG[8].reg |= PORT_PINCFG_INEN;				// Set TX Pin config for input enable (required for usart)
            PORT->Group[0].PINCFG[8].reg |= PORT_PINCFG_PMUXEN;             // enable PMUX
            PORT->Group[0].PMUX[8>>1].bit.PMUXE = PORT_PMUX_PMUXE_C_Val;	// Set the PMUX bit (if pin is even, PMUXE, if odd, PMUXO)

            PORT->Group[0].DIRCLR.reg = PORT_PA09;							// Set RX Pin direction to input
            PORT->Group[0].PINCFG[9].reg |= PORT_PINCFG_INEN;				// Set RX Pin config for input enable
            PORT->Group[0].PINCFG[9].reg &= ~PORT_PINCFG_PULLEN;			// enable pullup/down resistor
            PORT->Group[0].PINCFG[9].reg |= PORT_PINCFG_PMUXEN;             // enable PMUX
            PORT->Group[0].PMUX[9>>1].bit.PMUXO = PORT_PMUX_PMUXE_C_Val;	// Set the PMUX bit (if pin is even, PMUXE, if odd, PMUXO)
            break;
    }
	
	PM->APBCMASK.reg |= PM_APBCMASK_SERCOM0;						// Set the PMUX for SERCOM3 and turn on module in PM

	// Generic clock ?SERCOM3_GCLK_ID_CORE? uses GCLK Generator 0 as source (generic clock source can be
	// changed as per the user needs), so the SERCOM3 clock runs at 8MHz from OSC8M
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(SERCOM0_GCLK_ID_CORE) | GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(0);
    
    while ( GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY ) ; // Wait for synchronization

	// By setting the DORD bit LSB is transmitted first and setting the RXPO bit as 1
	// corresponding SERCOM PAD[1] will be used for data reception, PAD[0] will be used as TxD
	// pin by setting TXPO bit as 0, 16x over-sampling is selected by setting the SAMPR bit as
	// 0, Generic clock is enabled in all sleep modes by setting RUNSTDBY bit as 1,
	// USART clock mode is selected as USART with internal clock by setting MODE bit into 1.
    SERCOM0->USART.CTRLA.reg = SERCOM_USART_CTRLA_DORD | SERCOM_USART_CTRLA_MODE_USART_INT_CLK | SERCOM_USART_CTRLA_RXPO(1/*PAD1*/) | SERCOM_USART_CTRLA_TXPO(0/*PAD0*/);
	

	// 8-bits size is selected as character size by setting the bit CHSIZE as 0,
	// TXEN bit and RXEN bits are set to enable the Transmitter and receiver
	SERCOM0->USART.CTRLB.reg = SERCOM_USART_CTRLB_RXEN | SERCOM_USART_CTRLB_TXEN | SERCOM_USART_CTRLB_CHSIZE(0/*8 bits*/);
	
	// baud register value corresponds to the device communication baud rate
	SERCOM0->USART.BAUD.reg = (uint16_t)br;

	// SERCOM3 peripheral enabled
	SERCOM0->USART.CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;
}

/* @NAME: SERCOM0_UART_sendByte
 * 
 * @DESCRIPTION: Allows the user to send a Byte on the TX Line
 * 
 * @PARAM: 
 *          byte: the byte of data the user wishes to send
 */
void SERCOM0_UART_sendByte(uint8_t byte){
	while (!(SERCOM0->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_DRE));
	SERCOM0->USART.DATA.reg = byte;
}

/* @NAME: SERCOM0_UART_sendBytes
 * 
 * @DESCRIPTION: Allows the user to send multiple bytes on the TX Line
 * 
 * @PARAM: 
 *          data: the array of bytes the user wishes to transmit
 * 
 *          size: the size of the data array
 */
void SERCOM0_UART_sendBytes(uint8_t *data, uint32_t size){
    for(uint8_t i = 0; i < size; i++){
        SERCOM0_UART_sendByte(data[i]);
    }
}

/* @NAME: SERCOM0_UART_hasData
 * 
 * @DESCRIPTION: Returns a '1' if data is available in the data register and a '0' if not.
 */
uint8_t SERCOM0_UART_hasData(){
    if ((SERCOM0->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_RXC) >> SERCOM_USART_INTFLAG_RXC_Pos) {
    return 1;
  }
  return 0;
}

/* @NAME: SERCOM0_UART_getData
 * 
 * @DESCRIPTION: Returns the contents of the data register
 */
uint16_t SERCOM0_UART_getData(){
    return SERCOM0->USART.DATA.reg;
}

