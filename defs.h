/*
 * defs.h
 *
 * Created: 3/8/2016 7:26:33 PM
 *  Author: Nathan
 */ 


#ifndef DEFS_H_
#define DEFS_H_

// nop define for short delays
#define _NOP() do { __asm__ __volatile__ ("nop"); } while (0)

// digital output pin definitions
#define LEDPIN	0x20	// PORTB 5
#define PWMPIN	0x02	// PORTB 1
#define DIRA1	0x10	// PORTD 4
#define DIRA2	0x20	// PORTD 5
#define SDPINA	0x01	// PORTD 0
#define SDPINB	0x08	// PORTD 3
// RC receiver pin input
#define CH1		0x04	// PORTD 2
// analog pin definitions
#define CURR	0x01	// PORTC 0

// math constants
#define LOOPTIME 1000
#define FSTIME	 500000
#define RCABSMAX 2500
#define RCABSMIN 500
#define RCMIN	1000	// rc stick limits
#define RCMID	1500
#define RCMAX	2000
#define RCDB	25		// deadband
#define MAXPWM	250

#endif /* DEFS_H_ */