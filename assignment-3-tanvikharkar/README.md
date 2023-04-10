# Assignment 3 - Parallel LCD
Write a short program to initialize the LCD and write a single character to the display.

Modularize your code, writing useful functions to make later use of the LCD display easy to integrate. Minimum functions to be written include:
LCD_init(void); // initialize LCD
LCD_command(uint8_t command); // Send LCD a single 8-bit command
LCD_write_char(uint8_t letter); // write a character to the LCD

Note that some #define can be used for specific commands like CLEAR_HOME, NEW_LINE, or
CURSOR_BLINK.

Write a program using your functions to print “Hello World” on the top line and “Assignment 3” on the bottom line of the LCD.
