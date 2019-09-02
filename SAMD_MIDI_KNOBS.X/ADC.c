/*
 * File:   ADC.c
 * Author: Pete
 *
 * Created on August 13, 2019, 11:38 AM
 */


#include "ADC.h"

#define NVM_READ_CAL(cal) \
    ((*((uint32_t *)NVMCTRL_OTP4 + 27 / 32)) >> (27 % 32)) & ((1 << 8) - 1)

/* 
 * @NAME: ADC_init
 * 
 * @DESCRIPTION: Initializes the ADC module
 */
void ADC_init(void){

  PM->APBCMASK.reg |= PM_APBCMASK_ADC;

  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(ADC_GCLK_ID) |
      GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(0);

  ADC->CTRLA.reg = ADC_CTRLA_SWRST;
  while (ADC->CTRLA.reg & ADC_CTRLA_SWRST);

  ADC->REFCTRL.reg = ADC_REFCTRL_REFSEL_INTVCC1 | ADC_REFCTRL_REFCOMP;
  ADC->CTRLB.reg = ADC_CTRLB_RESSEL_16BIT | ADC_CTRLB_PRESCALER_DIV32;
  ADC->AVGCTRL.reg = ADC_AVGCTRL_SAMPLENUM_128;

  ADC->CALIB.reg = ADC_CALIB_BIAS_CAL(NVM_READ_CAL(ADC_BIASCAL)) |
      ADC_CALIB_LINEARITY_CAL(NVM_READ_CAL(ADC_LINEARITY));

  ADC->CTRLA.reg = ADC_CTRLA_ENABLE;
}

/* 
 * @NAME: ADC_read
 * 
 * @DESCRIPTION: Initializes a read, waits for the read, then returns the averaged 
 *               voltage reading stored in the RESULT register.
 * 
 * @PARAM:  
 *          channel: The MUXPOS ADC channel the user wishes to read
 */
int ADC_read(uint8_t channel){
  ADC->INPUTCTRL.reg = (channel << ADC_INPUTCTRL_MUXPOS_Pos)| ADC_INPUTCTRL_MUXNEG_GND |
    ADC_INPUTCTRL_GAIN_DIV2;
  ADC->SWTRIG.reg = ADC_SWTRIG_START;
  while (0 == (ADC->INTFLAG.reg & ADC_INTFLAG_RESRDY));

  return ADC->RESULT.reg;
}

void ADC_addChannel(uint8_t port, uint8_t pin){
    PORT->Group[port].DIRCLR.reg = (1 << pin);
    PORT->Group[port].PINCFG[pin].reg |= PORT_PINCFG_INEN;

    PORT->Group[port].PINCFG[pin].reg |= PORT_PINCFG_PMUXEN;
      if (pin & 1)
        PORT->Group[port].PMUX[pin>>1].bit.PMUXO = PORT_PMUX_PMUXO_B_Val;
      else
        PORT->Group[port].PMUX[pin>>1].bit.PMUXE = PORT_PMUX_PMUXO_B_Val;
}
