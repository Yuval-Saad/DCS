//******************************************************************************
//   MSP430xG46x Demo
//   Description: Poll P1.4 in a loop, if HI, P5.1 is set, if LOW, P5.1 clear.
//   ACLK = 32.768kHz, MCLK = SMCLK = default DCO
//******************************************************************************
#include  <msp430xG46x.h>

void main(void)
{
  WDTCTL = WDTPW+WDTHOLD;                   // Stop WDT
  P5DIR |= 0x002;                           // P5.1 output

  while(1)
    if (P1IN&0x10)                          // P1.4 hi/low?
      P5OUT |= 0x002;                       // P5.1 = 1 / LED ON
    else
      P5OUT &= ~0x002;                      // P5.1 = 0 / LED OFF
}
