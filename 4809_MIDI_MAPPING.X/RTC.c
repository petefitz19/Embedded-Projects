/*
 * File:   RTC.c
 * Author: austin
 *
 * Created on July 18, 2019, 10:20 AM
 * 
 * @VERSION 1.1
 */


#include "RTC.h"


/* @NAME: RTC_init
 * 
 * @DESCRIPTION: This function enables the RTC so that the clock count will equal
 *             the millisecond count. The clock will range from 0 to 65536 milliseconds. 
 */
void RTC_init(){
    
    //Wait for all register to be synchronized 
    while (RTC.STATUS > 0){;}
    
    //selects the internal 32kHz clock
    RTC.CLKSEL = RTC_CLKSEL_INT32K_gc;

    /* Run in debug: enabled */
    RTC.DBGCTRL = RTC_DBGRUN_bm;  
    
    //Enables the RTC and divides the clock by 32
    RTC.CTRLA = RTC_RTCEN_bm | (0x5 << 3);
}

/* @NAME: RTC_periodicIntSet
 * 
 * @DESCRIPTION: Allows the user to initiate an interrupt that will happen after
 *             a desired amount of time has passed.
 * 
 * @PARAM:
 *          interruptTime: desired amount of time to initiate the interrupt
 */
void RTC_periodicIntSet(uint16_t interruptTime){
     
    uint16_t currentTime = RTC_getTime();
    
    RTC.CMP = currentTime + interruptTime;
    
    RTC.INTCTRL = RTC_CMP_bm;
}

/* @NAME: RTC_periodicISR
 * 
 * @DESCRIPTION: Function to be called within the RTC_CNT_vect to clear the interrupt
 *              and set the next interrupt.
 * 
 * @PARAM:
 *          interruptTime: desired amount of time to initiate the interrupt
 */
void RTC_periodicIntISR(uint16_t interruptTime){
    
    //resets the interrupt flag
    RTC.INTFLAGS = RTC_CMP_bm;
    
    uint16_t currentTime = RTC_getTime();
        
    RTC.CMP = currentTime + interruptTime;
}

/* @NAME: RTC_periodicIntDisable
 * 
 * @DESCRIPTION: Allows the user to disable the periodic interrupt. To reenable
 *             simply use RTC_periodicIntSet.
 */
void RTC_periodicIntDisable(){
    RTC.INTCTRL &= ~RTC_CMP_bm;
}

/* @NAME: RTC_getTime
 * 
 * @DESCRIPTION: Returns the current count value of the RTC.
 * 
 * @NOTE: This is in an atomic block so that reading the clock will return the 
 *          correct value.
 */
uint16_t RTC_getTime(){
    uint16_t time;
    ATOMIC_BLOCK(ATOMIC_FORCEON){
        time = RTC.CNT;
    }
    return time;
}