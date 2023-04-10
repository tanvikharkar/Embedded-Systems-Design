#include "msp.h"

#define LCD_DATA 0xF0 // 1111 0000 (pin 2.4 = DB4 pin 2.5 = DB5, pin 2.6 = DB6, pin 2.7 = DB7)
#define LCD_CONTROL 0xE0 // 1110 0000(pin 3.5 = RS, pin 3.6 = R/W, pin 3.7 = E)
#define LCD_RS_RW 0x60 // 0110 0000(pin 3.5 = RS, pin 3.6 = R/W)
#define LCD_E 0x80 // 1000 0000 (pin 3.7 = E)

#define CPU_FREQ 3000000
#define __delay_us(t_us) (__delay_cycles((((uint64_t)t_us)*CPU_FREQ) / 1000000))

void LCD_init(void); // initialize LCD
void LCD_command(uint8_t command); // send LCD a single 8-bit command
void LCD_write_char(uint8_t letter); // write a character to the LCD
void LCD_write_string(char string[]); // write a string to the LCD

/**
* main.c
*/
void main(void)
{
 WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // stop watchdog timer
 LCD_init();

 LCD_write_string("Hello World");
 LCD_command(0xC0); // to write on 2nd line, set address to 0x40 -> 0x80 | 0x40 = 0xC0 into LCD_command
 LCD_write_string("Assignment 3");

 while(1){
 // loop
 }
}

void LCD_command(uint8_t command){
uint8_t high = command & 0xF0; // split command into high and low nibble
uint8_t low = command << 4;
P3->OUT &= ~(LCD_RS_RW); // set RS=0 (instruction code) and RW=0 (write)
P2->OUT &= ~(LCD_DATA); // clear DB7-DB4 before setting high nibble
P2->OUT |= high; // set DB7-DB4 = high nibble
P3->OUT |= LCD_E; // set E=1 (rising edge of enable pulse)
__delay_us(1); // > 140ns (E hold pulse width)
P3->OUT &= ~(LCD_E); // set E=0 (falling edge of enable pulse)
__delay_us(2); // > 1200ns (enable cycle period)
P2->OUT &= ~(LCD_DATA); // clear DB7-DB4 before setting low nibble
P2->OUT |= low; // set DB7-DB4 = low nibble
P3->OUT |= LCD_E; // set E=1 (rising edge of enable pulse)
__delay_us(1); // > 140ns (E hold pulse width)
P3->OUT &= ~(LCD_E); // set E=0 (falling edge of enable pulse)
if (command <= 0x03){ // delay execution time of command (differs for certain commands)
 __delay_us(1530); // delay 1.53ms if command is 'clear display' or 'return home'
}
else{
 __delay_us(39); // delay 39us for every other command
}
return;
}

void LCD_write_char(uint8_t letter){
uint8_t high = letter & 0xF0; // split command into high and low nibble
uint8_t low = letter << 4;
P3->OUT &= ~(LCD_RS_RW); // set RW=0 (write)
P3->OUT |= 0x20; // set RS=1 (DATA)
P2->OUT &= ~(LCD_DATA); // clear DB7-DB4 before setting high nibble
P2->OUT |= high; // set DB7-DB4 = high nibble
P3->OUT |= LCD_E; // set E=1 (rising edge of enable pulse)
__delay_us(1); // > 140ns (E hold pulse width)
P3->OUT &= ~(LCD_E); // set E=0 (falling edge of enable pulse)
__delay_us(2); // > 1200ns (enable cycle period)
P2->OUT &= ~(LCD_DATA); // clear DB7-DB4 before setting low nibble
P2->OUT |= low; // set DB7-DB4 = low nibble
P3->OUT |= LCD_E; // set E=1 (rising edge of enable pulse)
__delay_us(1); // > 140ns (E hold pulse width)
P3->OUT &= ~(LCD_E); // set E=0 (falling edge of enable pulse)
__delay_us(43); // delay 43us for execution time of writing char to LCD
return;
}

void LCD_init(void){
P3->SEL0 &= ~(LCD_CONTROL); // setup GPIO for RS, RW, E, DB7-DB4
P3->SEL1 &= ~(LCD_CONTROL);
P2->SEL0 &= ~(LCD_DATA);
P2->SEL1 &= ~(LCD_DATA);
P3->DIR |= LCD_CONTROL; // set control and data pins to output
P2->DIR |= LCD_DATA;
__delay_us(40000); // delay 40ms
P3->OUT &= ~(LCD_RS_RW); // set RS=0 (instruction code) and RW=0 (write)
P2->OUT &= ~(LCD_DATA); // clear pins before setting to 0x3
P2->OUT |= 0x30; // set DB7-DB4 = 0x3 = 0b0011
P3->OUT |= LCD_E; // set E=1 (rising edge of enable pulse)
__delay_us(1); // > 140ns (E hold pulse width)
P3->OUT &= ~(LCD_E); // set E=0 (falling edge of enable pulse)
__delay_us(39);
LCD_command(0x28); // 4-bit mode, 2 line, 5x8 font
LCD_command(0x28); // 4-bit mode, 2 line, 5x8 font
LCD_command(0x0F); // display on, cursor on and blink
LCD_command(0x01); // clear home
LCD_command(0x06); // increment cursor, no shift
return;
}

void LCD_write_string(char string[]){
int count = 0;
 while (string[count] != '\0'){
 LCD_write_char(string[count]);
 count++;
 }
}
