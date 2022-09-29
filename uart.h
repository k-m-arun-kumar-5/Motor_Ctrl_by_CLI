/**************************************************************************
   FILE          :    uart.h
 
   PURPOSE       :    UART Header
 
   AUTHOR        :    K.M. Arun Kumar alias Arunkumar Murugeswaran
 
  KNOWN BUGS     :
	
  NOTE           :  
 
  CHANGE LOGS    :
	   
 **************************************************************************/
 /* to avoid this header file to be included more than once, conditional directive is used  */
#ifndef _UART_H
#define _UART_H
                            
/* -------------------- public prototype declaration --------------------------------------- */
void UART_Init(void);
void UART_Tx_Char(const char tx_char);
void UART_Tx_Str(const char *const tx_str);
char UART_Rx_Char();

#endif

/*-------------------------------------------------------------------
  ------------------------ END OF FILE ------------------------------
-------------------------------------------------------------------*/
