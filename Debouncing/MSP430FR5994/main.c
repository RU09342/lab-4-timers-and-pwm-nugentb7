#include <msp430.h> 


/**
 * Debouncing Button
 * Brendan Nugent
 */
unsigned int buttonPress = 0;
unsigned int count = 1;
void timerstart(int f);

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;    // disable high-impedance

    P1DIR |=BIT0; //set P1.0 as output
    P5DIR &=~(BIT6); //set P1.1 input
    P5REN|=BIT6;//enable pull-up resistor
    P5OUT|=BIT6;
    P5IE |=BIT6;//enable the interrupt for P1.1
    P5IES |=BIT6;//set to look for falling edge
    P5IFG &=~(BIT6);//clear interrupt flag

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
        if (count < 25) // This will cause a delay of .01s * 25 = 0.25s
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

#pragma vector=PORT5_VECTOR
__interrupt void PORT_5(void)
{
    if (buttonPress == 0) //if we're ready for another press
    {
        buttonPress = 1; //set high
        P1OUT ^= BIT0; //toggle LED
    }

    P5IFG &= ~BIT6; //clear flag
}
