/**
 * Pulse width modulation using the timer A interrupts to toggle the LED
 * To test the PWM, simply pressing the on-board button, P1.1 will alter the Duty Cycle by 10%
 * 10/9/17
 * Brendan Nugent
 */


#include <msp430.h>

unsigned int x = 500;
unsigned int buttonPressed = 0;
unsigned int count = 0;

void initializeTimer();

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    P1DIR |= BIT0;
    P1SEL &= ~BIT0;
    P4DIR |= BIT7;
    P4SEL &= ~BIT7;

    TA0CCTL1 = OUTMOD_7;
    TA0CCR1 = x - 1;

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

void initializeTimer(void)
{
    TA0CCTL1 = CCIE; //Enable interrupt in compare mode
    TA0CCTL0 = CCIE;
    TA0CTL = TASSEL_2 + MC_1; //SMCLK, up mode
    TA0CCR0 = 1000 - 1;
    TA0CCR1 = 500 - 1;

    TA1CCR0 = 4000;
    TA1CCTL0 |= CCIE;
    TA1CTL |= TASSEL_2 + MC_1;
}



#pragma vector = TIMER1_A0_VECTOR
__interrupt void debounce_timer(void)
{
    if (buttonPressed == 1)
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

#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMER_1(void)
{
    if (x != 1)
        P1OUT |= BIT0; // LED on
    TA0CCTL0 &= ~BIT0;
}

#pragma vector = TIMER0_A1_VECTOR
__interrupt void TIMER_0(void)
{
     if (TA0CCR1 != 1000)
         P1OUT &= ~BIT0; //LED off
     TA0CCTL1 &= ~BIT0;
}

#pragma vector=PORT1_VECTOR
__interrupt void PORT_1(void)
{
    P4OUT ^= BIT7;
    if (!(P1IN & BIT1))
    {
        if (buttonPressed == 0)
        {
            buttonPressed = 1;
            if (x >= 1000)
            {
                x = 1;
            }

            else
            {
                x += 100;
            }
            TA0CCR1 = x - 1;
        }
    }
    P1IES ^= BIT1;
    P1IFG &= ~BIT1;
}


