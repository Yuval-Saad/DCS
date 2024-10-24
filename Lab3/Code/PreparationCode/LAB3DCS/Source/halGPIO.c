#include  "../header/halGPIO.h"     // private library - HAL layer


//--------------------------------------------------------------------
//             System Configuration  
//--------------------------------------------------------------------
void sysConfig(void){ 
	GPIOconfig();
	TIMERconfig();
	DMAconfig();
}

//---------------------------------------------------------------------
//            Polling based Delay function
//---------------------------------------------------------------------
void delay(unsigned int t){  // t[msec]
	volatile unsigned int i;

	for(i=t; i>0; i--);
}

//---------------------------------------------------------------------
//            Enable interrupts
//---------------------------------------------------------------------
void enable_interrupts(){
  _BIS_SR(GIE);
}
//---------------------------------------------------------------------
//            Disable interrupts
//---------------------------------------------------------------------
void disable_interrupts(){
  _BIC_SR(GIE);
}

//---------------------------------------------------------------------
//            Enter from LPM0 mode
//---------------------------------------------------------------------
void enterLPM(unsigned char LPM_level){
	if (LPM_level == 0x00)
	  _BIS_SR(LPM0_bits);     /* Enter Low Power Mode 0 */
        else if(LPM_level == 0x01)
	  _BIS_SR(LPM1_bits);     /* Enter Low Power Mode 1 */
        else if(LPM_level == 0x02)
	  _BIS_SR(LPM2_bits);     /* Enter Low Power Mode 2 */
	else if(LPM_level == 0x03)
	  _BIS_SR(LPM3_bits);     /* Enter Low Power Mode 3 */
        else if(LPM_level == 0x04)
	  _BIS_SR(LPM4_bits);     /* Enter Low Power Mode 4 */
}


void enableKeypad(void){
    KeysIRQIntIE   |= KEYPADIRQ;
}

extern void disableKeypad(void){
    KeysIRQIntIE   &= ~KEYPADIRQ;
}


char findKeyPressed(void){
    KeysPortOut = (KEYPADROW1 + KEYPADROW4 + KEYPADROW7);
    if ((KeysPortIn & KEYPADCOLA) == 0) return 16;
    else if ((KeysPortIn & KEYPADCOL3) == 0) return 15;
    else if ((KeysPortIn & KEYPADCOL2) == 0) return 14;
    else if ((KeysPortIn & KEYPADCOL1) == 0) return 13;

    KeysPortOut = (KEYPADROW1 + KEYPADROW4 + KEYPADROWF);
    if ((KeysPortIn & KEYPADCOLA) == 0) return 12;
    else if ((KeysPortIn & KEYPADCOL3) == 0) return 11;
    else if ((KeysPortIn & KEYPADCOL2) == 0) return 10;
    else if ((KeysPortIn & KEYPADCOL1) == 0) return 9;

    KeysPortOut = (KEYPADROW1 + KEYPADROW7 + KEYPADROWF);
    if ((KeysPortIn & KEYPADCOLA) == 0) return 8;
    else if ((KeysPortIn & KEYPADCOL3) == 0) return 7;
    else if ((KeysPortIn & KEYPADCOL2) == 0) return 6;
    else if ((KeysPortIn & KEYPADCOL1) == 0) return 5;

    KeysPortOut = (KEYPADROW4 + KEYPADROW7 + KEYPADROWF);
    if ((KeysPortIn & KEYPADCOLA) == 0) return 4;
    else if ((KeysPortIn & KEYPADCOL3) == 0) return 3;
    else if ((KeysPortIn & KEYPADCOL2) == 0) return 2;
    else if ((KeysPortIn & KEYPADCOL1) == 0) return 1;

    return 0;
}

void EnableInterrupsFromTimer(void){
    TimerBCMP0CTL |= CCIE;
}
void DisableInterrupsFromTimer(void){
    TimerBCMP0CTL &= ~CCIE;
}

void startTimerFor1sec(void){
    TimerBCMP0VAL = 0x8000; //ACLK is 2^15 Hz so we need a value of 2^15 for one sec
    TimerBCTL |= MC_1 + TACLR; //clear the timer and start counting to TimerBCMP0VAL value
}

extern void startTimerFor05sec(void){
    TimerBCMP0VAL = 0x4000; //ACLK is 2^15 Hz so we need a value of 2^14 for one sec
    TimerBCTL |= MC_1 + TACLR; //clear the timer and start counting to TimerBCMP0VAL value
}

void disableTimerB(void){
    TimerBCTL &= ~MC_3; //Stop mode
}

