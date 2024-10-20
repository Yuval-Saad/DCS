#ifndef _halGPIO_H_
#define _halGPIO_H_

#include  "../header/bsp_msp430x2xx.h"    		// private library - BSP layer
#include  "../header/app.h"    		// private library - APP layer

// #define CHECKBUSY    1  // using this define, only if we want to read from LCD
#ifdef CHECKBUSY
    #define LCD_WAIT lcd_check_busy()
#else
    #define LCD_WAIT DelayMs(5)
#endif

/*----------------------------------------------------------
  CONFIG: change values according to your port pin selection
------------------------------------------------------------*/
#define LCD_EN(a)   (!a ? (P2OUT&=~0X20) : (P2OUT|=0X20)) // P1.5 is lcd enable pin
#define LCD_EN_DIR(a)   (!a ? (P2DIR&=~0X20) : (P2DIR|=0X20)) // P1.5 pin direction

#define LCD_RS(a)   (!a ? (P2OUT&=~0X40) : (P2OUT|=0X40)) // P1.6 is lcd RS pin
#define LCD_RS_DIR(a)   (!a ? (P2DIR&=~0X40) : (P2DIR|=0X40)) // P1.6 pin direction

#define LCD_RW(a)   (!a ? (P2OUT&=~0X80) : (P2OUT|=0X80)) // P1.7 is lcd RW pin
#define LCD_RW_DIR(a)   (!a ? (P2DIR&=~0X80) : (P2DIR|=0X80)) // P1.7 pin direction

#define LCD_DATA_OFFSET 0x04 //data pin selection offset for 4 bit mode, variable range is 0-4, default 0 - Px.0-3, no offset


/*---------------------------------------------------------
  END CONFIG
-----------------------------------------------------------*/
#define FOURBIT_MODE    0x0
#define EIGHTBIT_MODE   0x1
#define LCD_MODE        FOURBIT_MODE

#define OUTPUT_PIN      1
#define INPUT_PIN       0
#define OUTPUT_DATA     (LCD_MODE ? 0xFF : (0x0F << LCD_DATA_OFFSET))
#define INPUT_DATA      0x00

#define LCD_STROBE_READ(value)  LCD_EN(1), \
                asm("nop"), asm("nop"), \
                value=LCD_DATA_READ, \
                LCD_EN(0)


#define lcd_cursor(x)           lcd_cmd(((x)&0x7F)|0x80)
#define lcd_clear()             lcd_cmd(0x01)
#define lcd_putchar(x)          lcd_data(x)
#define lcd_goto(x)             lcd_cmd(0x80+(x))
#define lcd_cursor_right()      lcd_cmd(0x14)
#define lcd_cursor_left()       lcd_cmd(0x10)
#define lcd_display_shift()     lcd_cmd(0x1C)
#define lcd_home()              lcd_cmd(0x02)
#define cursor_off              lcd_cmd(0x0C)
#define cursor_on               lcd_cmd(0x0F)
#define lcd_function_set        lcd_cmd(0x3C) // 8bit,two lines,5x10 dots
#define lcd_new_line            lcd_cmd(0xC0)

/*old
#define segments_pointer        0x1000
*/
//new
#define files_segments_pointer        0x1000
#define header_segment_pointer        0xF000
//

//LCD Functions
extern void lcd_cmd(unsigned char);
extern void lcd_data(unsigned char);
extern void lcd_puts(const char * s);
extern void lcd_init();
extern void lcd_strobe();
extern void DelayMs(unsigned int);
extern void DelayUs(unsigned int);


extern enum FSMstate state;
extern struct FilesHeader header;
extern unsigned char recieveData;
extern unsigned char dataToSend[4];
extern unsigned char NumFile;
extern unsigned char PB_Pressed;
extern unsigned char d;
extern unsigned int currentPos;


extern __interrupt void PBs_handler(void);


extern void enterLPM(void);
extern void sysConfig(void);
extern void SampleJoyStick(unsigned int*,unsigned int*);
extern void StepMotorClockwise(unsigned char*);
extern void StepMotorCounterClockwise(unsigned char*);
extern void StartTimerA1(unsigned int);
extern void EnablePBs(void);
extern void StartPBs(void);
extern void DisablePBs(void);
extern void TransmitData(unsigned char);
extern void ReadSegment(char*, unsigned char);
extern void WriteSegment(char* , unsigned char);
extern void FlashConfig(void);

unsigned int convertMillisToTicks(unsigned int);
unsigned char CommandCode(unsigned char);
void ControlOp(unsigned char);
void delay(unsigned int);

#endif







