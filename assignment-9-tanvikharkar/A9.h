#ifndef A9_H_
#define A9_H_
#include "stdint.h"
#define FREQ_1_5_MHz (uint32_t) 0x00000000
#define FREQ_3_MHz (uint32_t) 0x00010000
#define FREQ_6_MHz (uint32_t) 0x00020000
#define FREQ_12_MHz (uint32_t) 0x00030000
#define FREQ_24_MHz (uint32_t) 0x00040000
#define CPU_FREQ 3000000

volatile int freq;

#define ADC_PIN BIT1
#define COMP_P BIT6
#define COMP_N BIT7
#define COMP_OUT BIT2
#define FREQ_TA 3000000

void set_DCO(uint32_t frequency);

#define UART_PORT P1
#define RXD_PIN BIT2
#define TXD_PIN BIT3

#define BAUD_BRW 1
#define BAUD_BRF 10
#define BAUD_BRS 0x00

#define ESC_CHAR 0x1B // VT-100 command escape character
#define MOVE_ONE "[1B" // esc code down one line
#define MOVE_LEFT1 "[10D"
#define MOVE_LEFT2 "[11D"

#define CURSOR_UPPER_LEFT "[H"
#define CLEAR "[2J"

#define __delay_us(t_us) (__delay_cycles((((uint64_t)t_us)*CPU_FREQ) / 1000000))

#define MAX 0xFFFF
#define MAX_FREQ 1000

void uart_init(); // UART initialization function
void uart_esc_code(char write_string[]); // execute escape code command
void uart_print(char string[]); // print single line string to terminal command
void COMP_Einit(void);
void TA0_0_IRQHandler(void);
void TA0_N_IRQHandler(void);

#endif /* A9_H_ */
