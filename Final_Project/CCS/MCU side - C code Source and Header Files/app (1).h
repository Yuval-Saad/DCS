#ifndef _app_H_
#define _app_H_

enum FSMstate{Sleep , Manual_Sleep , Joystick_Control , Rotate , Painter , Calibration_Sleep , Calibration_before_count , Calibration_count , Send_Script_Status , Script_Sleep , Load , Execute}; // States Enum
/* old
struct FilesHeader {
  unsigned char filesState; // each bit indicates a specific file - 1 Byte
  char filenames[3][17]; // 3 file name max len 18 each - 54 Byte total
  char* filePonters[4]; // 3 pointers to Strings (data) - 6 Byte Total
  unsigned char sizes[3]; // 3 sizes (cannot be more then 64 Bytes (1 segment) - 3 Bytes
}; //Total memory : exactly 64 Byte which is 1 segment
*/
//new
struct FilesHeader {
  unsigned char inFlash; //1 Byte
  unsigned char filesState; // each bit indicates a specific file - 1 Byte
  char filenames[3][17]; // 3 file name max len 18 each - 51 Byte total
  char* filePonters[3]; // 3 pointers to Strings (data) - 6 Byte Total
  unsigned char sizes[3]; // 3 sizes (cannot be more then 64 Bytes (1 segment) - 3 Bytes
  unsigned int s_numOfStepsIn360; //2 Bytes
  unsigned int JoyStickMiddleX; //2 Bytes
  unsigned int JoyStickMiddleY; //2 Bytes
}; //Total memory : exactly 68 Byte which is 1 segment
//
#endif
