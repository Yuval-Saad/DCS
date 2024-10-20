# Stepper Motor Control System with Joystick & Remote Control

## Project Overview
This project is the final task for the Digital Computer Structures course, aimed at developing a stepper motor control system that can be operated manually via a joystick and remotely through a personal computer using serial communication.

The project was implemented on an MSP430G2553 microcontroller using the C programming language in CCS. The system features a GUI for remote control, utilizing FSM (Finite State Machine) architecture, with both hardware and software components to execute a set of defined commands.

## Features
- *Manual Control via Joystick*: The joystick controls the stepper motor’s position, rotating in accordance with the joystick's movements.
- *Remote Control via GUI*: Control the motor from a computer using a simple, user-friendly interface. The GUI communicates with the microcontroller via UART, allowing remote command execution.
- *Real-time Painter App*: A drawing tool controlled by the joystick, where the user can switch between drawing, erasing, and neutral modes.
- *Stepper Motor Calibration*: Calibrate the stepper motor and display the number of steps per revolution on the computer screen.
- *Script Mode*: Execute predefined high-level commands from a script, stored in the MSP430’s FLASH memory.

## Technologies Used
- *MSP430G2553 Microcontroller*: The primary hardware controller.
- *CCS (Code Composer Studio)*: The development environment used for programming the microcontroller in C.
- *PySimpleGUI*: The library used for building the GUI interface.
- *UART Communication*: For communication between the microcontroller and the computer.

## Installation and Setup
1. Clone this repository:
    bash
    git clone https://github.com/yourusername/stepper-motor-control.git
    
2. Open the project in CCS.
3. Build and flash the project to your MSP430G2553 microcontroller.
4. Install the required Python dependencies:
    bash
    pip install PySimpleGUI
    
5. Run the GUI application from the gui directory:
    bash
    python gui.py
    

## Usage
- *Manual Mode*: Use the joystick to manually control the motor's movements.
- *Remote Mode*: Use the GUI on your computer to send commands to the microcontroller and control the motor remotely.
- *Painter*: Use the joystick to draw on the screen, switching between different modes such as writing and erasing.
- *Script Mode*: Load a script into the microcontroller’s FLASH memory and execute it.

## Authors
- Yonatan Arama
- Yuval Yaacov Said