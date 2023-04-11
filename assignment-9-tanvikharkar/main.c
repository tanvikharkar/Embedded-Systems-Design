#include "msp.h"
#include "stdint.h"
#include "A9.h"

/**
* main.c
*/

int i = 0; //global variables
int flag = 0;

void main(void)
{
  WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // stop watchdog timer
  
  set_DCO(FREQ_24_MHz);
  uart_init();
  COMP_Einit();
  
  TIMER_A0->CTL = (TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC_2 | TIMER_A_CTL_IFG
                | TIMER_A_CTL_IE); // Timer A Continuous and SMCLK
  TIMER_A0->CCTL[3] &= ~TIMER_A_CCTLN_CCIFG;
  TIMER_A0->CCTL[3] = (TIMER_A_CCTLN_CM_1 // Capture on rising edge
                    | TIMER_A_CCTLN_CCIS_1 // CCI1B input select
                    | TIMER_A_CCTLN_CAP // Select capture mode
                    | TIMER_A_CCTLN_CCIE // Capture/compare interrupts
                    | TIMER_A_CCTLN_SCS); // Synchronize capture with clock
  
  NVIC->ISER[0] = 1 << (TA0_N_IRQn); // set interrupt
  __enable_irq();
  
  int calibrated_freq; // variable to output
  
  while(1)
  {
    if(flag==1) // If interrupt on rising edge triggered
    {
      calibrated_freq = freq;
      uint8_t freqthou = (calibrated_freq / 1000) % 10; // split value up for uart
      uint8_t freqhun = (calibrated_freq / 100) % 10;
      uint8_t freqten = (calibrated_freq / 10) % 10;
      uint8_t freqone = (calibrated_freq % 10);
      uart_esc_code(CLEAR); // clear screen
      uart_esc_code(CURSOR_UPPER_LEFT); // move to upper left
      uart_print("Frequency: "); // print out each value
      EUSCI_A0 -> TXBUF = freqthou + '0';
      while(!(EUSCI_A0 -> IFG & EUSCI_A_IFG_TXIFG));
      EUSCI_A0 -> TXBUF = freqhun + '0';
      while(!(EUSCI_A0 -> IFG & EUSCI_A_IFG_TXIFG));
      EUSCI_A0 -> TXBUF = freqten + '0';
      while(!(EUSCI_A0 -> IFG & EUSCI_A_IFG_TXIFG));
      EUSCI_A0 -> TXBUF = freqone + '0';
      uart_esc_code(MOVE_ONE);
      uart_esc_code(MOVE_LEFT1);
      uart_esc_code(MOVE_LEFT2);
      __delay_us(5000000);
      flag = 0;
      i = 0;
    }
  }
}

void COMP_Einit(void){
  P6->SEL0 |= (COMP_P | COMP_N); // V+ to 6.6(input) V- to 6.7(ref)
  P6->SEL1 |= (COMP_P | COMP_N);
  
  P7->SEL0 |= COMP_OUT; // Comparator out set 7.2
  P7->SEL1 &= ~(COMP_OUT);
  P7->DIR |= COMP_OUT;
  
  COMP_E1->CTL0 = (COMP_E_CTL0_IPEN // v+(input)
                | COMP_E_CTL0_IMEN // v-
                | COMP_E_CTL0_IMSEL_0 // v- to channel 1
                | COMP_E_CTL0_IPSEL_1); // v+ to channel 1
  
  COMP_E1->CTL1 = (COMP_E_CTL1_ON // turn on compE
                | COMP_E_CTL1_FDLY_3
                | COMP_E_CTL1_F); // filter
}

