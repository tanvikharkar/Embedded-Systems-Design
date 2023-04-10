#Assignment 4 - Clock System and Execution Timing
Create a function to use the DCO as the source for MCLK and set the DCO frequency on the MSP432. The function should be able to set the MSP432 to run at any of the nominal DCO frequencies from 1.5 to 24 MHz. (Do not try to set the DCO to 48 MHz☠) The lone parameter for the function will determine which nominal frequency to set. Create #defines as appropriate for representing the various system frequencies to make your code easier to read. For example, your function calls should look something like this set_DCO(FREQ_12_MHz)

Verify your function works using an oscilloscope. The MCLK signal can be brought out on P4.3 using the appropriate SEL0/1 values.
