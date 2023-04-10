main.c
#include "msp.h"
#define COL_PINS (BIT4 | BIT5 | BIT6 | BIT7) // The columns are the upper bits (0xF0)
#define ROW_PINS (BIT0 | BIT1 | BIT2 | BIT3) // The rows are the lower bits (0x0F)

void keypad_init(void);
char getKey(void);
void led_init(void);
void led(char);
void main(void)
{
 WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // stop watchdog timer
 unsigned char key = 0; // initialize the key to 0 and it stores the value of the button pressed
 keypad_init();
 led_init();

while(1)
{
 key = getKey();
 if (key != 0xFF) // lights the led only if the button is pressed
 led(key);
}
}

void keypad_init(void)
{
 P4->SEL0 &= ~(COL_PINS | ROW_PINS);
 P4->SEL1 &= ~(COL_PINS | ROW_PINS);
 P4->DIR &= ~(COL_PINS);
 P4->DIR |= ROW_PINS; // set rows to output and columns to input
 P4->REN |= COL_PINS; // enable the pull down resistor for columns
 P4->OUT |= ROW_PINS; // drive rows to 1
 P4->OUT &= ~(COL_PINS); // set columns to 0
}

char getKey(void)
{
 const char rowSelection[] = {0x01, 0x02, 0x04, 0x08};
 int cols; // stores the input
 int rows = 0; // index for the while loop
 P4->OUT |= ROW_PINS; // drive row pins high
 cols = P4->IN & COL_PINS; // masks the input and assigns it to cols

 if (cols == 0x00) //since row pins are high, if cols = 0, no button was pushed
 return 0xFF; // so, return -1
 while (rows < 4)
 {
  P4->OUT &= ~(ROW_PINS); // set row pins to 0
  P4->OUT |= rowSelection[rows]; // step through the rows
  __delay_cycles(25); // delay for button setting
  cols = P4->IN & COL_PINS; // masks the input and assigns it to cols

  if (cols != 0x00)
  break; // if a button is pressed, leave the loop
  rows++;
 }
 
P4->OUT |= ROW_PINS; // set rows to 1

if (rows == 4) // in case a button is missed
return 0xFF; // return -1

if (rows == 0) // if row 0 (bottom row)
{
 if (cols == 0x10) // button equals D = 13
 return 0x0D;
 if (cols == 0x20) // button equals # = 14
 return 0x0E;
 if (cols == 0x40) // button equals 0
 return 0x00;
 if (cols == 0x80) //button equals * = 15
 return 0x0F;
}

if (rows == 1) // if row 1
{
 if (cols == 0x10) // button equals C= 12
 return 0x0C;
 if (cols == 0x20) // button equals 9
 return 0x09;
 if (cols == 0x40) // button equals 8
 return 0x08;
 if (cols == 0x80) // button equals 7
 return 0x07;
}

if (rows == 2) // if row 2
{
 if (cols == 0x10) // button equals B = 11
 return 0x0B;
 if (cols == 0x20) // button equals 6
 return 0x06;
 if (cols == 0x40) // button equals 5
 return 0x05;
 if (cols == 0x80) //button equals 4
 return 0x04;
}

if (rows == 3) // if row 3 (top row)
{
 if (cols == 0x10) // button equals A=10
 return 0x0A;
 if (cols == 0x20) // button equals 3
 return 0x03;
 if (cols == 0x40) // button equals 2
 return 0x02;
 if (cols == 0x80) //button equals 1
 return 0x01;
 }

return 0xFF; // just to make sure it returns -1 // if a button isn't pressed
}

void led_init(void)
{
 P2->DIR = 0x07; // set the led pins as output
 P2->OUT &= ~0x07; //set output to equal 0
 P2->SEL0 &= ~0x07;
 P2->SEL1 &= ~0x07;
}
void led(char button)
{
 button &= 0x07; // mask the button
 P2->OUT = (P2->OUT & ~0x07) | button; // the led output equals to the value of the button pressed
}
