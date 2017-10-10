/**
 * Pulse width modulation using the internal connection between TIMERA output and P1.0
 * To test the PWM, simply pressing the on-board button, P5.6 will alter the Duty Cycle by 10%
 * 10/9/17
 * Brendan Nugent
 */

#include <msp430.h>

unsigned int x = 500;
unsigned int buttonPressed = 0;
unsigned int count = 0;

void initializeTimer();

void main(void)
{
    WDTCTL = WDTPW + WDTHOLD;  // Stop WDT
    PM5CTL0 &= ~LOCKLPM5;    // disable high-impedance

    P1DIR |= BIT1;
    P1DIR |= BIT0;             // P1.0 to output
    P1SEL0 |= BIT0;             //Select primary peripheral module function
    P1SEL1 &= ~BIT0;           // as stated in family user guide (slau44g)

    P1OUT&=~BIT1;   // clear LED

    P5DIR &=~(BIT6); //set P5.6 input
    P5REN|=BIT6;//enable pull-up resistor
    P5OUT|=BIT6;
    P5IE |=BIT6;//enable the interrupt on P1.1
    P5IES |= BIT6;//set to look for falling edge
    P5IFG &=~(BIT6);//clear interrupt flag

    initializeTimer();

    __enable_interrupt();


    __bis_SR_register(LPM0 + GIE); // enable interrupts in LPM0

    while(1) {}
}

#pragma vector = TIMER1_A0_VECTOR
__interrupt void debounce_timer(void)
{
    if (buttonPressed == 1) // if we just pressed the button, debounce
    {
        if (count == 50)
        {
            buttonPressed = 0;
            count = 0;
        }

        else
        {
            count++;
        }
    }
}

void initializeTimer(void)
{
    TA0CCR0 = 1000-1; // Set max value
    TA0CCTL1 = OUTMOD_7; // set output on reset
    TA0CCR1 = 500-1; // initialize compare register for 50% cycle
    TA0CTL = TASSEL_2 + MC_1; // Use the SMCLK and up mode

    TA1CCR0 = 4000; // this is used for debounce
    TA1CCTL0 |= CCIE; // set for compare
    TA1CTL |= TASSEL_2 + MC_1; // use SMCLK and up mode

}

#pragma vector=PORT5_VECTOR
__interrupt void PORT_5(void)
{
    P1OUT ^= BIT1;  //toggle LED
    if (!(P5IN & BIT6)) //only do this if the button has been pressed and not released
    {
        if (buttonPressed == 0)
        {
            buttonPressed = 1; //debounce now!
            if (x >= 1000) //if we need to go back to off
            {
                x = 1; //reset TA0CCR1
            }

            else
            {
                x += 100; //increment duty cycle
            }
            TA0CCR1 = x - 1;
        }
    }
    P5IES ^= BIT6; //allow for redo LED toggle when button is released
    P5IFG &= ~BIT6; // clear flag
}

