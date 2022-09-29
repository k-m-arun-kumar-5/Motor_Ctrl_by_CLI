/**************************************************************************
   FILE          :    port.h
 
   PURPOSE       :    port header - define port and its pin assignment.
 
   AUTHOR        :    K.M. Arun Kumar alias Arunkumar Murugeswaran
 
  KNOWN BUGS     :
	
  NOTE           :    port header - user interface to external device, such as LED, Switch,
 	connection to other microcontroller via RS232, USB, etc. 
 To avoid this header file to be included more than once, conditional directive is used  
	
  CHANGE LOGS     :  
	   
 **************************************************************************/
#ifndef _PORT_H
#define _PORT_H

 
/* Pins 3.0(Rx) and 3.1 (Tx) are used by on chip UART of uC 8051. On Chip UART used for RS-232 interface.

/* -------- DC Motor Driver Interface --------- */
sbit MOTOR_1_DRIVER_IN1  = P2^0;
sbit MOTOR_1_DRIVER_IN2  = P2^1;
sbit MOTOR_1_DRIVER_EN1  = P2^2;

/* -------- LED Interface --------- */
sbit ERROR_LED                = P1^3;
sbit MOTOR_1_ON_LED           = P1^2;
sbit MOTOR_1_CLOCKWISE_LED    = P1^1;


#endif 

/*-------------------------------------------------------------------
  ---- END OF FILE -------------------------------------------------
-------------------------------------------------------------------*/
