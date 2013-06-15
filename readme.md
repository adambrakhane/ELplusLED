Circuit Info
------------
**Inputs:**

*	Pink button
*	Blue button
*	Green button
*	Yellow button
*	Missile switch
*	Knob 1 (0-1023)

**Outputs:**

*	Analog (0-255)
*	LED RGB
*	Digital (High or low)
*	2x EL RGB
*	Audio?


**Design notes**

*	Do not use pins 8 and 9 for input. The pins are permanently connected to triac inputs.


Code Info
---------
**Notes**

*	Prescaler: [Info](http://www.marulaberry.co.za/index.php/tutorials/code/arduino-adc/)
*	Arduino Ports to pins mapping: [Image](http://arduino.cc/en/Hacking/PinMapping168)  
*	Queue Library: [Here](https://github.com/adambrakhane/AVRQueue)