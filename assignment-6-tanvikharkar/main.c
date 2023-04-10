#include "msp.h"
#include "set_DCO.h"
#include "keypad.h"

/**
* P1.7 CS Chip Select
* P1.6 SDI Data In
* P1.5 SCK System Clock In
* CIPO / SIMO not needed since no data coming from anything
*/

#define SPI_CS BIT7
#define SPI_COPI BIT6
#define SPI_SCK BIT5
#define SPI_PORT P1
#define DAC_BITS 4096 // 12-bit DAC = 2^12 = 4096
#define MAX_VOLT 3300 // 3.3V = 3300mV
#define FLAT_CALIBRATION 37 // measured voltage was found to be ~37mV from desired

void DAC_init(void);
void DAC_write(uint16_t mVolts);
uint16_t DAC_volt_conv(uint16_t mVolts);

/**
* main.c
*/
void main(void)
{
  int keypad_input;
  int index = 0;
  WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // stop watchdog timer
  set_DCO(FREQ_24_MHz);
  DAC_init();
  keypad_init();
  while(1) {
    while(index < 3){
      while(getKey() != (char)-1 && getKey() <= 9){ // check for valid inputs (0-9)
        if (index == 0){
          keypad_input = getKey()*1000; // 1000mV = 1V
        }
        else if (index == 1){
          keypad_input += getKey()*100; // 100mV = 0.1V
        }
        else if (index == 2){
          keypad_input += getKey()*10; // 10mV = 0.01V
        }
        index++;
        __delay_cycles(8000000); // delay for user input speed
      }
    }
    index = 0;
    while(getKey() == (char)-1){ // while no new input is entered
      DAC_write((uint16_t)keypad_input); // output current keypad input
      __delay_cycles(20); // delay between transmission
    }
  }
}
// initialize the eUSCI peripheral to communicate with the DAC
void DAC_init(void){
  EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST; // Put Bus into Software Reset
  EUSCI_B0->CTLW0 = EUSCI_B_CTLW0_MSB // MSB First
                  | EUSCI_B_CTLW0_MST // Controller Mode
                  | EUSCI_B_CTLW0_MODE_0 // 3-Pin SPI Mode
                  | EUSCI_B_CTLW0_SYNC // Synchronous Mode
                  | EUSCI_B_CTLW0_SSEL__SMCLK // SMCLK as Clock Source
                  | EUSCI_B_CTLW0_SWRST; // Keep Reset On
  EUSCI_B0->BRW = 0x01; // Divide Clock by 1
  
  SPI_PORT->SEL0 |= (SPI_SCK | SPI_COPI); // UCB0 PICO and CLK Mode
  SPI_PORT->SEL1 &= ~(SPI_SCK | SPI_COPI);

  SPI_PORT->SEL0 &= ~SPI_CS; // CS as GPIO
  SPI_PORT->SEL1 &= ~SPI_CS;

  SPI_PORT->DIR |= SPI_CS; // Output
  SPI_PORT->OUT |= SPI_CS; // Active Low, so Initialize High

  EUSCI_B0->CTLW0 &= ~(EUSCI_B_CTLW0_SWRST); // Clear Software Reset
}

// write a 12-bit value to the DAC
void DAC_write( uint16_t mVolts){ // write 0x3--- where - = data into COPI
  uint8_t loByte, hiByte;
  uint16_t data;

  data = DAC_volt_conv(mVolts);

  loByte = data & 0xFF; // mask lower byte
  hiByte = (data >> 8) & 0x0F; // mask upper byte
  hiByte |= 0x30; // set configure bits

  SPI_PORT->OUT &= ~(SPI_CS); // Set CS (chip select) Low before data transmission

  while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG)); // wait for TXBUF to be empty
  EUSCI_B0->TXBUF = hiByte; // Put High Byte into Buffer

  while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG)); // Wait for TXIFG to be Set (TXBUF Empty)
  EUSCI_B0->TXBUF = loByte; // Put Low Byte into Buffer

  while(!(EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG)); // Wait for RXIFG to be Set (RXBUF Empty) // (transmission over)

  SPI_PORT->OUT |= SPI_CS; // Set CS High After Transmission
}

// convert a voltage value into a 12-bit value to control the DAC
uint16_t DAC_volt_conv(uint16_t mVolts){
  uint16_t data = ((mVolts + FLAT_CALIBRATION) * DAC_BITS / MAX_VOLT); // conversion
  if (data >= 0xFFF){ // if input exceeds 3.3V
  data = 0xFFF; // cap at 3.3V
  }
  return data;
}
