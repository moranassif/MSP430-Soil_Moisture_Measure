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

/////////////////
// Definitions //
/////////////////
// General
#define TICKS_FOR_SEC 1000000L // Approx

// Port 1
#define LED0 BIT0
#define LED1 BIT6
#define BUTTON BIT3
#define A2D_PIN BIT2 // Change also INCH in a2d if using bit other than 2

// Port 2
#define POWER_OUTPUT_PIN BIT4

#define BASE_ADDR 0x0E000
#define CUR_ADDR_ADDR 0xF600

// Globals
int sample_and_log;

unsigned int currentMinutes = 50;
unsigned int currentHours = 21;
unsigned int *cur_addr_p = (unsigned int *)CUR_ADDR_ADDR ; // Address of the flash memory segment starting
unsigned int *addr;
int button_was_pressed = 0;
int powering_up;


////////////
// Timers //
////////////

// Timer A0 interrupt service routine
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer_A1 (void)
{
	 if (++currentMinutes >= 60) {
		 currentMinutes = 0;
		 if (++currentHours >=24) {
			 currentHours = 0;
		 }
	 }
	 // Log the sample every 20 minutes
	 if (currentMinutes % 20 == 0) {
		 sample_and_log = 1;
		 __bic_SR_register_on_exit(CPUOFF);        // Return to active mode
	 }
}

void InitializeTimer0(void)
{
	BCSCTL1 |= DIVA_3;				// ACLK/8
	BCSCTL3 |= XCAP_3;				//12.5pF cap- setting for 32768Hz crystal

	currentMinutes = 00;
	currentHours = 19;

	/*** Timer0_A Set-Up ***/
	/*TA0CCR0 |= PWM_PERIOD;			// PWM period
	TA0CCR1 |= PWM_PERIOD/2;		// TA0CCR1 PWM duty cycle
	TA0CCTL1 |= OUTMOD_7;			// TA0CCR1 output mode = reset/set
	TA0CTL |= TASSEL_2 + MC_1;		// SMCLK, Up Mode (Counts to TA0CCR0)*/

	/*** Timer1_A Set-Up ***/
	TA1CCTL0 = CCIE;				// CCR0 interrupt enabled
	TA1CCR0 = 30720;					// 512 -> 1 sec, 30720 -> 1 min
	TA1CTL = TASSEL_1 + ID_3 + MC_1;			// ACLK, /8, upmode
}

/////////
// A2D //
/////////

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
	SetPowerOutputHigh();
	ADC10CTL0 |= ENC + ADC10SC;			// Sampling and conversion start
	__bis_SR_register(CPUOFF + GIE);	// Low Power Mode 0 with interrupts enabled
	return_value = ADC10MEM;				// Assigns the value held in ADC10MEM to the integer called ADC_value
	SetPowerOutputLow();
	return return_value;
}

// ADC10 interrupt service routine
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR (void)
{
	__bic_SR_register_on_exit(CPUOFF);
}


////////////
// Button //
////////////
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
    sample_and_log = 0;
}

/* Port 1 interrupt to service the button press */
#pragma vector=PORT1_VECTOR
__interrupt void PORT1_ISR(void)
{
  // Button handling
  P1IFG &= ~BUTTON;                 // Clear the interrupt flag for the button
  P1IE &= ~BUTTON;                  // Disable Button interrupt
  WDTCTL = WDT_ADLY_250;            // Start and set watchdog timer (WDT) to trigger every 250ms
  IFG1 &= ~WDTIFG;                  // Clear the interrupt flag for the WDT
  IE1 |= WDTIE;                     // enable WDT interrupt

  // Led handling
  P1OUT |= (LED0);                  // Set the LED P1.0 to on

  // Set global
  sample_and_log = 1;
  button_was_pressed = 1;
  __bic_SR_register_on_exit(CPUOFF);        // Return to active mode
}

// WDT Interrupt Service Routine used to de-bounce button press
#pragma vector=WDT_VECTOR
__interrupt void WDT_ISR(void)
{
	P1IFG &= ~BUTTON;
	P1OUT &= ~LED0;                  // Set the LED P1.0 to off
	IE1 &= ~WDTIE;                   // disable Watchdog timer (WDT) interrupt
    IFG1 &= ~WDTIFG;                 // clear WDT interrupt flag
    WDTCTL = WDTPW + WDTHOLD;        // put WDT back in hold state
    P1IE |= BUTTON;                  // Reenable interrupts for the button
}


