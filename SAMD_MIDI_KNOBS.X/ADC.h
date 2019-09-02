/* 
 * File:   ADC.h
 * Author: Pete
 *
 * Created on August 13, 2019, 11:38 AM
 */

#ifndef ADC_H
#define	ADC_H

#include <proc/SAMD21A/samd21.h>

void ADC_init(void);
int ADC_read(uint8_t channel);
void ADC_addChannel(uint8_t port, uint8_t pin);

#endif	/* ADC_H */

