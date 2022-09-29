/* ********************************************************************
FILE                 : uart.c

PURPOSE              : 
	 
AUTHOR               : K.M. Arun Kumar alias Arunkumar Murugeswaran
	 
KNOWN BUGS           : 

NOTE                 : 								

CHANGE LOGS          :

*****************************************************************************/

#include "main.h"

// ------  data type declarations ---------------------------


// ------  function prototypes ------------------------------


// ------  constants ----------------------------------------



// ------  variables ----------------------------------------



/*------------------------------------------------------------*-
FUNCTION NAME  : UART_Init

DESCRIPTION    : Initialize UART port						 

INPUT          : 

OUTPUT         : 

NOTE           : 
-*------------------------------------------------------------*/
void UART_Init(void)
{
  	SCON = 0x50;    // setup serial port control 
  	TMOD = 0x20;    // hardware (9600 BAUD @11.0592MHZ) 
  	TH1  = 0xFD;    // TH1
  	TR1	 = 1;  		// Timer 1 on
	
	Delay_in_Milli_Sec(10);
	return;
}

/*------------------------------------------------------------*-
FUNCTION NAME  : UART_Tx_Char

DESCRIPTION    : Transmit Char by polling 				 

INPUT          : 

OUTPUT         : 

NOTE           : 
-*------------------------------------------------------------*/
void UART_Tx_Char(const char tx_char)
{
	SBUF = tx_char;
	while (TI==0); //Transmit Char by polling 
	TI = 0;
	return; 
}

/*------------------------------------------------------------*-
FUNCTION NAME  : UART_Tx_Str

DESCRIPTION    : Transmit String by polling 				 

INPUT          : 

OUTPUT         : 

NOTE           : 
-*------------------------------------------------------------*/
void UART_Tx_Str(const char *const tx_str)
{
     unsigned char index;
	 
     for(index = 0; ((*(tx_str + index)) != NULL_CHAR); index++)
	     UART_Tx_Char(*(tx_str + index));      
	 return; 
}

/*------------------------------------------------------------*-
FUNCTION NAME  : UART_Rx_Char

DESCRIPTION    : Receive Char by polling 				 

INPUT          : 

OUTPUT         : 

NOTE           : 
-*------------------------------------------------------------*/
char UART_Rx_Char()
{
	char rx_char;
	
	while (RI==0);
	RI = 0;
	rx_char = SBUF;
	return(rx_char);
}

/*-------------------------------------------------------------------
  ------------------------ END OF FILE ------------------------------
-------------------------------------------------------------------*/