void TA0_N_IRQHandler(void)
{
  static int overflow = 0; // variables for calculation
  static uint16_t val0 = 0;
  static uint16_t val1 = 0;
  static uint16_t freq_conv = 0;
  static uint32_t difference = 0;
  
  if (TIMER_A0->CTL & TIMER_A_CTL_IFG) // If Timer A interrupt flag triggered
  {
    TIMER_A0->CTL &= ~TIMER_A_CTL_IFG; // Clear flag
    overflow++; // Counts number of overflows to use in freq calculation
  }
  
  if (TIMER_A0->CCTL[3] & TIMER_A_CCTLN_COV)
  {
    TIMER_A0->CCTL[3] &= ~TIMER_A_CCTLN_COV; // If overwrite occurred, reset
    val0 = 0;
    val1 = 0;
  }
  
  if (TIMER_A0->CCTL[3] & TIMER_A_CCTLN_CCIFG) // Interrupt triggered
  {
    TIMER_A0->CCTL[3] &= ~TIMER_A_CCTLN_CCIFG; // Clear interrupt flag
    val1 = val0; // Store previous CCR[3] value
    val0 = TIMER_A0->CCR[3]; // Store CCR[3] value in val0
    
    if(val1! = 0){
    difference = (val0 - val1) + (overflow * MAX); // Use difference to calculate freq
    freq_conv = ((FREQ_TA/difference)*1007)/1000; // conversion factor
    if(freq_conv != 0){
      freq = freq_conv; // output freq
      if(freq_conv>MAX_FREQ){ // dont output over 1k
      freq=MAX_FREQ;
      }else
      freq = freq_conv;
      flag = 1; // set flag for main
      }
      val1 = 0; // reset values for next calc
      val0 = 0;
    }
    overflow = 0; // Reset overflow
  }
}

void uart_init(){
  // Configure UART pins
  UART_PORT->SEL0 |= RXD_PIN | TXD_PIN; // set 2-UART pin as secondary function
  
  // Configure UART
  EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Put eUSCI in reset
  
  EUSCI_A0->CTLW0 = (
                      EUSCI_A_CTLW0_SWRST | // Remain in software reset mode
                      EUSCI_A_CTLW0_MODE_0 | // UART mode
                      EUSCI_A_CTLW0_SSEL__SMCLK // Set clk source to SMCLK
                    );
  EUSCI_A0->BRW = BAUD_BRW; // Set CLK prescaler setting to value calculated in .h file
  EUSCI_A0->MCTLW = (BAUD_BRS << EUSCI_A_MCTLW_BRS_OFS) |
  // Set Second modulation stage to value calculated in .h file
                    (BAUD_BRF << EUSCI_A_MCTLW_BRF_OFS) |
  // Set first modulation stage to value calculated in .h file
                    EUSCI_A_MCTLW_OS16; // Enable Oversampling mode
  
  EUSCI_A0->IFG &= ~EUSCI_A_IFG_RXIFG; // Clear eUSCI RX interrupt flag
  
  EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // Clear software reset register
  
  EUSCI_A0->IE = EUSCI_A_IE_RXIE; // Enable USCI_A0 RX and TX interrupt
}

void uart_print(char string[]){
  uint8_t index= 0;
  while( string[index] != 0){
    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
    // Wait until transmit buffer is empty
    EUSCI_A0->TXBUF = string[index++];
    // Send ith member of string array and increment by 1
  }
}

void uart_esc_code(char string[]){
uint8_t index = 0;
  
while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG)); // Wait until transmit buffer is empty
EUSCI_A0->TXBUF = ESC_CHAR; // Send escape character to terminal
  
while( string[index] != 0){
  while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
  // Wait until transmit buffer is empty
  EUSCI_A0->TXBUF = string[index++];
  // Send ith member of string array and increment by 1
  }
}

void set_DCO(uint32_t frequency)
{
  CS->KEY = CS_KEY_VAL; // unlock cs registers
  CS->CTL0 = 0; // clear registers for dco, selects and dividers
  
  if(frequency == FREQ_1_5_MHz)
  {
    CS->CTL0 = CS_CTL0_DCORSEL_0; // set dco to 1.5MHz
  }
  
  else if(frequency == FREQ_3_MHz)
  {
    CS->CTL0 = CS_CTL0_DCORSEL_1; // set dco to 3MHz
  }
  
  else if(frequency == FREQ_6_MHz)
  {
    CS->CTL0 = CS_CTL0_DCORSEL_2; // set dco to 6MHz
  }
  
  else if(frequency == FREQ_12_MHz)
  {
    CS->CTL0 = CS_CTL0_DCORSEL_3; // set dco to 12MHz
  }
  
  else if(frequency == FREQ_24_MHz)
  {
    CS->CTL0 = CS_CTL0_DCORSEL_4; // set dco to 24MHz
  }
  
  CS->CTL1 |= CS_CTL1_SELM__DCOCLK; // write dco to mclk
  CS->CTL1 |= (CS_CTL1_SELS__DCOCLK | CS_CTL1_DIVS_3) ; // write dco to smclk
  CS->KEY = 0;
}
