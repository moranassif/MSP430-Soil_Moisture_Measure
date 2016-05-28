/*
 * buzzer.c
 *
 *  Created on: 29 ���� 2016
 *      Author: Moran
 */

#include <msp430g2553.h>
#include "definitions.h"

void InitializeBuzzer(int pin_bit)
{
	P2DIR |= pin_bit;
	P2OUT &= ~pin_bit;
}

/* This function uses to buzzer to sound the given digit
 * in a series of buzzing sounds */
void BuzzDigit(int digit, int pin_bit)
{
	int i;
	for (i=0; i < digit; i++) {
		P2OUT |= pin_bit;
		__delay_cycles(TICKS_FOR_SEC/3);
		P2OUT &= ~pin_bit;
		__delay_cycles(TICKS_FOR_SEC/3);
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
