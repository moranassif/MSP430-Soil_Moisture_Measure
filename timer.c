/*
 * timer.c
 *
 *  Created on: 29 αταψ 2016
 *      Author: Moran
 */

#include <msp430g2553.h>
#include "definitions.h"

unsigned int currentMinutes, currentSeconds;

// Timer A0 interrupt service routine
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer_A (void)
{
	P1OUT ^= LED1;					// Toggle LED
}

void InitializeTimer0(void)
{
	BCSCTL1 |= DIVA_3;				// ACLK/8
	BCSCTL3 |= XCAP_3;				//12.5pF cap- setting for 32768Hz crystal

	currentMinutes = 0;
	currentSeconds = 0;

	/*** Timer0_A Set-Up ***/
	TA0CCR0 |= PWM_PERIOD;			// PWM period
	TA0CCR1 |= PWM_PERIOD/2;		// TA0CCR1 PWM duty cycle
	TA0CCTL1 |= OUTMOD_7;			// TA0CCR1 output mode = reset/set
	TA0CTL |= TASSEL_1 + MC_1;		// SMCLK, Up Mode (Counts to TA0CCR0)

	/*** Timer1_A Set-Up ***/
	TA1CCTL0 = CCIE;				// CCR0 interrupt enabled
	TA1CCR0 = 511;					// 512 -> 1 sec, 30720 -> 1 min
	TA1CTL = TASSEL_1 + ID_3 + MC_1;			// ACLK, /8, upmode
}
