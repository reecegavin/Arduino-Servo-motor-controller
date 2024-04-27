# Arduino-Servo-motor-controller
The program controls the position setting of a servomotor, using PWM and the Timer/Counter 2 output OC2A, where a PWM on time value of 1ms sets a position of 90 degrees and a PWM on time value of 2ms sets a position of 180 degrees. The OC2A output is found on PORTB Bit 3.
For servomotor control, the PWM frequency should be approximately 20ms.
The OC2A signal from the ATMega328P is shared with PORTB Bit 3. The PWM output on OC2A is controlled by the data value written to the OCR2A register. 
The ADC2 input is driven by the Shield Potentiometer with the ADC jumper posts set to the lower position as viewed with the USB connector at the top.  The ADC should be initialised so that it runs in Auto Trigger Mode, with Timer 0 Overflow set as the Retrigger source. Timer 0 is set for overflow every 10 ms, Timer0 interrupts are NOT used.
The Timer/Counter2 output OC2A is used as the PWM output, but Timer/Counter2 interrupts are not enabled.
The Timer/Counter1 input, ICP1, is connected to a 555 Timer, which produces a periodic signal in a frequency range from approximately 2 kHz to 13 kHz, controlled by a potentiometer on the shield.

The program responds to the following single character commands received over the serial port:

‘1’ to ‘8’: Set the servomotor position using the OC2RA register, to an angular position between 90 degrees (1ms PWM on time) and 180 degrees (2ms PWM on time). You must convert the char value to an integer and set the PWM based on this value. If the servomotor is being set by the ADC reading, do not change the servo position, but send a message to the user, saying that the setting is controlled by the ADC.
‘T’ or ‘t’: Report the period of the 555 Timer in microseconds.
‘L’ or ‘l’: Report the time taken by the low pulse of the 555 Timer signal in microseconds
‘H’ or ‘h’: Report the time taken by the high pulse of the 555 Timer signal in microseconds
'C' or 'c': Continuously report the Timer input period in microseconds.
‘E’ or ‘e’: Stop continuous reporting of Timer input.
‘A’ or ‘a’: Report the ADC2 conversion result. This is the ADC value.
