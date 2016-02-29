/*
 * buzzer.c
 *
 *  Created on: 29 αταψ 2016
 *      Author: Moran
 */

#include <msp430g2553.h>
#include "definitions.h"

/* This function uses to buzzer to sound the given digit
 * in a series of buzzing sounds */
void BuzzDigit(int digit, int pin_bit)
{
	int i;
	for (i=0; i < digit; i++) {
		P1SEL |= pin_bit;
		__delay_cycles(TICKS_FOR_SEC/1.5);
		P1SEL &= ~pin_bit;
		__delay_cycles(TICKS_FOR_SEC/1.5);
	}
}

/* This function uses the buzzer to buzz the given
 * number digit by digit from lsb to msb*/
void BuzzNumber(int number, int pin_bit)
{
	while (number > 0) {
		BuzzDigit(number % 10, pin_bit);
		__delay_cycles(TICKS_FOR_SEC);
		number /= 10;
	}
}
