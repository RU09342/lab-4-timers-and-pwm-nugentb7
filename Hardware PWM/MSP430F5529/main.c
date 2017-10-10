/**
 * Pulse width modulation using the internal connection between TIMERA output and P1.2
 * To test the PWM, simply connect the jumper next to LED0 to P1.2
 * Pressing the on-board button, P2.1 will alter the Duty Cycle by 10%
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

    P1DIR |= BIT2;
    P4DIR |= BIT7;             // P4.7 to output for button acknowledgement
    P1SEL |= BIT2;             //Select primary peripheral module function on P1.2 --> Connect to P1.0 for test
    P4OUT &= ~BIT7;

    P1OUT&=~BIT0;
    P2DIR &=~(BIT1); //set P2.1 input
    P2REN|=BIT1;//enable pull-up resistor
    P2OUT|=BIT1;
    P2IE |=BIT1;//enable the interrupt on P2.1
    P2IES |= BIT1;//set to look for falling edge
    P2IFG &=~(BIT1);//clear interrupt flag

    initializeTimer();

    __enable_interrupt();


   // __bis_SR_register(LPM0 + GIE); // enable interrupts in LPM0

    while(1) {}
}

#pragma vector = TIMER1_A0_VECTOR
__interrupt void debounce_timer(void)
{
    if (buttonPressed == 1) // if we just pressed the button, debounce
    {
        if (count == 75)
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
    TA0CCR0 = 1000 - 1; // Set max value
    TA0CCTL1 = OUTMOD_7; // set output on reset
    TA0CCR1 = 500 - 1; // initialize compare register for 50% cycle
    TA0CTL = TASSEL_2 + MC_1; // Use the SMCLK and up mode

    TA1CCR0 = 4000; // this is used for debounce
    TA1CCTL0 |= CCIE; // set for compare
    TA1CTL |= TASSEL_2 + MC_1; // use SMCLK and up mode

}

#pragma vector=PORT2_VECTOR
__interrupt void PORT_2(void)
{
    P4OUT ^= BIT7;  //toggle LED
    if (!(P2IN & BIT1)) //only do this if the button has been pressed and not released
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
    P2IES ^= BIT1; //allow for redo LED toggle when button is released
    P2IFG &= ~BIT1; // clear flag
}

