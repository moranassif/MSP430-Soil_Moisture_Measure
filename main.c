/******************************************************************************
 *                          MSP430G2553 ACLK Test
 *
 * Description: This code can be used to test if you have installed the
 *              32.768kHz crystal on your LaunchPad correctly. Using this code
 *              LED1 (on P1.0) will turn on for 1 second, and off for one
 *              second. You can verify this with either an oscilloscope, logic
 *              analyzer, or by inspection. Simple as that!
 *
 * 				This code was originally created for "NJC's MSP430
 * 				LaunchPad Blog".
 *
 * Author:	Nicholas J. Conn - http://msp430launchpad.com
 * Email:	webmaster at msp430launchpad.com
 * Date:	03-15-12
 ******************************************************************************/

#include  <msp430g2553.h>

#define LED0 BIT0
#define LED1 BIT6
#define BUTTON BIT3

unsigned int currentMinutes, currentSeconds;

/* Function Prototype */
void InitializeButton(void);

void main(void)
{
	WDTCTL = WDTPW + WDTHOLD;			// Stop WDT

	// Define pins
	P1DIR |= (LED0+LED1);					// set P1.0 (LED1) as output
	P1OUT |= (LED1);					// P1.0 low

	// Timer interrupt
	BCSCTL1 |= DIVA_3;				// ACLK/8
	BCSCTL3 |= XCAP_3;				//12.5pF cap- setting for 32768Hz crystal

	currentMinutes = 0;
	currentSeconds = 0;

	CCTL0 = CCIE;					// CCR0 interrupt enabled
	CCR0 = 511;					// 512 -> 1 sec, 30720 -> 1 min
	TACTL = TASSEL_1 + ID_3 + MC_1;			// ACLK, /8, upmode

	// Button interrupt
	InitializeButton();

	// LPM higher than 1 does not seem to support the button interrupt
	_BIS_SR(LPM1_bits+GIE);        // Enter LPM1 with interrupts enabled
	//_BIS_SR(LPM3_bits + GIE);			// Enter LPM3 w/ interrupt
}

// Timer A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
	P1OUT ^= LED1;					// Toggle LED
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
