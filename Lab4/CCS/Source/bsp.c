#include  "../header/bsp_msp430x2xx.h"    // private library - BSP layer

//-----------------------------------------------------------------------------  
//           GPIO congiguration
//-----------------------------------------------------------------------------
void GPIOconfig(void){
  
  WDTCTL = WDTHOLD | WDTPW;		// Stop WDT
   
  //Taken from Example code for UART:
  if (CALBC1_1MHZ==0xFF)                  // If calibration constant erased
  {
    while(1);                               // do not load, trap CPU!!
  }
  DCOCTL = 0;                               // Select lowest DCOx and MODx settings
  BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
  DCOCTL = CALDCO_1MHZ;


  // RGB setup
  RGBArrPortSel     &= ~RGB_ALL;
  RGBArrPortDir     |= RGB_ALL;
  RGBArrPort        &= ~RGB_ALL;

  //BUZZER setup
  BuzzerArrPortSel  |= BUZZER;
  BuzzerArrPortDir  |= BUZZER;
  BuzzerArrPort     &= ~BUZZER;

  //Potenciometer setup
  ANArrPortSel      &= ~AN;
  ANArrPortDir      &= ~AN;

  //LCD Setup
  LCDDataArrPort      &= ~LCDDataEntries;
  LCDDataArrPortDir   |= LCDDataEntries;
  LCDDataArrPortSel   &= ~LCDDataEntries;
  LCDCTLArrPortSel    &= ~LCDControlEntries;
  LCDCTLArrPortDir    |= LCDControlEntries;
  LCDCTLArrPort       &= ~LCDControlEntries;

  //For UART - this is I/O config so this is here. UART registers config are in the UARTconfig
  UARTArrPortSel |= UART_RX + UART_TX;
  UARTArrPortSel2 |= UART_RX + UART_TX;

  _BIS_SR(GIE);                     // enable interrupts globally
}                             
//------------------------------------------------------------------------------------- 
//            Timers configuration
//-------------------------------------------------------------------------------------
void TIMERconfig(void){

    TA0CTL = TASSEL_2 + ID_3 + TACLR;  // SMCLK \8 2^17 Hz
    TA0CCTL0 = CCIE;    //interupt enable

    TA1CTL = TASSEL_2 + TACLR;  // SMCLK 2^20 Hz
    TA1CCTL2 = OUTMOD_0;    //interupt enable
} 

//------------------------------------------------------------------------------------- 
//            DMA configuration
//-------------------------------------------------------------------------------------
void ADCconfig(void){
    ADC10CTL0 &= ~ENC;
    ADC10CTL0 = SREF_0 + ADC10SHT_2 + ADC10ON + ADC10IE;
    ADC10CTL1 = INCH_3 + ADC10SSEL_3;
    ADC10AE0 |= BIT3;
}

//-------------------------------------------------------------------------------------
//            UART configuration
//-------------------------------------------------------------------------------------
void UARTconfig(void){
    //Taken from Example code for UART
    UCA0CTL1 |= UCSSEL_2;                     // CLK = SMCLK
    UCA0BR0 = 104;
    UCA0BR1 = 0x00;
    UCA0MCTL = UCBRS0;
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
}

 
             
             
            
  

