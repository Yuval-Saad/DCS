#include  "../header/halGPIO.h"     // private library - HAL layer

unsigned char whoWantsToTransmit; //0 Painter data , 1 PB
int TrasmitIndex;

void sysConfig(void){ //System Configuration
	GPIOconfig();
	ADCconfig();
	TimersConfig();
	UARTConfig();
}

void enterLPM(){
    _BIS_SR(LPM0_bits); // Enter Low Power Mode 0
}

void SampleJoyStick(unsigned int* xVal, unsigned int* yVal){
    ADC10CTL0 |= ENC + ADC10SC;
    enterLPM();
    *yVal = ADC10MEM;
    while (ADC10CTL1 & ADC10BUSY){
        ADC10CTL0 &= ~ADC10SC;
        ADC10CTL0 |= ADC10SC;
        enterLPM();
    }
    *xVal = ADC10MEM;
    ADC10CTL0 &= ~ENC;
    ADC10CTL0 &= ~ADC10SC;
}

void StepMotorClockwise(unsigned char* status){
    *status = *status >> 1;
    if (*status == 0x00) *status = 0x08;
    MotorArrPort &= ~0x0F;
    MotorArrPort |= *status;
}

void StepMotorCounterClockwise(unsigned char* status){
    *status = *status << 1;
    if (*status == 0x10) *status = 0x01;
    MotorArrPort &= ~0x0F;
    MotorArrPort |= *status;
}

void StartTimerA1(unsigned int millis){
    unsigned int leftToWait = millis;
    while (leftToWait > 500){
        TA1CCR0 = 0xFFFF;
        TA1CTL |= MC_1 + TACLR;
        enterLPM();
        leftToWait = leftToWait - 500;
    }
    TA1CCR0 = leftToWait << 7;
    TA1CTL |= MC_1 + TACLR;
    enterLPM();
}

unsigned char CommandCode(unsigned char code){
    return (code > 0x00 && code < 0x03) || (code > 0x03 && code < 0x0A) || (code > 0x0A && code < 0x12);
}

void ControlOp(unsigned char code){
    switch (code){

        case 0x01:
            state = Sleep;
            break;

        case 0x02:
            state = Manual_Sleep;
            break;

        case 0x04:
            state = Calibration_Sleep;
            break;

        case 0x05:
            state = Send_Script_Status;
            break;

        case 0x06:
            state = Joystick_Control;
            break;

        case 0x07:
            state = Rotate;
            break;

        case 0x08:
            state = Manual_Sleep;
            break;

        case 0x09:
            state = Painter;
            break;

        case 0x0B:
            state = Calibration_before_count;
            break;

        case 0x0C:
            state = Load;
            NumFile = 0;
            break;

        case 0x0D:
            state = Load;
            NumFile = 1;
            break;

        case 0x0E:
            state = Load;
            NumFile = 2;
            break;

        case 0x0F:
            state = Execute;
            NumFile = 0;
            break;

        case 0x10:
            state = Execute;
            NumFile = 1;
            break;

        case 0x11:
            state = Execute;
            NumFile = 2;
            break;

        default :
            break;
    }
}

void StartPBs(){
    PBJoyStickIntPend &= ~PBJoyStick;
    PBJoyStickIntEN |= PBJoyStick;
}

void EnablePBs(){
    PBJoyStickIntEN |= PBJoyStick;
}

void DisablePBs(){
    PBJoyStickIntEN &= ~PBJoyStick;
}

void TransmitData(unsigned char user){
    if (user == 0)
        whoWantsToTransmit = 0;
    if (user == 1)
        whoWantsToTransmit = 1;
    if (user == 2)
        whoWantsToTransmit = 2;
    if (user == 3)
        whoWantsToTransmit = 3;
    if (user == 4)
        whoWantsToTransmit = 4;
    if (user == 5)
        whoWantsToTransmit = 5;
    if (user == 6)
        whoWantsToTransmit = 6;
    if (user == 7)
        whoWantsToTransmit = 7;
    TrasmitIndex = 0;
    UARTIntVector |= UARTTransIE;
    enterLPM();
}

