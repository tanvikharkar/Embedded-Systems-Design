#include "msp.h"
#include "keypad.h"

void keypad_init(void){
  P4->SEL0 &= ~(KEYPAD_COLS | KEYPAD_ROWS); // set rows and cols to GPIO
  P4->SEL1 &= ~(KEYPAD_COLS | KEYPAD_ROWS);
  P4->DIR &= ~(KEYPAD_COLS); // set cols to input
  P4->DIR |= KEYPAD_ROWS; // set rows as output
  P4->OUT &= ~(KEYPAD_COLS); // make resistors pull-down
  P4->REN |= KEYPAD_COLS; // enable pull-down resistors on input
  P4->OUT |= KEYPAD_ROWS; // set all rows to 1
  return;
}

char getKey(void){
  int cols = P4->IN & KEYPAD_COLS; // read cols to detect button press
  int keyPressed;
  int rows = 0;

  if (cols != 0){
    P4->OUT &= ~(KEYPAD_ROWS);
    while(!(rows > 3)){
      P4->OUT |= (1 << rows + 4) & KEYPAD_ROWS; // set "rows" pin high
      __delay_cycles(25); // delay for button settling
      cols = (P4->IN & KEYPAD_COLS); // read cols
      
      if (cols != 0){ // calculate button from row/col
        if (rows < 3){
          if (cols != 1){ // if not in last column
            if (cols == 1 << 3){ // if in column 0
              keyPressed = rows + (2*rows + 1);
            }
            else if (cols == 1 << 2){ // if in column 1
              keyPressed = rows + 1 + (2*rows + 1);
            }
            else{ // if in column 2
              keyPressed = rows + 2 + (2*rows + 1);
            }
          }
          else{
          keyPressed = 10 + rows;
          }
          P4->OUT |= KEYPAD_ROWS; //set all rows high
        }
        else{
          if (cols == 1 << 3){ // column 0
            keyPressed = 14;
          }
          else if (cols == 1 << 2){ // column 1
            keyPressed = 0;
          }
          else if (cols == 1 << 1){ // column 2
            keyPressed = 15;
          }
          else{ // column 3
            keyPressed = 13;
          }
          P4->OUT |= KEYPAD_ROWS; // set all rows high
        }
        return keyPressed;
      }
      rows++;
    }
  }
  return (char)-1;  // no button press
}
