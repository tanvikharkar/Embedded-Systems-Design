# Assignment 5 - Interrupts and TimerA
Part A: 25kHz with 25% Duty Cycle Square Wave
Calculate the necessary CCR values to create a 25 kHz square wave with a 25% duty cycle
using a 24 MHz input clock. It is recommended to draw out the 25 kHz wave along with a 24
MHz wave for reference. Be sure to specify period, high, and low time values of the 25 kHz wave.__
Using SMCLK for TimerA, create a 25 kHz with a 25% duty cycle square wave by setting a pin
high and low in a TimerA ISR. The code in main should only set up TimerA and not keep track
of any timing. No software delays should be used.

Part B: ISR Processing Measurement
Change your code above to use a single ISR which generates a 25 kHz square wave with a 50%
duty cycle using only CCR0 in continuous mode.__
Add a second GPIO output to your system to measure ISR processing time. This can be done
by driving a pin high immediately when entering the ISR and driving it low just before exiting.
