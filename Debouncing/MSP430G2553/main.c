#include <msp430.h> 


/**
 * Brendan Nugent
 * Debouncing Button on MSP430G2553
 * By providing a delay timed by Timer A set up for 100Hz, we can temporarily stop the effect from bouncing until it halts
 * 10/9/17
 */
unsigned int buttonPress = 0;
unsigned int count = 1;
void timerstart(int f);

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    P1DIR |=BIT0; //set P1.0 as output
    P1DIR &=~(BIT3); //set P1.3 input
    P1REN|=BIT3;//enable pull-up resistor
    P1OUT|=BIT3;
    P1IE |=BIT3;//enable the interrupt for P1.1
    P1IES |=BIT3;//set to look for falling edge
    P1IFG &=~(BIT3);//clear interrupt flag

    timerstart(100); //initialize timer A with 100Hz freq

    __enable_interrupt();

    __bis_SR_register(LPM0 + GIE); // enable interrupts in LPM0

    while(1){}
}

void timerstart(int f) // call function with desired frequency to initialize timer module
{
    int n;
    TA0CCTL0 = CCIE; //Enable interrupt in compare mode
    TA0CTL = TASSEL_2 + MC_1 + ID_2; //SMCLK, up mode
    n = 250000 / f; //250000 / 100 = 2500
    TA0CCR0 = n; // [( 10^6 / 4) / (2500) = 100Hz)]
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMER_A(void)
{
    if (buttonPress == 1)
    {
        if (count < 25) // This will cause a delay of .01s * 10 = 0.1s
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
        while (!(P1IN & BIT3));
    }

    P1IFG &= ~BIT3; //clear flag
}
