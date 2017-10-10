#include <msp430.h>

unsigned int x = 500;
unsigned int buttonPressed = 0;
unsigned int count = 0;

void initializeTimer();

void main(void)
{
    WDTCTL = WDTPW + WDTHOLD;  // Stop WDT

    P1DIR |= BIT0;
    P1DIR |= BIT6;             // P1.6 to output
    P1SEL |= BIT6;             //Select primary peripheral module function
    P1SEL2 &= ~BIT6;           // as stated in family user guide (slau144j)

    P1OUT&=~BIT0;
    P1DIR &=~(BIT3); //set P1.3 input
    P1REN|=BIT3;//enable pull-up resistor
    P1OUT|=BIT3;
    P1IE |=BIT3;//enable the interrupt on P1.1
    P1IES |= BIT3;//set to look for falling edge
    P1IFG &=~(BIT3);//clear interrupt flag

    initializeTimer();

    __enable_interrupt();


    __bis_SR_register(LPM0 + GIE); // enable interrupts in LPM0

    while(1) {}
}

#pragma vector = TIMER1_A1_VECTOR
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

#pragma vector=PORT1_VECTOR
__interrupt void PORT_1(void)
{
    P1OUT ^= BIT0;  //toggle LED
    if (!(P1IN & BIT3)) //only do this if the button has been pressed and not released
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
    P1IES ^= BIT3; //allow for redo LED toggle when button is released
    P1IFG &= ~BIT3; // clear flag
}

