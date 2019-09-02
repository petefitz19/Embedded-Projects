/* 
 * File:   I2C_Master.h
 * Author: austin
 *
 * Created on June 13, 2019, 12:33 PM
 * 
 * @VERSION: 2.1(oled)
 */

#ifndef I2C_MASTER_H
#define	I2C_MASTER_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>

//holds all of the possible result values
//@NOTE: refer to this table for using results
typedef enum TWI_RESULT_enum {
	TWI_RESULT_UNKNOWN          = (0x00),
	TWI_RESULT_OK               = (0x01),
	TWI_RESULT_ARBLOST          = (0x02),
	TWI_RESULT_BUSERR           = (0x03),
	TWI_RESULT_NACK_RECEIVED    = (0x04),
	TWI_RESULT_FAIL             = (0x05),
} TWI_RESULT_t;

//define used to set the baud rate of the I2C module
#define TWI0_BAUD(F_SCL, T_RISE)    \
    ((((((float)3333333 / (float)F_SCL)) - 10 - ((float)3333333 * T_RISE / 1000000))) / 2)

void TWI0_init(uint8_t);
uint8_t TWI0_getReady();
uint8_t TWI0_getResult();
void TWI0_start(uint8_t, uint8_t, uint8_t*, uint8_t);

#endif	/* I2C_MASTER_H */

