/*
 * fury-bridge.c
 *
 * Created: 3/7/2016 3:43:26 AM
 * Author : Nathan
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "funcs.h"
#include "defs.h"

// pulse width  and rc variables
volatile uint32_t pwRise = 0;
volatile uint16_t pw = 0;
volatile uint8_t pwGood = 0;
uint16_t lastPw = 0;

// output variables
int16_t rawCmd = 0;
uint16_t cmd = 0;
uint8_t dir = 0;

// timer variables
uint32_t loopTimer = 0;
uint32_t fsTimer = 0;
uint8_t failsafe = 1;

// exponential loopup table
uint8_t exp1x[] = {0,0,1,1,2,2,3,4,4,5,5,6,7,7,8,8,9,10,10,11,12,12,13,14,14,15,15,16,17,17,18,19,19,20,21,21,22,23,23,24,25,25,26,27,27,28,29,30,30,31,
	32,32,33,34,35,35,36,37,37,38,39,40,40,41,42,43,43,44,45,46,46,47,48,49,49,50,51,52,53,53,54,55,56,57,57,58,59,60,61,61,62,63,64,65,66,66,67,68,69,
	70,71,72,72,73,74,75,76,77,78,79,80,80,81,82,83,84,85,86,87,88,89,90,91,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,
	112,113,114,115,116,117,118,119,120,122,123,124,125,126,127,128,129,130,131,132,133,135,136,137,138,139,140,141,142,144,145,146,147,148,149,151,152,
	153,154,155,156,158,159,160,161,163,164,165,166,167,169,170,171,172,174,175,176,178,179,180,181,183,184,185,187,188,189,191,192,193,195,196,197,199,
	200,201,203,204,206,207,208,210,211,213,214,215,217,218,220,221,223,224,226,227,228,230,231,233,234,236,237,239,241,242,244,245,247,248,250,251,253,255};

// do stuff on pin change interrupt
// read the pulse width of rc input
ISR(PCINT2_vect){
	if(PIND & CH1){
		pwRise = micros();
		}else{
		pw = (uint16_t)(micros() - pwRise);
		pwGood = 1;
	}
}

int main(void){
	pinInit();
	rcInterruptInit();
	timerInit();
	
    while(1){
		// update loop every 1 ms
		if(micros() - loopTimer > LOOPTIME){
			loopTimer = micros();
			
			// check the current pulse reading for validity
			if(checkPulse(pw)){
				lastPw = pw;
			}else{
				pwGood = 0;
				pw = lastPw;
			}
			
			// bound the pulse width
			if(pw > RCMAX){
				pw = RCMAX;
			}else if(pw < RCMIN){
				pw = RCMIN;
			}
			
			// process the pulse width for output
			if(pwGood){
				rawCmd = mWRADB(pw, rawCmd, RCMIN, RCMAX, RCMID, -MAXPWM, MAXPWM, 0, RCDB, 2);
			}
			
			// get direction, 1 is forward
			if(rawCmd >= 0){
				dir = 1;
			}else{
				dir = 0;
			}
			
			cmd = rawCmd;
			// get magnitude of cmd
			if(!dir){
				cmd = -(cmd);
			}
			
			// exponential lookup and limit output
			cmd = exp1x[cmd];
			if(cmd > MAXPWM){
				cmd = MAXPWM;
			}
			
			// if failsafe isnt true, send output to motor
			if(!failsafe){
				if(dir){
					OCR1A = cmd;
					PORTD |= DIRA1;
					PORTD &= ~(DIRA2);
				}else{
					OCR1A = cmd;
					PORTD &= ~(DIRA1);
					PORTD |= DIRA2;
				}
			}else{
				// else shut off the pwm
				OCR1A = 0;
			}
			
			// failsafe check
			if(micros() - fsTimer > FSTIME){
				fsTimer = micros();
				
				if(pwGood){
					PORTD |= (SDPINA | SDPINB);
					failsafe = 0;
				}else{
					// shutdown the gate drivers
					PORTD &= ~(SDPINA | SDPINB);
					failsafe = 1;
				}
				
				pwGood = 0;
			}
		}
    }
}
