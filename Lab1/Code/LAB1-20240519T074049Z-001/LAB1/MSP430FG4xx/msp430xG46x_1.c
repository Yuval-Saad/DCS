//******************************************************************************
//   MSP430xG46x Demo:
//
//   Description: Toggle P9 by xor'ing P9 inside of a software loop.
//   ACLK = 32.768kHz, MCLK = SMCLK = default DCO
//******************************************************************************
#include  <msp430xG46x.h>

void main(void)
{
  volatile unsigned int i;

  WDTCTL = WDTPW+WDTHOLD;                   // Stop WDT
  P9DIR |= 0xFF;                            // P9.1 output
  P9OUT = 0xFF;
  
 while(1){
    P9OUT ^= 0xFF;                            // toggeling p9.1
    for(i=50000;i>0;i--);                   // Delay
  }
  
}