void rstTimerB(void){
    TimerBCTL |= TBCLR;
}

void disablePBsInterrupts(){
    PBsArrIntEn &= ~PBs0_3;
}

void enablePBsInterrupts(){
    PBsArrIntEn |= PBs0_3;
}

void ConfigDMA0ForStart(char* dst , char* src , char size , char srcDir , char dstDir){
    DMA0DA = dst;
    DMA0SA = src;
    DMA0SZ = size;
    if (srcDir == 0) //incremrnt
        DMA0CTL |= DMASRCINCR_3;
    else{
        DMA0CTL &= ~DMASRCINCR_3;
        DMA0CTL |= DMASRCINCR_2;
    }
    if(dstDir == 0) //increment
        DMA0CTL |= DMADSTINCR_3;
    else{
        DMA0CTL &= ~DMADSTINCR_3;
        DMA0CTL |= DMADSTINCR_2;

    }
}

extern void ConfigDMA1ForStart(char* src){
    DMA1SA = src;
    DMA1CTL |= DMAEN;
}


void trigerDMA0(void){
    DMA0CTL |= DMAREQ + DMAEN;
}

void lcd_writeStrLimit(char * str, char num){
    int i;
    for (i=0;i<num;i++){
        lcd_putchar(str[i]);
    }
}

extern void clrLEDS(void){
    LEDsPortOut    &= ~LEDFULLPORT;
}