//////////////////
// Power output //
//////////////////
void InitializePowerOutput()
{
	P2DIR |= POWER_OUTPUT_PIN;
	P2OUT &= ~POWER_OUTPUT_PIN;
}

void SetPowerOutputHigh()
{
		P2OUT |= POWER_OUTPUT_PIN;
		__delay_cycles(1000);
}

void SetPowerOutputLow()
{
		P2OUT &= ~POWER_OUTPUT_PIN;
		__delay_cycles(1000);
}


///////////
// Flash //
///////////

void flash_write(unsigned int *addr, unsigned int data)
{
  __disable_interrupt();
  FCTL2 = FWKEY + FSSEL_1 + FN0;       // Clk = SMCLK/4
  FCTL3 = FWKEY;                       // Clear Lock bit
  FCTL1 = FWKEY + WRT;                 // Set WRT bit for write operation

  *addr = data;         // copy value to flash

  FCTL1 = FWKEY;                        // Clear WRT bit
  FCTL3 = FWKEY + LOCK;                 // Set LOCK bit
  while(BUSY & FCTL3);
  __enable_interrupt();
}

void flash_erase(unsigned int *addr)
{
  __disable_interrupt();               // Disable interrupts. This is important, otherwise,
                                       // a flash operation in progress while interrupt may
                                       // crash the system.
  while(BUSY & FCTL3);                 // Check if Flash being used
  FCTL2 = FWKEY + FSSEL_1 + FN3;       // Clk = SMCLK/4
  FCTL1 = FWKEY + ERASE;               // Set Erase bit
  FCTL3 = FWKEY;                       // Clear Lock bit
  *addr = 0;                           // Dummy write to erase Flash segment
  while(BUSY & FCTL3);                 // Check if Flash being used
  FCTL1 = FWKEY;                       // Clear WRT bit
  FCTL3 = FWKEY + LOCK;                // Set LOCK bit
  __enable_interrupt();
}

unsigned int get_last_address() {

	if (*cur_addr_p == 0xFFFF) {
		return BASE_ADDR; // Old value
	} else {
		return *cur_addr_p; // New one
	}

}

//////////
// Main //
//////////

void main(void)
{
	int a2d_value;

	WDTCTL = WDTPW + WDTHOLD;			// Stop WDT

	// Define pins
	P1DIR |= (LED0+LED1);					// set P1.0 (LED1) as output
	P1OUT &= ~(LED1);					// P1.0 low

	powering_up = 1;

	// Timer interrupt
	InitializeTimer0();

	// Button interrupt
	InitializeButton();
	// Power output init
	InitializePowerOutput();

	// A2d stuff
	InitializeA2D();

	addr = (unsigned int *) get_last_address();

	while (1)
	{
		// LPM higher than 1 does not seem to support the button interrupt
		_BIS_SR(CPUOFF+GIE);        // Enter LPM1 with interrupts enabled
		// Handle button
		if (sample_and_log == 1) {
			P1OUT |= LED1;	// Activate LED1 and VCC to the resistance meter
			__delay_cycles(1000);				// Wait for ADC Ref to settle
			sample_and_log = 0;
			a2d_value = GetA2dSample();
			addr = (unsigned int*)get_last_address();
			// Write sample and time to the flash
			flash_write(addr++, currentHours);
			flash_write(addr++, currentMinutes);
			flash_write(addr++, a2d_value);
			if (button_was_pressed) {
				flash_write(addr++, 0x17); // Sanity
			} else {
				if (powering_up) {
					flash_write(addr++, 0x35); // Signal
					powering_up = 0;
				} else {
					flash_write(addr++, 0x42); // Sanity
				}
			}
			// Save the address
			flash_erase((unsigned int *)CUR_ADDR_ADDR);
			flash_write((unsigned int *)CUR_ADDR_ADDR, (unsigned int)addr);
			button_was_pressed = 0;
			P1OUT &= ~LED1;	// Deactivate LED1 and VCC to the resistance meter
			// If full, stop increasing
			if ((unsigned int)addr >= CUR_ADDR_ADDR) {
				P1OUT |= LED0;
				_BIS_SR(CPUOFF);        // Enter LPM1
			}
		}
	}
}
