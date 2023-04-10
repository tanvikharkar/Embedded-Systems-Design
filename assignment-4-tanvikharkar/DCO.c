#include "msp.h"
#include "DCO.h"

void set_DCO(int freq)
{
  CS->KEY = CS_KEY_VAL; // Unlock CS Registers
  CS->CTL0 = 0; // Reset Control 0 Register
  
  if (freq == FREQ_1_5_MHz)
  {
    CS->CTL0 |= CS_CTL0_DCORSEL_0;
  }
  
  else if (freq == FREQ_3_MHz)
  {
    CS->CTL0 |= CS_CTL0_DCORSEL_1;
  }
  
  else if (freq == FREQ_6_MHz)
  {
    CS->CTL0 |= CS_CTL0_DCORSEL_2;
  }
  
  else if (freq == FREQ_12_MHz)
  {
    CS->CTL0 |= CS_CTL0_DCORSEL_3;
  }
  
  else if (freq == FREQ_24_MHz)
  {
    CS->CTL0 |= CS_CTL0_DCORSEL_4;
  }
  
CS->CTL1 = (CS_CTL1_DIVM__1 | CS_CTL1_SELM__DCOCLK);
CS->KEY = 0; // Lock CS Registers
}
