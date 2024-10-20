#ifndef _bsp_H_
#define _bsp_H_

#include  <msp430g2553.h>          // MSP430x2xx

#define   debounceVal      250

//UART abstraction
#define UARTArrPortSel            P1SEL
#define UARTArrPortSel2           P1SEL2
#define UART_RX                   BIT1
#define UART_TX                   BIT2
#define receiveBuffer             UCA0RXBUF
#define transmitBuffer            UCA0TXBUF
#define UARTIntVector             IE2
#define UARTRecieveIE             UCA0RXIE
#define UARTTransIE               UCA0TXIE

//Analog Joystick abstraction
#define ANJoyStickArrPort         P1IN
#define ANJoyStickArrPortSel      P1SEL
#define ANJoyStickArrPortDir      P1DIR

#define ANJoyStick_Xaxis          BIT0 //A0
#define ANJoyStick_Yaxis          BIT3 //A3

//PB Joystick abstraction
#define PBJoyStickArrPortOut      P2OUT
#define PBJoyStickArrPort         P2IN
#define PBJoyStickArrPortSel      P2SEL
#define PBJoyStickREN             P2REN
#define PBJoyStickArrPortDir      P2DIR
#define PBJoyStickIntEdgeSel      P2IES
#define PBJoyStickIntEN           P2IE
#define PBJoyStickIntPend         P2IFG

//#define PBJoyStick                BIT4
#define PBJoyStick                0x10

//LCD abstraction
#define LCDDataArrPort            P1OUT
#define LCDDataArrPortDir         P1DIR
#define LCDDataArrPortSel         P1SEL
#define LCD_DATA_WRITE            P1OUT
#define LCD_DATA_DIR              P1DIR
#define LCD_DATA_READ             P1IN

#define LCDDataEntries            BIT4 + BIT5 + BIT6 + BIT7

#define LCDCTLArrPort             P2OUT
#define LCDCTLArrPortDir          P2DIR
#define LCDCTLArrPortSel          P2SEL

#define LCDE                      BIT5
#define LCDRS                     BIT6
#define LCDRW                     BIT7
#define LCDControlEntries         BIT5 + BIT6 + BIT7

//Motor abstraction
#define MotorArrPort             P2OUT
#define MotorArrPortDir          P2DIR
#define MotorArrPortSel          P2SEL

#define MotorOut                 BIT0 + BIT1 + BIT2 + BIT3


extern void GPIOconfig(void);
extern void ADCconfig(void);
extern void TimersConfig(void);
extern void UARTConfig(void);

#endif
