#ifndef _api_H_
#define _api_H_

#include  "../header/halGPIO.h"     // private library - HAL layer

extern void getX(void);
extern void buzzGen(void);
extern void dispalyPoten(void);
extern void countUp(void);
extern void PlayRGB(void);
extern void PBstr(void);


void mul10(unsigned int*);
unsigned int mulBy3222(unsigned int);
void convetIntToStr(char* , unsigned int);
void printCounter(char*, int);
unsigned int convert(int x);
void printIfneeded(char* , char*);
void wait(int , enum FSMstate);

#endif