//*********************************************************************
//            Port1 Interrupt Service Rotine
//*********************************************************************
#pragma vector=PORT1_VECTOR
  __interrupt void PBs_handler(void){
   
	delay(debounceValKP);
//---------------------------------------------------------------------
//            selector of transition between states
//---------------------------------------------------------------------
	if(PBsArrIntPend & PB0){
	  resetState1 = 1;
	  state = state1;
	  PBsArrIntPend &= ~PB0;
    }
    else if(PBsArrIntPend & PB1){
	  state = state2;
	  PBsArrIntPend &= ~PB1; 
    }
	else if(PBsArrIntPend & PB2){ 
	  state = state3;
	  PBsArrIntPend &= ~PB2;
    }
	//for real time
    else if(PBsArrIntPend & PB3){
      state = state4;
      PBsArrIntPend &= ~PB3;
    }
//---------------------------------------------------------------------
//            Exit from a given LPM
//---------------------------------------------------------------------
        switch(lpm_mode){
		case mode0:
		 LPM0_EXIT; // must be called from ISR only
		 break;
		 
		case mode1:
		 LPM1_EXIT; // must be called from ISR only
		 break;
		 
		case mode2:
		 LPM2_EXIT; // must be called from ISR only
		 break;
                 
                case mode3:
		 LPM3_EXIT; // must be called from ISR only
		 break;
                 
                case mode4:
		 LPM4_EXIT; // must be called from ISR only
		 break;
	}
        
}


  //*********************************************************************
  //            Port2 Interrupt Service Rotine
  //*********************************************************************
  #pragma vector=PORT2_VECTOR
    __interrupt void Keypad_handler(void){

      delay(debounceVal); //is it the same as PBs?

  //---------------------------------------------------------------------
  //            selector of transition between states
  //---------------------------------------------------------------------
      if(KeysIRQIntPend & KEYPADIRQ){
        pressed = findKeyPressed();
        KeysPortOut &= ~KEYPADOUT;
        KeysIRQIntPend &= ~KEYPADIRQ;
      }
      switch(lpm_mode){
      case mode0:
       LPM0_EXIT; // must be called from ISR only
       break;

      case mode1:
       LPM1_EXIT; // must be called from ISR only
       break;

      case mode2:
       LPM2_EXIT; // must be called from ISR only
       break;

              case mode3:
       LPM3_EXIT; // must be called from ISR only
       break;

              case mode4:
       LPM4_EXIT; // must be called from ISR only
       break;
  }
  }

    //*********************************************************************
    //            TimerB Interrupt Service Rotine
    //*********************************************************************
    #if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
    #pragma vector=TIMERB0_VECTOR//TIMERB0_VECTOR||TIMER0_B0_VECTOR
    __interrupt void Timer_B (void)
    #elif defined(__GNUC__)
    void __attribute__ ((interrupt(TIMER0_B0_VECTOR))) Timer_B (void)
    #else
    #error Compiler not supported!
    #endif
    {
      timerEnded = 1;
      LPM0_EXIT;
    }

    //*********************************************************************
    //            DMA interrupt service routine
    //*********************************************************************
    #pragma vector = DMA_VECTOR
    __interrupt void DMA_ISR(void)
    {
     DMA0CTL &= ~DMAIFG;
     DMA1CTL &= ~DMAIFG;
     __bic_SR_register_on_exit(LPM0_bits);       // Exit LPMx, interrupts enabled
    }

  //******************************************************************
  // send a command to the LCD
  //******************************************************************
  void lcd_cmd(unsigned char c){

      LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h

      if (LCD_MODE == FOURBIT_MODE)
      {
          LCD_DATA_WRITE &= ~OUTPUT_DATA;// clear bits before new write
          LCD_DATA_WRITE |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;
          lcd_strobe();
          LCD_DATA_WRITE &= ~OUTPUT_DATA;
          LCD_DATA_WRITE |= (c & (0x0F)) << LCD_DATA_OFFSET;
          lcd_strobe();
      }
      else
      {
          LCD_DATA_WRITE = c;
          lcd_strobe();
      }
  }
  //******************************************************************
  // send data to the LCD
  //******************************************************************
  void lcd_data(unsigned char c){

      LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h

      LCD_DATA_WRITE &= ~OUTPUT_DATA;
      LCD_RS(1);
      if (LCD_MODE == FOURBIT_MODE)
      {
          LCD_DATA_WRITE &= ~OUTPUT_DATA;
          LCD_DATA_WRITE |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;
          lcd_strobe();
          LCD_DATA_WRITE &= (0xF0 << LCD_DATA_OFFSET) | (0xF0 >> 8 - LCD_DATA_OFFSET);
          LCD_DATA_WRITE &= ~OUTPUT_DATA;
          LCD_DATA_WRITE |= (c & 0x0F) << LCD_DATA_OFFSET;
          lcd_strobe();
      }
      else
      {
          LCD_DATA_WRITE = c;
          lcd_strobe();
      }

      LCD_RS(0);
  }
  //******************************************************************
  // write a string of chars to the LCD
  //******************************************************************
  void lcd_puts(const char * s){

      while(*s)
          lcd_data(*s++);
  }
  //******************************************************************
  // initialize the LCD
  //******************************************************************
  void lcd_init(){

      char init_value;

      if (LCD_MODE == FOURBIT_MODE) init_value = 0x3 << LCD_DATA_OFFSET;
          else init_value = 0x3F;

      LCD_RS_DIR(OUTPUT_PIN);
      LCD_EN_DIR(OUTPUT_PIN);
      LCD_RW_DIR(OUTPUT_PIN);
      LCD_DATA_DIR |= OUTPUT_DATA;
      LCD_RS(0);
      LCD_EN(0);
      LCD_RW(0);
      DelayMs(15);
      LCD_DATA_WRITE &= ~OUTPUT_DATA;
      LCD_DATA_WRITE |= init_value;
      lcd_strobe();
      DelayMs(5);
      LCD_DATA_WRITE &= ~OUTPUT_DATA;
      LCD_DATA_WRITE |= init_value;
      lcd_strobe();
      DelayUs(200);
      LCD_DATA_WRITE &= ~OUTPUT_DATA;
      LCD_DATA_WRITE |= init_value;
      lcd_strobe();

      if (LCD_MODE == FOURBIT_MODE){
          LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h
          LCD_DATA_WRITE &= ~OUTPUT_DATA;
          LCD_DATA_WRITE |= 0x2 << LCD_DATA_OFFSET; // Set 4-bit mode
          lcd_strobe();
          lcd_cmd(0x28); // Function Set
      }
          else lcd_cmd(0x3C); // 8bit,two lines,5x10 dots

      lcd_cmd(0xF); //Display On, Cursor On, Cursor Blink
      lcd_cmd(0x1); //Display Clear
      lcd_cmd(0x6); //Entry Mode
      lcd_cmd(0x80); //Initialize DDRAM address to zero
  }
  //******************************************************************
  // Delay usec functions
  //******************************************************************
  void DelayUs(unsigned int cnt){

      unsigned char i;
          for(i=cnt ; i>0 ; i--) asm(" nop"); // tha command asm("nop") takes raphly 1usec

  }
  //******************************************************************
  // Delay msec functions
  //******************************************************************
  void DelayMs(unsigned int cnt){

      unsigned char i;
          for(i=cnt ; i>0 ; i--) DelayUs(1000); // tha command asm("nop") takes raphly 1usec

  }
  //******************************************************************
  // lcd strobe functions
  //******************************************************************
  void lcd_strobe(){
    LCD_EN(1);
    asm(" nop");
    asm(" nop");
    LCD_EN(0);
  }
