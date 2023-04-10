main.c
------------------------------------------------------------------------------------------
#include "msp.h"
#include <math.h>
//Nathan Tang, Tanvi Kharkar, Assignment #1.
unsigned int Add_Bits(unsigned int Num){
unsigned int i = 0;
unsigned int sum = 0;
while(i < 10){
if(Num & (1 << i)){ //move 1 to the left by i; i.e. if i = 2, 0001 -> 0100
sum += (1 << i); // add 2^i to sum
}
i++;
}
return sum;
}
int gcd(int num1, int num2){
if(num1 == 0 || num2 == 0){
return 0;
}
if(num1 == num2){
return num1;
}
if(num1 > num2){
num1 = gcd(num1 - num2, num2);
return num1;
}
num2 = gcd(num1, num2 - num1);
return num2;
}
char bit_manipulator(char num, char opcode){
char data = (opcode & ((1 << 3) - 1)); // grabs last three digits of opcode
char set1 = 0;
char set2 = 0;
if (data == 0){
num = (num & ~(0x0F)); // sets last four bits = 0x00
num = (num | (0xA)); // sets last four bits = 0xA
}
else if (data == 1){
num = (num & (0x0F)); // sets first four bits = 0x00
num = (num | (0x50)); // sets first four bits = 0x5
}
else if (data == 2){
num = (num ^ 17); // 17 = 10001, XOR w/ 17 toggles bits 0 and 4
}
else if (data == 3){
num = (num & 59); // 59 = 0111011, AND w/ 0 sets bits 2 and 6 to 0
}
else if (data == 4){
num = (num | (0x88)); // 0x88 = 10001000, OR w/ 0x88 sets bits 3 and 7 to 1
}
else if (data == 5){
set1 = ((num >> 4) & (0x0F)); // shifts position of top four bits to position of bottom four bits
set2 = ((num << 4) & (0xF0)); // shifts position of bottom four bits to position of top four bits
num = (set1 | set2); // OR set1 and set2 to swap the original top and bottom bits
}
else{
num = (num ^ (0xFF)); // XOR w/ 1 toggles all bits
}
return num;
}

/**
* main.c
*/

void main(void)
{
WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // stop watchdog timer
unsigned int Answer; // test cases
Answer = Add_Bits(10);
int someNum;
someNum = gcd(12,8);
someNum = gcd(3,9);
someNum = gcd(7,1);
someNum = 0xFF;
char bits;
bits = bit_manipulator(0x88, 0x00);
bits = bit_manipulator(0x88, 0x01);
bits = bit_manipulator(0x88, 0x02);
bits = bit_manipulator(0x55, 0x03);
bits = bit_manipulator(0x55, 0x04);
bits = bit_manipulator(0x18, 0x05);
bits = bit_manipulator(0x18, 0x06);
}
