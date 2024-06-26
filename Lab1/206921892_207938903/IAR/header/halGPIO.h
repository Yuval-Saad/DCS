#ifndef _halGPIO_H_
#define _halGPIO_H_

#include  "../header/bsp_msp430x2xx.h"    		// private library - BSP layer
#include  "../header/app.h"    		// private library - APP layer


extern enum FSMstate state;   // global variable
extern enum SYSmode lpm_mode; // global variable
extern enum CountType tp;     // global variable

extern void sysConfig(void);
extern void print2LEDs(unsigned char);
extern void clrLEDs(void);
extern void toggleLEDs(char);
extern void setLEDs(char);
extern unsigned char readSWs(void);
extern void delay(unsigned int);
extern void enterLPM(unsigned char);
extern void SetLEDs(unsigned char);
extern void shiftLEDcyclic (unsigned char);
extern unsigned char GetLEDs(void);
extern void incLEDs(char);
extern void disablePBInterrupts(unsigned char);
extern void enablePBInterrupts(unsigned char);
extern void enable_interrupts();
extern void disable_interrupts();
extern void SignalUp(void);
extern void SignalDown(void);

extern __interrupt void PBs_handler(void);

#endif