void delay(unsigned int t){  // t[msec]
    volatile unsigned int i;

    for(i=t; i>0; i--);
}
/*old
void ReadSegment(char* buffer, unsigned char segNum){
    char *Flash_ptr;                          // Flash pointer
    unsigned int i;
    Flash_ptr = header.filePonters[segNum];
    for (i = 0; i<64 ; i++){
        buffer[i] = *Flash_ptr++;
    }
}
*/
//new
void ReadSegment(char* buffer, unsigned char segNum){
    char *Flash_ptr;                          // Flash pointer
    unsigned int i;
    unsigned int size = (segNum == 3)? 68 : 64;
    if (segNum==3){
        Flash_ptr = (char*)header_segment_pointer;
    }
    else{
        Flash_ptr = header.filePonters[segNum];
    }
    for (i = 0; i<size ; i++){
        buffer[i] = *Flash_ptr++;
    }
}
//
/*old
void WriteSegment(char* writeBuffer, unsigned char segNum){
    char *Flash_ptr;                          // Flash pointer
    unsigned int i;
    unsigned int numOfBytesToWrite;
    Flash_ptr = header.filePonters[segNum];
    numOfBytesToWrite = (segNum == 3) ? 64 : header.sizes[segNum];

    FCTL1 = FWKEY + ERASE;                    // Set Erase bit
    FCTL3 = FWKEY;                            // Clear Lock bit
    *Flash_ptr = 0;                           // Dummy write to erase Flash segment

    //while (FCTL3 & BUSY);

    FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation

    for (i=0; i<numOfBytesToWrite; i++)
    {
      *Flash_ptr++ = writeBuffer[i];                   // Write value to flash
    }

    FCTL1 = FWKEY;                            // Clear WRT bit
    FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
}
*/
//new
void WriteSegment(char* writeBuffer, unsigned char segNum){
    char *Flash_ptr;                          // Flash pointer
    unsigned int i;
    unsigned int numOfBytesToWrite;
    if (segNum==3){
        Flash_ptr = (char*)header_segment_pointer;
    }
    else{
        Flash_ptr = header.filePonters[segNum];
    }
    numOfBytesToWrite = (segNum == 3) ? 68 : header.sizes[segNum];

    FCTL1 = FWKEY + ERASE;                    // Set Erase bit
    FCTL3 = FWKEY;                            // Clear Lock bit
    *Flash_ptr = 0;                           // Dummy write to erase Flash segment

    //while (FCTL3 & BUSY);

    FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation

    for (i=0; i<numOfBytesToWrite; i++)
    {
      *Flash_ptr++ = writeBuffer[i];                   // Write value to flash
    }

    FCTL1 = FWKEY;                            // Clear WRT bit
    FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
}
//

