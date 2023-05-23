Description
===========
Embedded C Program for Motor Power Control using AT89C52 Microcontroller using CLI Command with UART interface. Microcontroller controls power supply for a DC Motors  through DC Motor Driver (L293D). There are 2 DC motors MOTOR 1 and MOTOR 2. DC Motor control command is entered in UART terminal. For DC Motor 1 command to stop running, enter in UART Terminal, CLI Command : "MOTOR 1 OFF" . For DC Motor 1 command to trigger running in clockwise direction, CLI Command : "MOTOR 1 ON". To know the DC MOTOR 1 current state, CLI Command : "MOTOR 1 STATUS". For trigger MOTOR 1 and MOTOR 2 or know its status, CLI Command: "MOTOR 3 ON" or "MOTOR 3 STATUS". MOTOR 3 indicates all DC motors ( ie in our case,  MOTOR 1 & MOTOR 2) in CLI command. ERR_RESET_SW is used to reset error condition when motor command input exceeds max chars. EMER_SW is used in
emergency condition to stop running all MOTORS. 	

CAUTION:
=========
Schematics and Simulated is done by using Proteus CAD. NOT EXPERIMENTED IN REAL TIME ENVIRONMENT.  

Purpose :
=========
In all my respective repositories, I, K.M. Arun Kumar alias Arunkumar Murugeswaran, just shared my works that I worked as the learning path and practiced, with designed, developed, implemented, simulated and tested, including some projects, assignments, documentations and all other related files and some programming that might not being implement, not being completed, lacks some features or have some bugs. Purpose of all my repositories, if used, can be used for LEARNING and EDUCATIONAL PURPOSE ONLY. It can be used as the open source and freeware. Kindly read the LICENSE.txt for license, terms and conditions about the use of source codes, binaries, documentation and all other files, located in all my repositories. 

My Thanks and Tribute :
========================
I, thank to my family, Friends, Teachers, People behind the toolchains and references that I used, all those who directly or indirectly supported me and/or helped me and/or my family, Nature and God. My tribute to my family, Friends, Teachers, People behind the toolchains and references that I used, Nature, Jimmy Dog, God and all those, who directly or indirectly help and/or support me and/or my family.

Toolchains that I used for AT89C52 Application design and development are as follows :
======================================================================================
1: IDE and Compiler for Application development of AT89C52            - Keil uvision 4 v9.60a for C51 Development Tools with C51 Compiler.
2: CAD                                                                - Proteus 8.11. 
3: Desktop Computer Architecture and OS for development of AT89C51    - Intel X64 & Windows 10 (64 bit).
4: Code editor                                                        - Notepad++.
5: Documentation                                                      - Microsoft Office 2007 (alternative LibreOffice) and Text Editor.
6: File Archiver                                                      - 7-Zip.              

Some reference that I refered for AT89C51 Application design and development, are as follows :
==============================================================================================
1: Schaum's Outline of Programming with C, 2nd Edition - Book authored by Byron Gottfried.
2: Understanding and Using C Pointers: Core Techniques for Memory Management - Book authored by Richard M. Reese. 
3: Embedded C - Book authored by Michael J. Pont.
4: AT89C51 product datasheet
5: https://www.electronicwings.com/8051/inside  - 8051


Note :
======
Kindly read in the source codes, if mentioned, about the Program Description or Purpose, Known Bugs, Caution & Notes and documentations. 

My Repositories Web Link :
==========================
https://github.com/k-m-arun-kumar-5
