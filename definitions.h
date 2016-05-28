/*
 * definitions.h
 *
 *  Created on: 29 αταψ 2016
 *      Author: Moran
 */

#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

// General
#define TICKS_FOR_SEC 1000000L // Approx
#define PWM_PERIOD 500

// Port 1
#define LED0 BIT0
#define LED1 BIT6
#define BUTTON BIT3
#define A2D_PIN BIT2 // Change also INCH in a2d if using bit other than 2

// Port 1
#define BUZZER_PIN BIT3

// Globals
extern int button_was_pressed;

#endif /* DEFINITIONS_H_ */
