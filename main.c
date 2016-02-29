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

#include <msp430g2553.h>
#include "definitions.h"
#include "buzzer.h"
#include "button.h"
#include "timer.h"

void main(void)
{
	WDTCTL = WDTPW + WDTHOLD;			// Stop WDT

	// Define pins
	P1DIR |= (LED0+LED1);					// set P1.0 (LED1) as output
	P1OUT |= (LED1);					// P1.0 low

	// Timer interrupt
	InitializeTimer0();

	// Button interrupt
	InitializeButton();

	// LPM higher than 1 does not seem to support the button interrupt
	_BIS_SR(LPM1_bits+GIE);        // Enter LPM1 with interrupts enabled
	//_BIS_SR(LPM3_bits + GIE);			// Enter LPM3 w/ interrupt
}
