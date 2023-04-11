# Project 3 - Digital Multimeter
The purpose of this project is to design and implement a digital multimeter (DMM) that can measure voltage and frequency to meet the specified requirements below. This project is a more complicated coding challenge than the previous projects and requires a strategy to be formulated before starting the coding.
System Requirements
1. The DMM shall measure voltage.
a. Voltage measurements shall be limited to 0 to 3.3 volts.
b. Voltage measurements shall be accurate to +/- 25 mv
c. Voltage measurements shall be updated at least every 3.5 seconds.
d. The DMM shall have DC and AC measurements.
i. Mode selection can be selected manually by the user
47
ii. DC and AC measurements can be displayed in separate modes or simultaneously
2. The DMM DC measurements
a. DC measurement shall be an average of multiple measurements over a time period
greater than 1 ms and less than 100 ms.
3. The DMM AC measurements
a. AC measurements shall display RMS value
i. The RMS measurement shall be true-RMS (includes the DC offset)
b. AC measurements shall display peak-to-peak value.
c. AC measurements shall work for various waveforms
i. Sine waveforms
ii. Saw-tooth waveforms
iii. Square waveforms
iv. Other periodic waveforms
d. AC measurements shall work for waveforms of various amplitudes and offsets
i. The maximum voltage that shall be measured is 3 V
ii. The minimum voltage that shall be measured is 0 V
iii. The minimum peak-to-peak voltage that shall be measured is 0.5 V
iv. The maximum DC offset value that shall be measured is 2.75 V
4. The DMM shall measure frequency.
a. Frequency measurements shall be limited from 1 to 1000 HZ.
b. Frequency measurements shall be accurate to within 10% or 5 Hz whichever is smaller.
c. Frequency measurements shall work for various waveforms.
i. Sine waveforms
ii. Saw-tooth waveforms
iii. Square waveforms
iv. Other periodic waveforms
5. The DMM shall have a terminal-based interface.
48
a. The terminal shall operate at 115200 baud.
b. The terminal shall utilize VT100 escape codes
i. The terminal shall display all fields in non-changing locations
c. The terminal shall update at least every 3.5 seconds
d. The terminal shall display AC voltages as described above.
e. The terminal shall display DC voltages as described above.
f. The terminal shall display frequency as described above.
g. The terminal shall organize the presentation of information.
i. AC, DC, and frequency shall be simple to read.
ii. The display may use horizontal and vertical lines (borders) to organize the
presentation of information.
h. The terminal shall use character based bar-graphs for voltages being measured.
i. The bar graphs shall be a single line or column of characters
ii. The bar graphs shall have delineators, e.g. a scale, indicating the value being
measured.
iii. The bar graphs shall have a length or height that is proportional to the value
being measured.
iv. The bar graphs shall respond in real-time to changes in AC or DC measurements
v. The AC measurements shall have a bar-graph for the RMS voltage.
vi. The DC measurement shall have a bar-graph for the DC voltage.
