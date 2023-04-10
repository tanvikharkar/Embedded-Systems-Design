#include "msp.h"
#include "set_DCO.h"

void set_DCO(uint32_t frequency){
  CS->KEY = CS_KEY_VAL; //unlock CS Registers
  CS->CTL0 = (frequency); //change DCO speed to input speed

  CS->CTL1 = (CS_CTL1_SELM__DCOCLK | // Select the DCOCLK signal (MCLK)
  CS_CTL1_DIVM_0| //set MCLK to divide by 1
  CS_CTL1_SELM__DCOCLK| // set SMCLK to DCO
  CS_CTL1_DIVS_0); // divide SMCLK by 1

  CS->KEY = 0; // lock CS registers
}
