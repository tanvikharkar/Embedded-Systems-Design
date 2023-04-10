#ifndef KEYPAD_H_
#define KEYPAD_H_

#define RGB_LED 0x07 // 0000 0111
#define KEYPAD_COLS 0x0F // 0000 1111 (pin 4.3 = col 0, pin 4.2 = col 1, etc.)
#define KEYPAD_ROWS 0xF0 // 1111 0000 (pin 4.4 = row 0, pin 4.5 = row 1, etc.)

#define CPU_FREQ 3000000
#define __delay_us(t_us) (__delay_cycles((((uint64_t)t_us)*CPU_FREQ) / 1000000))

//function prototypes
void keypad_init(void);
char getKey(void);

#endif /* KEYPAD_H_ */
