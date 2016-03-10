/*
 * funcs.h
 *
 * Created: 3/8/2016 7:31:27 PM
 *  Author: Nathan
 */ 


#ifndef FUNCS_H_
#define FUNCS_H_

void pinInit(void);
void rcInterruptInit(void);
void timerInit(void);
uint32_t micros(void);
uint8_t checkPulse(uint16_t);
int16_t mWRADB(int16_t fromVar, int16_t toVar, int16_t fromLow, int16_t fromHigh, int16_t fromZero, int16_t toLow, int16_t toHigh, int16_t toZero, int16_t db, int16_t upi);

#endif /* FUNCS_H_ */