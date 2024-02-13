Description
===========
Motor Power Control using AT89C52 Microcontroller using CLI Command with UART interface. Microcontroller controls power supply for a DC Motors  through DC Motor Driver (L293D). There are 2 DC motors MOTOR 1 and MOTOR 2. DC Motor control command is entered in UART terminal. For DC Motor 1 command to stop running, enter in UART Terminal, CLI Command : "MOTOR 1 OFF" . For DC Motor 1 command to trigger running in clockwise direction, CLI Command : "MOTOR 1 ON". To know the DC MOTOR 1 current state, CLI Command : "MOTOR 1 STATUS". For trigger MOTOR 1 and MOTOR 2 or know its status, CLI Command: "MOTOR 3 ON" or "MOTOR 3 STATUS". MOTOR 3 indicates all DC motors ( ie in our case,  MOTOR 1 & MOTOR 2) in CLI command. ERR_RESET_SW is used to reset error condition when motor command input exceeds max chars. EMER_SW is used in
emergency condition to stop running all MOTORS. 	

CAUTION:
=========
Schematics and Simulated is done by using Proteus CAD. NOT EXPERIMENTED IN REAL TIME ENVIRONMENT.  

Purpose :
=========
In all my respective repositories, I, K.M. Arun Kumar alias Arunkumar Murugeswaran, just shared my works that I worked as the learning path and practiced, with designed, developed, implemented, simulated and tested, including some projects, assignments, documentations and all other related files and some programming that might not being implement, not being completed, lacks some features or have some bugs. Purpose of all my repositories, if used, can be used for EDUCATIONAL PURPOSE ONLY. It can be used as the open source and freeware. Kindly read the LICENSE.txt for license, terms and conditions about the use of source codes, binaries, documentation and all other files, located in all my repositories. 

