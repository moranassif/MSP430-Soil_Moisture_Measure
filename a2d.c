/*
 * a2d.c
 *
 *  Created on: 29 αταψ 2016
 *      Author: Moran
 */

#include <msp430g2553.h>
#include "definitions.h"

void InitializeA2D(void)
{
	BCSCTL1 |= CALBC1_1MHZ;			// Set range   DCOCTL = CALDCO_1MHZ;
	BCSCTL2 &= ~(DIVS_3);			// SMCLK = DCO = 1MHz
	P1SEL |= A2D_PIN;					// ADC input pin P1.3

	ADC10CTL1 = INCH_2 + ADC10DIV_3 ;        // Channel A2D_BIT, ADC10CLK/3
	ADC10CTL0 = SREF_0 + ADC10SHT_3 + ADC10ON + ADC10IE;  // Vcc & Vss as reference, Sample and hold for 64 Clock cycles, ADC on, ADC interrupt enable
	ADC10AE0 |= A2D_PIN;                      // ADC input enable P1.3
}

unsigned int GetA2dSample() {
	unsigned int return_value;
	ADC10CTL0 |= ENC + ADC10SC;			// Sampling and conversion start
	__bis_SR_register(CPUOFF + GIE);	// Low Power Mode 0 with interrupts enabled
	return_value = ADC10MEM;				// Assigns the value held in ADC10MEM to the integer called ADC_value
	return return_value;
}

// ADC10 interrupt service routine
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR (void)
{
	__bic_SR_register_on_exit(CPUOFF);
}
