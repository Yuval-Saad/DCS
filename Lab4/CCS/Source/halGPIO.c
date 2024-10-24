#include  "../header/halGPIO.h"     // private library - HAL layer

char toTransmit;

//--------------------------------------------------------------------
//             System Configuration  
//--------------------------------------------------------------------
void sysConfig(void){ 
	GPIOconfig();
	TIMERconfig();
	UARTconfig();
	ADCconfig();
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

void enableUARTRecevie(void){
    UARTIntVector |= UARTRecieveIE;
}


void enableTimer1(void){
    TimerA1CTL |= MC_1 + TACLR;
    TimerA1BuzCTL = OUTMOD_7;
}

void disableTimer1(void){
    TimerA1CTL &= ~MC_3;
    TimerA1BuzCTL = OUTMOD_0;
}

extern void configTimer1(int val){
    TimerA1Reg0 = val;
    TimerA1RegBuz = val>>1;
}

void startTimer0(void){
    TimerA0CTL |= MC_1 + TACLR;
}

void disableTimer0(void){
    TimerA0CTL &= ~MC_3;
}

void configTimer0(unsigned int val){
    TimerA0Reg0 = val;
}

void EnableADC(void){
    ADC10CTL0 |= ENC + ADC10SC;
}

unsigned int GetNadc(void){
    return ADC10MEM;
}

void updateRGB(char val){
    RGBArrPort &=  ~RGB_ALL;
    RGBArrPort |= (val & RGB_ALL);
}
//*********************************************************************
//            ADC10 Interrupt Service Rotine
//*********************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC10_VECTOR))) ADC10_ISR (void)
#else
#error Compiler not supported!
#endif
{
    LPM0_EXIT;
}

//*********************************************************************
//            TimerA0 Interrupt Service Rotine
//*********************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) Timer_A (void)
#else
#error Compiler not supported!
#endif
{
  LPM0_EXIT;
}

//******************************************************************
// USCI A0/B0 Transmit ISR
//******************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0TX_VECTOR))) USCI0TX_ISR (void)
#else
#error Compiler not supported!
#endif
{
    transmitBuffer = toTransmit;
    UARTIntVector &= ~UARTTransIE;


}

//******************************************************************
// USCI A0/B0 Receive ISR
//******************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCI0RX_ISR (void)
#else
#error Compiler not supported!
#endif
{
    char keyPressed = receiveBuffer;
    char backToState0 = 0;
    if ((keyPressed >= '0' && keyPressed <= '9') || keyPressed == '\n'){
        if (state != state4){
            switch (keyPressed){
                case '1':
                    state = state1;
                    break;
                case '2':
                    state = state2;
                    break;
                case '3':
                    state = state3;
                    break;
                case '4':
                    state = state4;
                    break;
                case '5':
                    state = state5;
                    break;
                case '6':
                    counter = 0;
                    lcd_clear();
                    cleared = 1;
                    backToState0 = 1;
                    break;
                case '7':
                    toTransmit = keyPressed;
                    UARTIntVector = UARTTransIE;
                    backToState0 = 1;
                    break;
                case '8':
                    backToState0 = 1;
                    break;
                default :
                    break;
            }
        }
        else{
            c = keyPressed;
        }
        if (backToState0){
            state = state0;
        }
    }
    LPM0_EXIT;
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
