#include  "../header/bsp_msp430x2xx.h"    // private library - BSP layer

//-----------------------------------------------------------------------------  
//           GPIO congiguration
//-----------------------------------------------------------------------------
void GPIOconfig(void){
 // volatile unsigned int i; // in case of while loop usage
  
  WDTCTL = WDTHOLD | WDTPW;		// Stop WDT
   
  // PushButtons Setup
  PBsArrPortSel &= ~0x07;
  PBsArrPortDir &= ~0x07;
  PBsArrIntEdgeSel |= 0x03;          // pull-up mode
  PBsArrIntEdgeSel &= ~0x04;         // pull-down mode
  PBsArrIntEn |= 0x07;
  PBsArrIntPend &= ~0x07;            // clear pending interrupts
  
  PB3sArrPortSel &= ~PB3;
  PB3sArrPortDir &= ~PB3;
  PB3sArrIntEdgeSel &= ~PB3;         // pull-down mode
  PB3sArrIntEn |= PB3;
  PB3sArrIntPend &= ~PB3;            // clear pending interrupts

  // SWsButtons Setup
  SWsArrPortSel &= ~0x01;
  SWsArrPortDir &= ~0x01;

  //LCD Setup
   LCDDataArrPort      &= ~0xF0;
   LCDDataArrPortDir   |= 0xF0;
   LCDDataArrPortSel   &= ~0xF0;
   LCDCTLArrPortSel    &= ~0xE0;
   LCDCTLArrPortDir    |= 0xE0;
   LCDCTLArrPort       &= ~0xE0;

   // Generator Setup
   GENsArrPortDir       &= ~0x10;
   GENsArrPortSel       |= 0x10;

   // Buzzer Setup
   BuzzerArrPortDir |= 0x04;             // P2.2 Output compare - '1'
   BuzzerArrPortSel |= 0x04;             // P2.2 Select = '1'
   BuzzerArrPort    &= ~0x04;             // P2.2 out = '0'

  _BIS_SR(GIE);                     // enable interrupts globally
}                             
//------------------------------------------------------------------------------------- 
//            Timers congiguration 
//-------------------------------------------------------------------------------------
void TIMERconfig(void){
    TA0CTL = TASSEL_2 + ID_3 + TACLR;  // timer is off SMCLK /8 = 0.5 sec interupt
    TA0CCTL0 = CCIE;    //interupt enable
    TA0CCR0 = 0xFFFF;      // value of 0.5 sec
    TA1CTL = TASSEL_2 + TACLR;
    TA1CCTL1 = OUTMOD_0;   // for state3
    TA1CCTL2 = CM_1 + SCS + CAP;     // for state1
} 
//------------------------------------------------------------------------------------- 
//            ADC congiguration 
//-------------------------------------------------------------------------------------
void ADCconfig(void){
    ADC10CTL0 &= ~ENC;
    ADC10CTL0 = SREF_0 + ADC10SHT_2 + ADC10ON + ADC10IE;
    ADC10CTL1 = INCH_3 + ADC10SSEL_3;
    ADC10AE0 |= BIT3;
}              

           
             

 
             
             
            
  

