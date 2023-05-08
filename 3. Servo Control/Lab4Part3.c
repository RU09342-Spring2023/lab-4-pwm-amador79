/*
 *              Lab 4 Part 1
 *  Created on: April 25, 2023
 *      Author: Caleb Amadoro
 */

#include <msp430.h>

int main(void)

{
    WDTCTL = WDTPW | WDTHOLD;                         // Stop WDT
    PM5CTL0 &= ~LOCKLPM5;                             // Disable the GPIO power-on default high-impedance mode

    P2DIR |= BIT7;                                    // 2.7 as output for servo pin
    P2OUT |= BIT7;

    P2DIR &= ~BIT3;                                   // make 2.3 an input with pull up resistor and interrupts enabled
    P2REN |= BIT3;
    P2OUT |= BIT3;
    P2IES &= ~BIT3;
    P2IE |= BIT3;

    P4DIR &= ~BIT1;                                   // make 4.1 an input with pull up resistor and interrupts enabled
    P4REN |= BIT1;
    P4OUT |= BIT1;
    P4IES &= ~BIT1;
    P4IE |= BIT1;

    TB0CTL = TBSSEL_2 | MC_1 | TBIE;                     // declare timer B0 SMCLK in continuous mode and enabling interrupts
    TB0CCTL1 |= CCIE;                                    // Enable TB0 CCR1 Interrupt
    TB0CCR1 = 500;                                       // Set CCR1 to the value to set the duty cycle (50%)
    TB0CCR0 = 1000;                                      // Set CCR0 to the value of the period (1khz)

    __bis_SR_register(LPM3_bits | GIE);                  // Enter LPM3, enable interrupts
    __no_operation();                                    // For debugger

}

#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
    P2IFG &= ~BIT3;                         // Clear P2.3 IFG
    if (TB0CCR1 < 1000){                    // if duty cycle is less than 100%
        TB0CCR1 += 100;                     // +10% duty cycle
    } 
}

#pragma vector=PORT4_VECTOR
__interrupt void Port_4(void)
{
    P4IFG &= ~BIT1;                         // Clear P4.1 IFG
    if (TB1CCR1 > 1000)                     // if duty cycle is greater than than at 0%
        TB1CCR1 -= 100;                     // -10% duty cycle
}

// Timer0_B3 Interrupt Vector (TBIV) handler
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_B1_VECTOR
__interrupt void TIMER0_B1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_B1_VECTOR))) TIMER0_B1_ISR (void)
#else
#error Compiler not supported!
#endif
{

    switch(__even_in_range(TB0IV,TB0IV_TBIFG))
    {
        case TB0IV_NONE:                         // no interrupt
            break;
        case TB0IV_TBCCR1:                       // CCR1 Set the pin to a 0
            P2OUT &= ~BIT7;
            break;
        case TB0IV_TBCCR2:                       // CCR2 not used
            break;
        case TB0IV_TBIFG:                        // overflow set the pin to 1
            P2OUT |= BIT7;
            break;
        default:                                 // default do nothing
            break;
    }
}
