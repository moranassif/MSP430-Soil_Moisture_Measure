/*
 * button.c
 *
 *  Created on: 29 αταψ 2016
 *      Author: Moran
 */

#include <msp430g2553.h>
#include "definitions.h"

/* This function configures the button so it will trigger interrupts
 * when pressed.  Those interrupts will be handled by PORT1_ISR() */
void InitializeButton(void)
{
    P1DIR &= ~BUTTON;   // Set button pin as an input pin
    P1OUT |= BUTTON;    // Set pull up resistor on for button
    P1REN |= BUTTON;    // Enable pull up resistor for button to keep pin high until pressed
    P1IES |= BUTTON;    // Enable Interrupt to trigger on the falling edge (high (unpressed) to low (pressed) transition)
    P1IFG &= ~BUTTON;   // Clear the interrupt flag for the button
    P1IE |= BUTTON;     // Enable interrupts on port 1 for the button
}

/* Port 1 interrupt to service the button press */
#pragma vector=PORT1_VECTOR
__interrupt void PORT1_ISR(void)
{
  P1IFG &= ~BUTTON;                 // Clear the interrupt flag for the button
  P1IE &= ~BUTTON;                  // Disable Button interrupt
  WDTCTL = WDT_ADLY_250;             // Start and set watchdog timer (WDT) to trigger every 250ms
  IFG1 &= ~WDTIFG;                  // Clear the interrupt flag for the WDT
  IE1 |= WDTIE;                     // enable WDT interrupt

  P1OUT ^= (LED0);                // Set the LED P1.0 to on
}

// WDT Interrupt Service Routine used to de-bounce button press
#pragma vector=WDT_VECTOR
__interrupt void WDT_ISR(void)
{
    IE1 &= ~WDTIE;                   // disable Watchdog timer (WDT) interrupt
    IFG1 &= ~WDTIFG;                 // clear WDT interrupt flag
    WDTCTL = WDTPW + WDTHOLD;        // put WDT back in hold state
    P1IE |= BUTTON;                  // Reenable interrupts for the button
}
