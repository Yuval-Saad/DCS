#ifndef _api_H_
#define _api_H_

#include  "../header/halGPIO.h"     // private library - HAL layer

#define JoyStickMiddleTolerance     50
#define MotorNoiseTolerance         20


extern void F_Joystick_Control(void);
extern void F_Manual_Sleep(void);
extern void F_Rotate(void);
extern void F_Painter(void);
extern void F_Calibration_Sleep(void);
extern void F_Calibration_before_count(void);
extern void F_Calibration_count(void);
extern void F_Send_Script_Status(void);
extern void F_Script_Sleep(void);
extern void F_Load(void);
extern void F_Execute(void);
int CollectOperand(char*, int);
void ConvertIntToStr(char*, int);
void inc_lcd(char);
void dec_lcd(char);
void rra_lcd(char);
void set_delay(char);
void stepper_deg(char);
void stepper_scan(char, char);
int ExecuteOpcode(char*, int);
void printCurrentDegree(void);
unsigned int getPosFromDeg (char deg);
unsigned char findBestDirection(unsigned int, unsigned int);
extern void SendFileStatus(void);
void SampleJoyStickNoiseFilter (unsigned int*, unsigned int* , unsigned char ,unsigned char);
unsigned char calcDirection(unsigned int, unsigned int, unsigned int);
unsigned char JoyStickInMiddle(int, int);
unsigned int calcNewPos(int, int);
void PrepareCharToSend(unsigned int , unsigned int);
unsigned int eightCalcNewPos (unsigned int, unsigned int, unsigned int, int, int, unsigned char);
unsigned char ShouldNotMove(unsigned int, unsigned int );





#endif
