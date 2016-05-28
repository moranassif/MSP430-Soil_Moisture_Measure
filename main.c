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
#include "a2d.h"

int button_was_pressed = 0;

void main(void)
{
	int a2d_value;

	WDTCTL = WDTPW + WDTHOLD;			// Stop WDT

	// Define pins
	P1DIR |= (LED0+LED1);					// set P1.0 (LED1) as output
	P1OUT |= (LED1);					// P1.0 low

	// Timer interrupt
	InitializeTimer0();

	// Button interrupt
	InitializeButton();
	// Buzzer init
	InitializeBuzzer(BUZZER_PIN);

	// A2d stuff
	InitializeA2D();

	while (1)
	{
		__delay_cycles(1000);				// Wait for ADC Ref to settle
		// LPM higher than 1 does not seem to support the button interrupt
		_BIS_SR(CPUOFF+GIE);        // Enter LPM1 with interrupts enabled
		// Handle button
		if (button_was_pressed == 1) {
			button_was_pressed = 0;
			a2d_value = GetA2dSample();
			BuzzNumber(a2d_value, BUZZER_PIN);
		}
	}
}
