# Hardware PWM
## Brendan Nugent
## Compatible Microcontrollers
* MSP430G2553
* MSP430F5529
* MSP430FR2311
* MSP430FR5994
* MSP430FR6989
## Description
While the previous section made use of software pulse-width modulation through ISR and toggling the LED in code, this section is an exercise in mapping the output from the timer modules
on the boards directly to pins. The data sheets specify which pins may be used to map the output from each timer module and how these pins need to be configured.

On each compatible microcontroller listed above, the output of the timer module A/B may be mapped directly to the LED pin, except for on the MSP430F5529. On the 5529, the example code maps
the output of the timer module to P1.2 and instructs users to connect P1.2 to the pin provided next to LED1. This will allow users to test the on-board button for functionality including the
duty-cycle modulation; as before, the button is used to increase the duty cycle applied to the LED. Further documentation and explanation is included in the source files.