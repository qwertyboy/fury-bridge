/*
 * funcs.c
 *
 * Created: 3/8/2016 7:29:53 PM
 *  Author: Nathan
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "funcs.h"
#include "defs.h"

// timer 0 overflow interrupt for timing
volatile uint32_t timer0_overflow_count = 0;
ISR(TIMER0_OVF_vect){
	timer0_overflow_count++;
}

// timer 1 overflow interrupt for scope trigger
// fires on BOTTOM (center of phase-correct pwm cycle)
/*
ISR(TIMER1_OVF_vect){
	// set pin
	PORTD |= 0x02;
	_NOP();
	_NOP();
	_NOP();
	_NOP();
	_NOP();
	_NOP();
	_NOP();
	_NOP();
	_NOP();
	_NOP();
	_NOP();
	_NOP();
	_NOP();
	_NOP();
	_NOP();
	_NOP();
	PORTD &= ~(0x02);
}
*/

// function to initialize pins
void pinInit(){
	// digital outputs
	DDRB |= (LEDPIN | PWMPIN);
	DDRD |= (DIRA1 | DIRA2 | SDPINA | SDPINB);
	
	// digital input, pull-up
	DDRD &= ~(CH1);
	PORTD |= CH1;
}

// function to set up RC capture interrupt
void rcInterruptInit(){
	// enable on PCINT18
	PCMSK2 |= 0x04;
	// pin change interrupt control register enable 2
	PCICR |= 0x04;
	
	sei();
}

// function to set up and enable timers
void timerInit(){
	// timer/counter0 control register A
	// fast pwm
	TCCR0A |= 0x03;
	// clkio / 64
	TCCR0B |= 0x03;
	// enable overflow interrupt
	TIMSK0 |= 0x01;
	
	// timer/counter1 control register A
	// 8 bit phase correct, set COM1A1
	TCCR1A |= 0x81;
	// clkio / 1 - no prescaling (32kHz?)
	TCCR1B |= 0x01;
	// overflow interrupt for scope triggering
	//TIMSK1 |= 0x01;
}

// timekeeping function yanked from arduino
uint32_t micros(){
	uint32_t m;
	uint8_t t;
	
	cli();
	
	m = timer0_overflow_count;
	t = TCNT0;
	
	if((TIFR0 & 1) && (t < 255)){
		m++;
	}
	
	sei();
	
	return ((m << 8) + t) * 4;
}

// function to check if pulse is withing reasonable limits
uint8_t checkPulse(uint16_t ch){
	return (ch <= RCABSMAX && ch >= RCABSMIN);
}

// some mess copied from rage bridge firmware
// figure out how this works and probably re-write
int16_t mWRADB(int16_t fromVar, int16_t toVar, int16_t fromLow, int16_t fromHigh, int16_t fromZero, int16_t toLow, int16_t toHigh, int16_t toZero, int16_t db, int16_t upi){
	int32_t curVar = (int32_t)fromVar;
	
	if(curVar > fromZero + db){
		curVar = (((int32_t)(fromVar - fromZero - db) * (int32_t)(toHigh - toZero)) / (fromHigh - fromZero - db)) + toZero;
	}else if(curVar < fromZero - db){
		curVar = (((int32_t)(fromVar - fromZero + db) * (int32_t)(toZero - toLow)) / (fromZero - fromLow - db)) + toZero;
	}else{
		curVar = toZero;
	}
	
	int16_t intermed = (curVar - toVar);
	
	if(intermed > upi) return (int16_t)(toVar += upi);
	else if(intermed < -upi) return (int16_t)(toVar -= upi);
	else return (int16_t)curVar;
}