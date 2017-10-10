# Software PWM
## Brendan Nugent
## Compatible Microcontrollers
* MSP430G2553
* MSP430F5529
* MSP430FR2311
* MSP430FR5994
* MSP430FR6989
## Description
The programs included in this package are used to pulse-width modulate an on-board LED. The duty cycle can be altered by pressing the on-board button. Doing so will increase the
duty cycle by 10%, until it reaches 100%. At the next button press, the duty cycle is reset. 

To implement this, two timers are used. One timer is used solely for the purpose of debouncing. The other timer is set to operate at 1KHz and turn the LED off each time it reaches
the value held in TxCCR1 and back on each time it reaches the value in CCR0 (held at 1000). This CCR1 value, initially set to 500 to create a 50% duty cycle, can be used to alter the duty cycle. By moving the value in CCR1 closer to the value in CCR0 (1000),
the duty cycle is increased. The logic behind this is simple: The longer it takes to get to the LED off stage, the longer the LED is left on. The examples included use a clock with a high enough frequency that
the pulse-width modulation is noticable only by the LED's perceived brightness. As duty cycle increases, the LED appears brighter to the human eye.

The timer interrupt service routines are included here below.

#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMER_1(void)
{
    if (x != 1) // as long as duty cycle isn't 
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