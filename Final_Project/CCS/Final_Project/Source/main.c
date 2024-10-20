#include  "../header/api.h"    		// private library - API layer
#include  "../header/app.h"    		// private library - APP layer

enum FSMstate state; //Global Variable State type enum

struct FilesHeader header;
unsigned char d;
unsigned int currentPos;



void main(void){
    state = Sleep;  // start in "Sleep" state on RESET
    /* old
    header.filePonters[0] = (char*)(segments_pointer + 0x40);
    header.filePonters[1] = (char*)(segments_pointer + 0x80);
    header.filePonters[2] = (char*)(segments_pointer + 0xC0);
    header.filePonters[3] = (char*)segments_pointer;
    */
    //new
    header.inFlash = 'E';
    header.filePonters[0] = (char*)(files_segments_pointer);
    header.filePonters[1] = (char*)(files_segments_pointer + 0x40);
    header.filePonters[2] = (char*)(files_segments_pointer + 0x80);
    header.s_numOfStepsIn360 = 2043;
    header.JoyStickMiddleX = 472;
    header.JoyStickMiddleY = 473;
    //
    d = 50;
    currentPos = 0;

    sysConfig();
    lcd_init();
    lcd_clear();
    FlashConfig();

  while(1){
    switch(state){

      case Sleep:
          enterLPM();
          break;

      case Manual_Sleep:
          F_Manual_Sleep();
          break;

      case Joystick_Control:
          F_Joystick_Control();
          break;

      case Rotate:
          F_Rotate();
          break;

      case Painter:
          F_Painter();
          break;

      case Calibration_Sleep:
          F_Calibration_Sleep();
          break;

      case Calibration_before_count:
          F_Calibration_before_count();
          break;

      case Calibration_count:
          F_Calibration_count();
          break;

      case Send_Script_Status:
          F_Send_Script_Status();
          break;

      case Script_Sleep:
          F_Script_Sleep();
          break;

      case Load:
          F_Load();
          break;

      case Execute:
          F_Execute();
          break;
    }
  }
}
