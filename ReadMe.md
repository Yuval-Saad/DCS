# Stepper Motor Control System with Joystick & Remote Control

## Project Overview
This project is the final task for the Digital Computer Structures course. It involves designing and implementing a stepper motor control system that can be operated manually using a joystick and remotely through a personal computer via serial communication.

The system is built using the MSP430G2553 microcontroller, coded in C using Code Composer Studio (CCS). The project integrates multiple hardware and software components, utilizing a Finite State Machine (FSM) for smooth control.

## Features
- *Manual Control*: Operate the stepper motor using a joystick to adjust its angle in real-time.
- *Remote Control*: The motor can also be controlled from a personal computer using a graphical user interface (GUI) over UART.
- *Real-time Painter App*: A joystick-controlled painter application that allows switching between writing, erasing, and neutral modes.
- *Motor Calibration*: Calibrate the stepper motor to calculate the number of steps and the angle per step.
- *Script Mode*: Execute pre-defined commands stored in the microcontroller’s FLASH memory.

## Technologies Used
- *MSP430G2553 Microcontroller*: The core hardware used for the motor control.
- *Code Composer Studio (CCS)*: The development environment for programming the MSP430 in C.
- *PySimpleGUI*: The library used for building the GUI on the computer.
- *UART Communication*: Allows seamless communication between the microcontroller and the PC.

## Installation and Setup
1. Clone this repository:
    bash
    git clone https://github.com/yourusername/stepper-motor-control.git
    
2. Open the project in Code Composer Studio (CCS).
3. Build and upload the project to the MSP430G2553 microcontroller.
4. Install the required Python dependencies:
    bash
    pip install PySimpleGUI
    
5. Run the GUI application:
    bash
    python gui.py
    

## Usage
- *Manual Mode*: Use the joystick to control the motor’s movements.
- *Remote Mode*: Send commands to the microcontroller using the GUI interface on the PC.
- *Painter Mode*: Draw or erase on the screen using the joystick.
- *Script Mode*: Upload and execute predefined scripts from the PC to the microcontroller.

## Authors
- Yonatan Arama
- Yuval Yaacov Said