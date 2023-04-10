#include "msp.h"

#define UCBR_VAL 1
#define UCBRF_VAL 10
#define UCBRS_VAL 0

//ASCII Characters
#define ESC 0x1B
#define NULL '\0'

//ESC Commands
#define CLEAR_SCREEN "[2J"
#define RESET_CURSOR "[H"
#define DISABLE_ATTRIBUTES "[m"
#define BLINK "[5m"
#define CURSOR_DOWN_1 "[1B"
#define CURSOR_DOWN_3 "[3B"
#define CURSOR_RIGHT_5 "[5C"
#define CURSOR_LEFT_5 "[5D"
#define CURSOR_LEFT_15 "[15D"
#define TEXT_RED "[31m"
#define TEXT_BLUE "[34m"
#define TEXT_GREEN "[32m"
#define TEXT_WHITE "[37m"

#define Puart P1
#define RXD BIT2
#define TXD BIT3
#define NVIC_ISR_UART_A0 (1<<16)

void UART_init(void);
void UART_print_string(char sendstring[]);
void UART_send_data(uint8_t character);
void UART_esc_code(char sendstring[]);

void main(void){
  WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // stop watchdog timer
  UART_init();
  //default setup, reset previous terminal settings
  UART_esc_code(CLEAR_SCREEN); //clear entire terminal
  UART_esc_code(RESET_CURSOR); //Reset cursor to upper left
  UART_esc_code(TEXT_WHITE); //white text
  //Printing to terminal
  UART_esc_code(CURSOR_DOWN_3);
  UART_esc_code(CURSOR_RIGHT_5);
  UART_print_string("All good students read the");
  UART_esc_code(CURSOR_DOWN_1);
  UART_esc_code(CURSOR_LEFT_15);
  UART_esc_code(BLINK);
  UART_print_string("TRM");
  UART_esc_code(RESET_CURSOR);
  UART_esc_code(DISABLE_ATTRIBUTES);
  UART_print_string("Input:");
  while(1);
}

void EUSCIA0_IRQHandler(void)
{
  if((EUSCI_A0->IFG)&EUSCI_A_IFG_RXIFG) //check if receive flag is set
  {
    uint8_t data = EUSCI_A0->RXBUF;
    switch(data)
    {
      case('R'):
        UART_esc_code(TEXT_RED);
        break;
      case('B'):
        UART_esc_code(TEXT_BLUE);
        break;
      case('G'):
        UART_esc_code(TEXT_GREEN);
        break;
      case('W'):
        UART_esc_code(TEXT_WHITE);
        break;
      default:
        UART_send_data(data); //echo data
        break;
    }
  }
}

//Function to write string to terminal
void UART_print_string(char sendstring[])
{
  uint8_t iter = 0;
  while(sendstring[iter] != NULL)
  {
    UART_send_data(sendstring[iter]);
    iter++;
  }
}

//Function to send esc command to terminal
void UART_esc_code(char sendstring[])
{
  uint8_t iter = 0;
  UART_send_data(ESC);
  while(sendstring[iter] != NULL)
  {
    UART_send_data(sendstring[iter]);
    iter++;
  }
}

//Utility Function to send 8 bit data to terminal
//Utilized in other functions
void UART_send_data(uint8_t data)
{
  while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG)); //wait until TXBUF is empty
  EUSCI_A0->TXBUF = data;
}

void UART_init(void)
{
  //Initialize UART
  EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST; //put UART into software reset
  
  //Use SMCLK for BRCLK
  EUSCI_A0->CTLW0 = EUSCI_A_CTLW0_SSEL__SMCLK
                  |EUSCI_A_CTLW0_SWRST;
  
  //Set clock prescaler of baud generator
  EUSCI_A0->BRW = UCBR_VAL;
  
  //Oversampling mode enable, Set BRF(1st modulation) to BRF_VAL,
  //Set BRS(2nd modulation) to BRS_VAL
  EUSCI_A0->MCTLW = EUSCI_A_MCTLW_OS16
                  |(UCBRF_VAL<<EUSCI_A_MCTLW_BRF_OFS)
                  |(UCBRS_VAL<<EUSCI_A_MCTLW_BRS_OFS);
  
  //Initialize TX and RX pins
  Puart->SEL0 |= (TXD|RXD);
  Puart->SEL1 &= ~(TXD|RXD);
  Puart->DIR |= TXD;
  Puart->DIR &= ~RXD;
  
  EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; //end UART reset
  
  //Interrupts
  EUSCI_A0->IE |= EUSCI_A_IE_RXIE; //enable RX to trigger interrupt
  NVIC->ISER[0] = NVIC_ISR_UART_A0; //Enable ISR for UART
  __enable_irq();
}
