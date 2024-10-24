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


  //Analog Joystick setup
  ANJoyStickArrPortSel &= ~(ANJoyStick_Xaxis + ANJoyStick_Yaxis);
  ANJoyStickArrPortDir &= ~(ANJoyStick_Xaxis + ANJoyStick_Yaxis);

  //For UART - this is I/O config so this is here. UART registers config are in the UARTconfig
  UARTArrPortSel  |= UART_RX + UART_TX;
  UARTArrPortSel2 |= UART_RX + UART_TX;

  //LCD Setup
  LCDDataArrPort      &= ~LCDDataEntries;
  LCDDataArrPortDir   |= LCDDataEntries;
  LCDDataArrPortSel   &= ~LCDDataEntries;
  LCDCTLArrPortSel    &= ~LCDControlEntries;
  LCDCTLArrPortDir    |= LCDControlEntries;
  LCDCTLArrPort       &= ~LCDControlEntries;

  //PB Joystick setup
  PBJoyStickArrPortSel &= ~PBJoyStick;
  PBJoyStickArrPortDir &= ~PBJoyStick;
  PBJoyStickIntEdgeSel |= PBJoyStick; //PB pull-up
  PBJoyStickREN        |= PBJoyStick;
  PBJoyStickIntEN      &= ~PBJoyStick;
  PBJoyStickIntPend    &= ~PBJoyStick;
  PBJoyStickArrPortOut |= PBJoyStick;

  //Motor setup
  MotorArrPortSel     &= ~MotorOut;
  MotorArrPortDir     |= MotorOut;
  MotorArrPort        &= ~MotorOut;

  _BIS_SR(GIE);                     // enable Global interrupts
}

//-------------------------------------------------------------------------------------
//            ADC configuration
//-------------------------------------------------------------------------------------
void ADCconfig(void){
    ADC10CTL0 &= ~ENC;
    ADC10CTL0 = SREF_0 + ADC10SHT_3 + ADC10ON + ADC10IE;
    ADC10CTL1 = INCH_3 + ADC10SSEL_3 + CONSEQ_1; //CONSEQ_1 - sequence of channels
    ADC10AE0 |= BIT3 + BIT0;
}
//void ADCconfig(void){
//    ADC10CTL0 &= ~ENC;
//    ADC10CTL0 = SREF_0 + ADC10SHT_3 + ADC10ON + ADC10IE + MSC;
//    ADC10CTL1 = INCH_3 + ADC10SSEL_3 + CONSEQ_1; //CONSEQ_1 - sequence of channels
//    ADC10AE0 |= BIT3 + BIT0;
//    ADC10DTC1 = 4;
//}

//------------------------------------------------------------------------------------- 
//            Timers configuration
//-------------------------------------------------------------------------------------
void TimersConfig(void){

    TA0CTL = TASSEL_2 + ID_3 + TACLR;  // SMCLK \8 2^17 Hz And count to CCR0
    TA0CCTL0 = CCIE;    //interupt enable
    TA1CTL = TASSEL_2 + ID_3 + TACLR;  // SMCLK \8 2^17 Hz And count to CCR0
    TA1CCTL0 = CCIE;    //interupt enable
} 

//-------------------------------------------------------------------------------------
//            UART configuration
//-------------------------------------------------------------------------------------
void UARTConfig(void){
    //Taken from Example code for UART
    UCA0CTL1 |= UCSSEL_2;                     // CLK = SMCLK
    UCA0BR0 = 104;
    UCA0BR1 = 0x00;
    UCA0MCTL = UCBRS0;
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    UARTIntVector |= UARTRecieveIE;
}
