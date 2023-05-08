/*
 *              Lab 4 Part 1
 *  Created on: May 8, 2023
 *      Author: Caleb Amadoro
 */

#include <msp430.h>

int state = 0;

int main(void)

{
    WDTCTL = WDTPW | WDTHOLD;                         // Stop WDT
    PM5CTL0 &= ~LOCKLPM5;                             // Disable the GPIO power-on default high-impedance mode

    P6DIR |= BIT0;                                    // 6.0 as output
    P6OUT |= BIT0;
    P6DIR |= BIT1;                                    // 6.1 as output
    P6OUT |= BIT1;
    P6DIR |= BIT2;                                    // 6.2 as ouput
    P6OUT |= BIT2;

    TB0CTL = TBSSEL_2 | MC_1 | TBIE;                     // declare timer B0 SMCLK in continuous mode and enabling interrupts
    TB0CCTL1 |= CCIE;                                    // Enable TB0 CCR1 Interrupt
    TB0CCR1 = 500;                                       // Set CCR1 to the value to set the duty cycle (50%)
    TB0CCR0 = 1000;                                      // Set CCR0 to the value of the period (1khz)

    TB1CTL = TBSSEL_2 | MC_1 | TBIE;                    // declare timer B1 SMCLK in continuous mode and enabling interrupts
    TB1CCTL1 |= CCIE;                                   // Enable TB1 CCR1 Interrupt
    TB1CCR1 = 5000;                                     // Set CCR1 to the value to set the duty cycle (50%)
    TB1CCR0 = 1000;                                     // Set CCR0 to the value of the period (1khz)

    __bis_SR_register(LPM3_bits | GIE);                  // Enter LPM3, enable interrupts
    __no_operation();                                    // For debugger

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
            P1OUT &= ~BIT0;
            break;
        case TB0IV_TBCCR2:                       // CCR2 not used
            break;
        case TB0IV_TBIFG:                        // overflow set the pin to 1
            P1OUT |= BIT0;
            break;
        default:                                 // default do nothing
            break;
    }
}

// Timer1_B3 Interrupt Vector (TBIV) handler
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER1_B1_VECTOR
__interrupt void TIMER1_B1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER1_B1_VECTOR))) TIMER1_B1_ISR (void)
#else
#error Compiler not supported!
#endif
{
    if (state == 0){
        state = 1;
        switch(__even_in_range(TB1IV,TB1IV_TBIFG))
        {
            case TB1IV_NONE:                         // no interrupt
                break;
            case TB1IV_TBCCR1:                       // CCR1 Set the pin to a 0
                P6OUT &= ~BIT0;
                break;
            case TB1IV_TBCCR2:                       // CCR2 not used
                break;
            case TB1IV_TBIFG:                        // overflow set the pin to 1
                P6OUT |= BIT0;
                break;
            default:                                 // default do nothing
                break;
        }
    }
    if (state == 1){
        state = 2;
        switch(__even_in_range(TB1IV,TB1IV_TBIFG))
        {
            case TB1IV_NONE:                         // no interrupt
                break;
            case TB1IV_TBCCR1:                       // CCR1 Set the pin to a 0
                P6OUT &= ~BIT1;
                break;
            case TB1IV_TBCCR2:                       // CCR2 not used
                break;
            case TB1IV_TBIFG:                        // overflow set the pin to 1
                P6OUT |= BIT1;
                break;
            default:                                 // default do nothing
                break;
        }
    }
    if (state == 2){
        state = 0;
        switch(__even_in_range(TB1IV,TB1IV_TBIFG))
        {
            case TB1IV_NONE:                         // no interrupt
                break;
            case TB1IV_TBCCR1:                       // CCR1 Set the pin to a 0
                P6OUT &= ~BIT2;
                break;
            case TB1IV_TBCCR2:                       // CCR2 not used
                break;
            case TB1IV_TBIFG:                        // overflow set the pin to 1
                P6OUT |= BIT2;
                break;
            default:                                 // default do nothing
                break;
        }
    }
}
