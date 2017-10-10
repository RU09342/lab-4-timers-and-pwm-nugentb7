/**
 * Pulse Width Modulation using hardware directly from the timer B output
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

    P1DIR |= BIT0;
    P2DIR |= BIT0;             // P2.0 to output
    P2SEL0 |= BIT0;             //Select primary peripheral module function
    P2SEL1 &= ~BIT0;           // as stated in family user guide (slau44g)

    P1OUT&=~BIT0;
    P1DIR &=~(BIT1); //set P1.3 input
    P1REN|=BIT1;//enable pull-up resistor
    P1OUT|=BIT1;
    P1IE |=BIT1;//enable the interrupt on P1.1
    P1IES |= BIT1;//set to look for falling edge
    P1IFG &=~(BIT1);//clear interrupt flag

    initializeTimer();

    __enable_interrupt();


    __bis_SR_register(LPM0 + GIE); // enable interrupts in LPM0

    while(1) {}
}

#pragma vector = TIMER0_B0_VECTOR
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
    TB1CCR0 = 1000-1; // Set max value
    TB1CCTL1 = OUTMOD_7; // set output on reset
    TB1CCR1 = 500-1; // initialize compare register for 50% cycle
    TB1CTL = TBSSEL_2 + MC_1; // Use the SMCLK and up mode

    TB0CCR0 = 4000; // this is used for debounce
    TB0CCTL0 |= CCIE; // set for compare
    TB0CTL |= TBSSEL_2 + MC_1; // use SMCLK and up mode

}

#pragma vector=PORT1_VECTOR
__interrupt void PORT_1(void)
{
    P1OUT ^= BIT0;  //toggle LED
    if (!(P1IN & BIT1)) //only do this if the button has been pressed and not released
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
            TB1CCR1 = x - 1;
        }
    }
    P1IES ^= BIT1; //allow for redo LED toggle when button is released
    P1IFG &= ~BIT1; // clear flag
}

