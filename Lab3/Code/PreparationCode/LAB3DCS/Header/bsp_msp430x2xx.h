///NOT relevant for lab 3 (DMA)

#ifndef _bsp_H_
#define _bsp_H_

 #include  <msp430g2553.h>          // MSP430x2xx
 //#include  <msp430xG46x.h>  // MSP430x4xx

#define   debounceVal      250
//#define   SMCLKFREQ        1048576  //2^20
#define SMCLKFREQ         1057792 //I think it's a little faster from 2^20

// PushButtons abstraction
#define PBsArrPort         P1IN
#define PBsArrIntPend      P1IFG
#define PBsArrIntEn        P1IE
#define PBsArrIntEdgeSel   P1IES
#define PBsArrPortSel      P1SEL
#define PBsArrPortDir      P1DIR

#define PB0                0x01
#define PB1                0x02
#define PB2                0x04

// Switches abstraction
#define SWsArrPort         P2IN
#define SWsArrPortSel      P2SEL
#define SWsArrPortDir      P2DIR

#define PB3ArrProt         P2IN
#define PB3sArrIntPend     P2IFG
#define PB3sArrIntEn       P2IE
#define PB3sArrIntEdgeSel  P2IES
#define PB3sArrPortSel     P2SEL
#define PB3sArrPortDir     P2DIR

#define PB3                0x02

#define SW0                0x01

//Analogic Input
#define ANArrPort         P1IN
#define ANArrPortSel      P1SEL
#define ANArrPortDir      P1DIR

#define AN                0x08

//LCD abstraction
#define LCDDataArrPort    P1OUT
#define LCDDataArrPortDir P1DIR
#define LCDDataArrPortSel P1SEL

#define LCD_DATA_WRITE    P1OUT
#define LCD_DATA_DIR      P1DIR
#define LCD_DATA_READ     P1IN

#define LCDCTLArrPort     P2OUT
#define LCDCTLArrPortDir  P2DIR
#define LCDCTLArrPortSel  P2SEL

#define LCDE              0x20
#define LCDRS             0x40
#define LCDRW             0x80

//genrator abstraction
#define GENsArrPortIn     P2IN
#define GENsArrPortDir    P2DIR
#define GENsArrPortSel    P2SEL
#define GENsArrPortOut    P2OUT

#define GEN               0x10

//BUZZER abstraction
#define BuzzerArrPort     P2OUT
#define BuzzerArrPortDir  P2DIR
#define BuzzerArrPortSel  P2SEL

#define BUZZER            0x04


extern void GPIOconfig(void);
extern void TIMERconfig(void);
extern void ADCconfig(void);

#endif



