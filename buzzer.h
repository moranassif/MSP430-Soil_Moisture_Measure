/*
 * buzzer.h
 *
 *  Created on: 29 αταψ 2016
 *      Author: Moran
 */

#ifndef BUZZER_H_
#define BUZZER_H_

void InitializeBuzzer(int pin_bit);

/* This function uses the buzzer to buzz the given
 * number digit by digit from lsb to msb*/
void BuzzNumber(int number, int pin_bit);

/* This function uses to buzzer to sound the given digit
 * in a series of buzzing sounds */
void BuzzDigit(int digit, int pin_bit);

#endif /* BUZZER_H_ */
