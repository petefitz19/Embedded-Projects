/* 
 * File:   rotaryControl.c
 * Author: austin
 *
 * Created on June 19, 2019, 2:24 PM
 * 
 * @VERSION: 1.2
 * 
 * @NOTE: Change the port and pin values here. See .h for more information about pins
 */

#include "rotaryControl.h"

//values to select the port and pins
#define PORT PORTE
#define PINA 0
#define PINB 1

//direction of state table
#define DIR_NONE 0x0
#define DIR_CW   0x10
#define DIR_CCW  0x20

//FULL-STEP - Uses the full-step state table below (emits a code at 00 only) 
#define R_START 		0x0
#define R_CCW_BEGIN 	0x1
#define R_CW_BEGIN 	0x2
#define R_START_M 	0x3
#define R_CW_BEGIN_M 	0x4
#define R_CCW_BEGIN_M 0x5
const unsigned char ttable[6][4] = {
  {R_START_M,           R_CW_BEGIN,     R_CCW_BEGIN,  R_START},           // R_START (00)
  {R_START_M | DIR_CCW, R_START,        R_CCW_BEGIN,  R_START},           // R_CCW_BEGIN
  {R_START_M | DIR_CW,  R_CW_BEGIN,     R_START,      R_START},           // R_CW_BEGIN
  {R_START_M,           R_CCW_BEGIN_M,  R_CW_BEGIN_M, R_START},           // R_START_M (11)  
  {R_START_M,           R_START_M,      R_CW_BEGIN_M, R_START | DIR_CW},  // R_CW_BEGIN_M 
  {R_START_M,           R_CCW_BEGIN_M,  R_START_M,    R_START | DIR_CCW}, // R_CCW_BEGIN_M
};

register8_t* PINxCTRL_config[8] = {
    &(PORT.PIN0CTRL), 
    &(PORT.PIN1CTRL), 
    &(PORT.PIN2CTRL),
    &(PORT.PIN3CTRL),
    &(PORT.PIN4CTRL),
    &(PORT.PIN5CTRL),
    &(PORT.PIN6CTRL),
    &(PORT.PIN7CTRL),
};

uint8_t PINx_bm[8] = {
    PIN0_bm, PIN1_bm, PIN2_bm, PIN3_bm, PIN4_bm, PIN5_bm, PIN6_bm, PIN7_bm
};

uint8_t state = 0; //tracks the position of the encoder

/* @NAME: ROATRY_init
 *
 * @DESCRIPTION: initializes the rotary control to work with the pins defined in rotaryControl.h
 *              If pullup is 0 the pullups will not be enabled. If pullup != 0 then pullups will
 *              be enabled.
 * 
 * @PARAM:
 *          pullup: allows the user to select whether pull ups are enabled or not
 */
void ROTARY_init(uint8_t pullup){
    PORT.DIRCLR = PINx_bm[PINA] | PINx_bm[PINB];
    
    *PINxCTRL_config[PINA] = PORT_ISC_BOTHEDGES_gc;
    *PINxCTRL_config[PINB] = PORT_ISC_BOTHEDGES_gc;    
    
    if(pullup){
        *PINxCTRL_config[PINA] |= PORT_PULLUPEN_bm;
        *PINxCTRL_config[PINB] |= PORT_PULLUPEN_bm;   
    }
}

/* @NAME: ROTARY_ISR
 * 
 * @DESCRIPTION: Returns whether the encoder moved clockwise or counterclockwise
 * 
 * @NOTES: should be called within an ISR for the port defined in rotaryControl
 *
 */
int8_t ROTARY_ISR(){
     //stores the current position of the rotary encoders
    uint8_t pos = ((PORT.IN & PINx_bm[PINA]) >> PINA) | ((PORT.IN & PINx_bm[PINB]) >> (PINB - 1));
    
    //state - lookup table
    state = ttable[state & 0xf][pos];

    //direction
    int8_t _dir = state & 0x30;
    if (_dir == DIR_NONE) {
      return 0; //no change, return 0
    }
    if (_dir == DIR_CW) {
      return 1; //Clockwise
    }
    if (_dir == DIR_CCW) {
      return -1;//Counter clockwise
    }
    
    return 0; //if everything fails return 0
    
}
