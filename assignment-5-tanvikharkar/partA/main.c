#include "msp.h"

/**
* main.c
*/

#define CYCLES 3000000
#define FREQ_24_MHZ CS_CTL0_DCORSEL_4
#define FREQ_12_MHZ CS_CTL0_DCORSEL_3
#define FREQ_6_MHZ CS_CTL0_DCORSEL_2

void set_DCO(uint32_t desired_speed);

void main(void)
{
  WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // stop watchdog timer
  
  P4->SEL0 |= BIT3; // set P4.3 to output MCLK
  P4->SEL1 &= ~(BIT3);
  P4->DIR |= BIT3;
  
  P3->SEL0 &= ~(BIT0);  // set P3.0 to show timer signal
  P3->SEL1 &= ~(BIT0);
  P3->DIR |= BIT0;
  
  set_DCO(FREQ_24_MHZ); // set clock to 24MHz
  
  TIMER_A0->CTL |= (TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__CONTINUOUS);
  TIMER_A0->CCR[0] = 239; // count up to 240-1(starts at 0) cycles = 25kHz with 25% duty cycle
  TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE; // turn on interrupts for CCR0
  
  NVIC->ISER[0] = 1 << (TA0_0_IRQn);  // enable CCIFG flag for timer
  
  __enable_irq(); // global interrupt enable
  
  while(1);
}

void set_DCO(uint32_t desired_speed){
  CS->KEY = CS_KEY_VAL; // unlock CS registers
  CS->CTL0 = (desired_speed); // change DCO speed to input speed
  CS->CTL1 = (CS_CTL1_SELM__DCOCLK |  // select the DCOCLK signal
             CS_CTL1_DIVM__1 |
             CS_CTL1_SELS__DCOCLK); // set MCLK to divide by 1
  CS->KEY = 0;  // lock CS registers
}

void TA0_0_IRQHandler(void){
  if(TIMER_A0->CCTL[0] & TIMER_A_CCTLN_CCIFG){  // if CCIFG(CCTL[0]) is interrupted(high)
    TIMER_A0->CCTL[0] &= ~(TIMER_A_CCTLN_CCIFG);  // clear flag (atypical)
    P3->OUT ^= BIT0;  // toggle P3 so it goes on or off
    if(P3->OUT & 0x01){ // if P3 is high at port 0
      TIMER_A0->CCR[0] += 240;  // leave it on for 25% of cycle
    }
    else{
      TIMER_A0->CCR[0] += 720;  // if P3 is low, leave it on for 75% of cycle
    }
  }
}
