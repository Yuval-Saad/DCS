#ifndef _bsp_H_
#define _bsp_H_

#include  <msp430xG46x.h>  // MSP430x4xx

#define   debounceVal        250
#define   debounceValKP      3000


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
#define PBs0_3             0x0F

//For real time:
#define PB3                0x08
#define PBs0_3             0x0F


//LCD abstraction
#define LCDDataArrPort    P1OUT
#define LCDDataArrPortDir P1DIR
#define LCDDataArrPortSel P1SEL

#define LCDDataEntries    0xF0

#define LCD_DATA_WRITE    P1OUT
#define LCD_DATA_DIR      P1DIR
#define LCD_DATA_READ     P1IN

#define LCDCTLArrPort     P2OUT
#define LCDCTLArrPortDir  P2DIR
#define LCDCTLArrPortSel  P2SEL

#define LCDE                 0x20
#define LCDRS                0x40
#define LCDRW                0x80
#define LCDControlEntries    0xE0


//Keypad abstraction
#define KeysPortIn        P10IN
#define KeysPortOut       P10OUT
#define KeysPortDir       P10DIR
#define KeysPortSel       P10SEL

#define KEYPADOUT         0x0F
#define KEYPADIN          0xF0
#define KEYPADROW1        0x08
#define KEYPADROW4        0x04
#define KEYPADROW7        0x02
#define KEYPADROWF        0x01
#define KEYPADCOL1        0x80
#define KEYPADCOL2        0x40
#define KEYPADCOL3        0x20
#define KEYPADCOLA        0x10


#define KeysIRQPort        P2IN
#define KeysIRQIntPend     P2IFG
#define KeysIRQIntIE       P2IE
#define KeysIRQIntIes      P2IES
#define KeysIRQIntSel      P2SEL
#define KeysIRQIntDir      P2DIR

#define KEYPADIRQ          0x02

//Timer_B abstraction
#define TimerBCTL           TBCTL
#define TimerBCMP0CTL       TB0CCTL0
#define TimerBCMP0VAL       TB0CCR0

//LEDs abstraction
#define LEDsPortOut       P9OUT
#define LEDsPortDir       P9DIR
#define LEDsPortSel       P9SEL

#define LEDFULLPORT       0xFF





extern void GPIOconfig(void);
extern void TIMERconfig(void);
extern void DMAconfig(void);


#endif
