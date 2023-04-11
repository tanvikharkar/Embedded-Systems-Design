#include "msp.h"
#include "DCO.h"
#include "UART.h"

#define PORT_ADC P5 // Analog pin = P5.5
#define ANALOG_PIN BIT5
#define MEM_REG 2 // Memory register 2 used for ADC14
#define DEF_MAX -50000 // Max comparator value in for loop to calculate max of array
#define DEF_MIN 50000 // Min comparator value in for loop to calculate min of array
#define ARRAY_LENGTH 20 // 20 samples in array
#define EMPTY_ARRAY {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} // Initialize an empty array
#define SLOPE 203 // Calculated from array values
#define INTERCEPT 75031 // Calculated from array values
#define CAL 10000 // Value used to calibrate voltages
#define ZERO 0
#define MAX 330
#define PLUS2 2 // Calibration value
#define PLUS3 3 // Calibration value

void ADC14_init(void);
void ADC14_IRQHandler(void);
int val; // Global for saving MEM[2] values
uint8_t ADCflag = 0; // Global flag

void main(void)
{
  WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // stop watchdog timer
  
  int max = DEF_MAX; // Variables for maximum, minimum, and average of array values
  int min = DEF_MIN;
  int avg = 0;
  
  int max_dig1, max_dig2, max_dig3; // Variables for individually transmitting voltage digits to UART
  int min_dig1, min_dig2, min_dig3;
  int avg_dig1, avg_dig2, avg_dig3;
  
  uint8_t counter, i = 0; // Variables for cycling through loops
  int digital_conversion[ARRAY_LENGTH] = EMPTY_ARRAY; // Initialize empty array for saving MEM[2] values
  
  set_DCO(FREQ_24_MHz); // Select HSMCLK for ADC14 and SMCLK/8 for UART
  ADC14_init();
  UART_init();
  
  while(1) // Infinite loop to check for global flag
  {
    if (ADCflag == 1 && i < 20) // Interrupt triggered
    {
      digital_conversion[i] = val; // Save value in array
      i++; // Increment array index
      ADCflag = 0; // Reset interrupt flag
      ADC14->CTL0 |= (ADC14_CTL0_SC); // Perform another sample and conversion
    }
    
    else if (i >= 20) // Array collects 20 samples
    {
      for (counter = 0; counter < 20; counter++)
      {
        if (digital_conversion[counter] > max) // Find max value in array
        {
          max = digital_conversion[counter];
        }
        if (digital_conversion[counter] < min) // Find min value in array
        {
          min = digital_conversion[counter];
        }
        
        avg += digital_conversion[counter]; // Add all values in array to calculate average
      } // end for loop
      
      avg = (SLOPE * (avg/ARRAY_LENGTH) - INTERCEPT) / CAL; // Calculate calibrated average of array
      max = (SLOPE * max - INTERCEPT) / CAL; // Calculate calibrated max of array
      min = (SLOPE * min - INTERCEPT) / CAL; // Calculate calibrated min of array
      
      if (min <= 0) // To avoid negative integers
      {
        min = ZERO;
        max = ZERO;
        avg = ZERO;
      }
      
      else if (min > 0 && min <= 69) // Add 3 for 0V to 69mV, calibration
      {
        min += PLUS3;
        max += PLUS3;
        avg += PLUS3;
      }
      
      else if (min >= 70 && min <= 169) // Add 2 for 70mV to 169mV, calibration
      {
        min += PLUS2;
        max += PLUS2;
        avg += PLUS2;
      }
      
      else if (min > 325) // Max voltage = 3.3V
      {
        min = MAX;
        max = MAX;
        avg = MAX;
      }
      
      // Separating digits of min, max, avg to individually send to UART
      min_dig1 = (min / 100) % 10; // Digit 1
      min_dig2 = (min / 10) % 10; // Digit 2
      min_dig3 = (min / 1) % 10; // Digit 3
      
      max_dig1 = (max / 100) % 10;
      max_dig2 = (max / 10) % 10;
      max_dig3 = (max / 1) % 10;
      
      avg_dig1 = (avg / 100) % 10;
      avg_dig2 = (avg / 10) % 10;
      avg_dig3 = (avg / 1) % 10;
      
      // List of commands to send to terminal via UART to display min, max, avg voltages
      UART_print("Min: ");
      while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
      EUSCI_A0->TXBUF = min_dig1 + '0'; // '0' converts integers to ascii
      UART_print(".");
      while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
      EUSCI_A0->TXBUF = min_dig2 + '0';
      while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
      EUSCI_A0->TXBUF = min_dig3 + '0';
      UART_esc_code(DOWNONE);
      UART_esc_code(LEFT9);
      UART_print("Max: ");
      while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
      EUSCI_A0->TXBUF = max_dig1 + '0';
      UART_print(".");
      while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
      EUSCI_A0->TXBUF = max_dig2 + '0';
      while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
      EUSCI_A0->TXBUF = max_dig3 + '0';
      UART_esc_code(DOWNONE);
      UART_esc_code(LEFT9);
      UART_print("Avg: ");
      while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
      EUSCI_A0->TXBUF = avg_dig1 + '0';
      UART_print(".");
      while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
      EUSCI_A0->TXBUF = avg_dig2 + '0';
      while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
      EUSCI_A0->TXBUF = avg_dig3 + '0';
      UART_esc_code(TOPLEFT);
      
      i = 0; // Reset array index
      max = DEF_MAX; // Reset default max, min, and average values
      min = DEF_MIN;
      avg = 0;
      ADCflag = 1; // Raise interrupt flag
    } // end else if
  } // end while
} // end main

void ADC14_init(void)
{
  ADC14->CTL0 &= ~ADC14_CTL0_ENC; // ADC14 disabled
  
  ADC14->CTL0 = ADC14_CTL0_SSEL__HSMCLK // Select HSMCLK for ADC14
              | ADC14_CTL0_SHP // Sample and hold pulse mode select
              | ADC14_CTL0_SHT0_0 // Sample time of 4 clocks, registers 0-7, 24-31
              | ADC14_CTL0_CONSEQ_0 // Select single conversion
              | ADC14_CTL0_ON; // ADC14 on
  
  ADC14->CTL1 = (MEM_REG<<ADC14_CTL1_CSTARTADD_OFS) // Start conversions at memory register 2;
              | ADC14_CTL1_RES__14BIT // ADC 14-bit resolution
              | ADC14_CTL1_BATMAP;
  
  ADC14->MCTL[2] = ADC14_MCTLN_INCH_0 // Input channel = 0, P5.5
                 | ADC14_MCTLN_VRSEL_0; // Select 3.3V = AVCC
  
  ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC; // ADC enabled, start sample and conversion
  
  PORT_ADC->SEL0 |= ANALOG_PIN; // Configure analog pin, P5.5
  PORT_ADC->SEL1 |= ANALOG_PIN;
  
  ADC14->IER0 = ADC14_IER0_IE2; // Enable interrupt for memory control register 2
  NVIC->ISER[0] = 1 << (ADC14_IRQn);
  __enable_irq();
}

void ADC14_IRQHandler(void)
{
  ADCflag = 1;
  val = ADC14->MEM[2]; // Store value in memory register 2 in global array
}
