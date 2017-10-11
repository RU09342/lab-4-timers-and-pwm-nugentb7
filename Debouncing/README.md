# Software Debouncing
## Brendan Nugent
## Compatible Microcontrollers
* MSP430G2553
* MSP430F5529
* MSP430FR2311
* MSP430FR5994
* MSP430FR6989

## Overview
The above microcontrollers may be used to toggle the on-board LED without bounce affecting the LED's toggle. The MSP430FR5994 has allowed for two buttons to be used to toggle two of the on-board
LED's, with both of the buttons debounced.

## Description
While computer engineers read the input of a switch digitally, it is still a mechanical system in its primitive state. Because of this, embedded engineers may come across an issue
termed bounce. This is when the metal contacts of a switch continue to make contact and separate, "fooling" the embedded program into thinking the switch is toggling, after it is pressed.

To prevent this, embedded engineers may add bounce-handling functionality to their programs. Each of these programs, although for different microcontrollers, use an on-board button to
toggle an LED. To prevent bouncing from affecting the LED toggle, the programs employ a debouncing feature through the use of the boards' timer modules.

The steps behind this are to initialize a timer module to operate at 100Hz. With interrupts enabled, we use this timer's overflow ISR to increment a count after the button has been pressed.
Once this count reaches 10, the hold on the button press is released and the typical button ISR can be used. 

This creates a 0.1s delay between when the button interrupt is first triggered and when it may take effect again. This effectively disables the interrupt while the button is bouncing,
avoiding unwanted LED toggles.

To aid in this discussion, we may look at a code snippet used in the MSP430FR2311 code.

#pragma vector = TIMER0_B0_VECTOR
__interrupt void TIMER_B(void)
{
    if (buttonPress == 1)
    {
        if (count < 10) // This will cause a delay of .01s * 10 = 0.1s
        {
            count++;
        }

        else //reset buttonpress and count so the button can be noticed
        {
            buttonPress = 0;
            count = 1;
        }
    }
}

#pragma vector=PORT1_VECTOR
__interrupt void PORT_1(void)
{
    if (buttonPress == 0) //if we're ready for another press
    {
        buttonPress = 1; //set high
        P1OUT ^= BIT0; //toggle LED
    }
    P1IES ^= BIT1;
    P1IFG &= ~BIT1; //clear flag
}

Walking through this, a user presses the on-board button, triggering the PORT_1 interrupt. The buttonPress variable is set to 1. This variable tells the program to begin debouncing in the
TIMER_B interrupt. In this ISR, a count is incremented until it hits 10 on each TIMER B overflow. This overflow occurs every 0.01s, meaning it takes 0.1s to release the hold on the button interrupt.

## Extra Tasks
Multi-Switch Debounce