void FlashConfig(){
    char* Flash_ptr;
    Flash_ptr = (char*)header_segment_pointer;
    if (Flash_ptr[0] != 'E'){
        WriteSegment((char*)&header , 3);
    }
    ReadSegment((char*)&header, 3); //0 indicates segment A
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
//            TimerA1 Interrupt Service Rotine
//*********************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer1_A0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER1_A0_VECTOR))) Timer1_A0_ISR (void)
#else
#error Compiler not supported!
#endif
{
    LPM0_EXIT;
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
    if (CommandCode(receiveBuffer)) ControlOp(receiveBuffer); //Command
    else recieveData = receiveBuffer; //Data
    LPM0_EXIT;
}

//*********************************************************************
//            Port2 Interrupt Service Rotine
//*********************************************************************
#pragma vector=PORT2_VECTOR
  __interrupt void PBs_handler(void){

    delay(debounceVal);
    if (PBJoyStickIntPend & PBJoyStick){
        PBJoyStickIntPend &= ~PBJoyStick;
    }
    if (state == Painter){
        PB_Pressed = 1;
    }
    else if (state == Calibration_before_count){
        state = Calibration_count;
    }
    else if (state == Calibration_count){
        state = Calibration_Sleep;
    }
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
      if (whoWantsToTransmit == 0){ //Data for Painter
          if (TrasmitIndex == 0){
              transmitBuffer = 0x07;
              TrasmitIndex++;
          }
          else if (TrasmitIndex == 5){
              UARTIntVector &= ~UARTTransIE;
              LPM0_EXIT;
          }
          else{
              transmitBuffer = dataToSend[TrasmitIndex - 1];
              TrasmitIndex++;
          }
      }
      else if (whoWantsToTransmit == 1){ //PB
          if (TrasmitIndex == 1){
              UARTIntVector &= ~UARTTransIE;
              LPM0_EXIT;
          }
          else {
              transmitBuffer = 0x01;
              TrasmitIndex++;
          }
      }
      else if (whoWantsToTransmit == 2){ //s_numOfStepsIn360
            if (TrasmitIndex == 2){
                UARTIntVector &= ~UARTTransIE;
                LPM0_EXIT;
            }
            else if (TrasmitIndex == 1){
                transmitBuffer = ((unsigned char)((header.s_numOfStepsIn360%100) & 0xFF))+150;
                TrasmitIndex++;
            }
            else {
                transmitBuffer = ((unsigned char)((header.s_numOfStepsIn360/100) & 0xFF))+150;
                TrasmitIndex++;
            }
        }
      else if (whoWantsToTransmit == 3){ //JoyStickMiddle
            if (TrasmitIndex == 4){
                UARTIntVector &= ~UARTTransIE;
                LPM0_EXIT;
            }
            else if (TrasmitIndex == 3){
                transmitBuffer = ((unsigned char)((header.JoyStickMiddleY%100) & 0xFF))+150;
                TrasmitIndex++;
            }
            else if (TrasmitIndex == 2){
                transmitBuffer = ((unsigned char)((header.JoyStickMiddleY/100) & 0xFF))+150;
                TrasmitIndex++;
            }
            else if (TrasmitIndex == 1){
                transmitBuffer = ((unsigned char)((header.JoyStickMiddleX%100) & 0xFF))+150;
                TrasmitIndex++;
            }
            else {
                transmitBuffer = ((unsigned char)((header.JoyStickMiddleX/100) & 0xFF))+150;
                TrasmitIndex++;
            }
        }
      else if (whoWantsToTransmit == 4){ //file name
            if (TrasmitIndex == 0){
                transmitBuffer = NumFile+3;
                TrasmitIndex++;
            }
            else if (TrasmitIndex == -1){
                UARTIntVector &= ~UARTTransIE;
                LPM0_EXIT;
            }
            else{
                transmitBuffer = header.filenames[NumFile][TrasmitIndex-1];
                if (transmitBuffer == 0) TrasmitIndex = -1;
                else TrasmitIndex++;
            }
        }
      else if (whoWantsToTransmit == 5){ //Load Acknowledge
          if (TrasmitIndex == 1){
              UARTIntVector &= ~UARTTransIE;
              LPM0_EXIT;
          }
          else {
              transmitBuffer = 0x02;
              TrasmitIndex++;
          }
        }
      else if (whoWantsToTransmit == 6){ //Error from file execution
          if (TrasmitIndex == 1){
              UARTIntVector &= ~UARTTransIE;
              LPM0_EXIT;
          }
          else {
              transmitBuffer = 0x06;
              TrasmitIndex++;
          }
       }
      else if (whoWantsToTransmit == 7){ //currentPos
            if (TrasmitIndex == 2){
                UARTIntVector &= ~UARTTransIE;
                LPM0_EXIT;
            }
            else if (TrasmitIndex == 1){
                transmitBuffer = ((unsigned char)((currentPos%100) & 0xFF))+150;
                TrasmitIndex++;
            }
            else {
                transmitBuffer = ((unsigned char)((currentPos/100) & 0xFF))+150;
                TrasmitIndex++;
            }
       }
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
