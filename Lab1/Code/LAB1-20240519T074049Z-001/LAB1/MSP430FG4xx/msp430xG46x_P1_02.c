//******************************************************************************
//   MSP430xG46x Demo - Software Port Interrupt on P1.4 from LPM4
//
//   Description: A hi to low transition on P1.4 will trigger P1_ISR which,toggles P5.1 
//   ACLK = 32.768kHz, MCLK = SMCLK = default DCO
//******************************************************************************
#include  <msp430xG46x.h>

void main(void)
{
  WDTCTL = WDTPW+WDTHOLD;                   // Stop WDT
  P5DIR |= 0x002;                           // P5.1 output
  P1IE |= 0x010;                            // P1.4 Interrupt enabled
  P1IES |= 0x010;                           // P1.4 hi/low edge
  P1IFG &= ~0x010;                          // P1.4 IFG Cleared

  _BIS_SR(LPM4_bits + GIE);                 // LPM4, enable interrupts

}


// Port 1 interrupt service routine
#pragma vector=PORT1_VECTOR
__interrupt void Port1_ISR (void)
{
  P5OUT ^= 0x02;                            // P5.1 = toggle
  P1IFG &= ~0x010;                          // P1.4 IFG Cleared
}
