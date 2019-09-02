/*
 * File:   I2C_Master.c
 * Author: austin
 *
 * Created on June 13, 2019, 12:34 PM
 * 
 * @VERSION: 2.1 (oled)
 * 
 * @DESCRIPTION: This is a simple library to write and read data to a slave device
 *               using I2C
 * 
 * @NOTES: PA2: SDA
 *         PA3: SCL
 */

#include "I2C_Master.h"
#include <stdbool.h>
static volatile uint8_t ready = true;
static volatile uint8_t result;
static volatile uint8_t *data;
static uint8_t length;
static uint8_t curPos;

/* @NAME: TWI0_init
 * 
 * @DESCRITPTION: This is function initializes the device as a master device
 *                
 * @PARAM:
 *          muxSel:     Selects the port muxing to change the I2C location
 *                      If muxSel = 2 then the SDA is on pin PC2 and SCL is on pin PC3
 *                      if muxSel != 2 then the SDA is on pin PA2 and SCL is on pin PA3
 *                      
 */
void TWI0_init(uint8_t muxSel){
    TWI0.MBAUD = (uint8_t)TWI0_BAUD(400000, 1); //Set the MBAUD rate
    
    TWI0.MCTRLB |=  TWI_FLUSH_bm;           // Clear TWI state
    
    TWI0.MCTRLA &= ~TWI_TIMEOUT_gm;         // Disable Timeout for TWI operation
    
    TWI0.MCTRLA |=  TWI_ENABLE_bm           //enables as master 
                 | TWI_WIEN_bm              //enables write interrupt
                 | TWI_RIEN_bm;             //enables read interrupt
    
    TWI0.MSTATUS |= TWI_BUSSTATE_IDLE_gc;   //initially sets the bus state to idle
    
    if(muxSel == 2){
        PORTMUX.TWISPIROUTEA |= PORTMUX_TWI0_ALT2_gc;
    }
    else{
        PORTMUX.TWISPIROUTEA |= PORTMUX_TWI0_DEFAULT_gc;
    }
}

/* @NAME: TWI0_getReady
 * 
 * @DESCRIPTION: This function returns the status of the ready variable
 */
uint8_t TWI0_getReady(){
    return ready;
}

/* @NAME: TWI0_getResult
 *
 * @DESCRIPTION: This function returns the status of the ready variable
 */
uint8_t TWI0_getResult(){
    return result;
}

/* @NAME: TWI0_start
 *
 * @DESCRIPTION: Starts the i2c communication by sending the address and the r/w bit
 * 
 * @PARAM:
 *          sADDR:  address of the slave device
 * 
 *          rw:     determines whether it is read or write. If 0 it is write, if 1 it is read
 * 
 *          *tData: pointer to array of data to read or write to
 * 
 *          tLength: number of bytes to read or write
 */
void TWI0_start(uint8_t sADDR, uint8_t rw, uint8_t *tData, uint8_t tLength){
    if(ready){
        ready = false;
        result = TWI_RESULT_UNKNOWN;
        data = tData;
        curPos = 0;
        length = tLength;
        TWI0.MADDR = (sADDR << 1) | rw;
    }
}

/* @NAME: ISR
 * 
 * @DESCRIPTION: is the ISR for the reading and writing of the i2c data
 *
 * @NOTE: This is interrupt base so there is no need to call this function
 */
ISR(TWI0_TWIM_vect){
    uint8_t status = TWI0.MSTATUS;
    
    //checks arbitration
    if(status & TWI_ARBLOST_bm){
        TWI0.MSTATUS |= TWI_ARBLOST_bm | TWI_BUSERR_bm | TWI_WIF_bm;
        result = TWI_RESULT_ARBLOST;
        ready = true;
    }
    
    //checks bus error
    if(status & TWI_BUSERR_bm){
        TWI0.MSTATUS |= TWI_ARBLOST_bm | TWI_BUSERR_bm | TWI_WIF_bm;
        result = TWI_RESULT_BUSERR;
        ready = true;
    }
    
    //master write 
    else if(status & TWI_WIF_bm){
        
        //NAK
        if(status & TWI_RXACK_bm){
            TWI0.MCTRLB = TWI_MCMD_STOP_gc;
            result = TWI_RESULT_NACK_RECEIVED;
            ready = true;
        }
        //send next
        else if(curPos < length){
            TWI0.MDATA = data[curPos];
            curPos++;
        }
        //all data sent
        else{
            TWI0.MCTRLB = TWI_MCMD_STOP_gc;
            result = TWI_RESULT_OK;
            ready = true;
        }
    }
    
    //master read
    else if(status & TWI_RIF_bm){
        //NAK
        if(status & TWI_RXACK_bm){
            TWI0.MCTRLB = TWI_MCMD_STOP_gc;
            result = TWI_RESULT_NACK_RECEIVED;
            ready = true;
        }
        
        data[curPos] = TWI0.MDATA;
        curPos++;
        
        //receive next
        if(curPos < length){
            TWI0.MCTRLB = TWI_MCMD_RECVTRANS_gc;
        }
        
        //receive complete
        else{
            TWI0.MCTRLB = TWI_ACKACT_bm | TWI_MCMD_STOP_gc;
            result = TWI_RESULT_OK;
            ready = true;
        }
    }
    //unknown error
    else{
        result = TWI_RESULT_FAIL;
        ready = true;
    }
}