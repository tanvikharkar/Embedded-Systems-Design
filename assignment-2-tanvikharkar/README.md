# Assignment 2 - Keypad
Write a keypad function that can determine keypresses and return the value of the key pressed. If no key is pressed, it should return an obvious error value like -1.

Write a main program that uses the keypad function in an infinite while loop and outputs the result to the RGB LED (P2.2 - P2.0) when a key is pressed. If a keypress is detected, the resulting button value should be output to the RGB LED, changing the color. If no key is pressed the LED should not change color from what it was previously. For example, 1 would change the RGB to red (001) and 7 would change it to white (111). Values greater than 7 would repeat colors. 8 would turn the RGB off (1000) and 9 would be red (1001). Extra keys like * and # will depend on what values you assign to them.
