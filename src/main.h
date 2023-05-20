/**************************************************************************
   FILE          :    main.h
 
   PURPOSE       :    project header.  
 
   AUTHOR        :   K.M. Arun Kumar alias Arunkumar Murugeswaran
 
  KNOWN BUGS     :
	
  NOTE           :   PROJECT header- groups the key information about the 8051 device you have used, along with other key
parameters � such as the oscillator frequency and commonly used information such as common data types in the project
	
  CHANGE LOGS     :
	   
 **************************************************************************/
 
 /* to avoid this header file to be included more than once, conditional directive is used  */
#ifndef _MAIN_H
#define _MAIN_H

/* Must include the appropriate microcontroller header file here eg for eg we use 8051/8052 uC device.
 In most case, microcontroller header is also a device header.in our case, reg52.h
 device header will include the addresses of the special function registers (SFRs) used for port access, plus similar
 details for other on-chip components such as analog-to-digital converters*/
 /* reg52.h is a system header and for <> enclosed one, preprocessor will search the reg52.h in predetermined directory path to locate the header file. */
 // Must include the appropriate microcontroller header file here
#include <reg52.h> 
#include "constants.h"
#include "port.h"
#include "conf.h"
#include "uart.h"
 
void Delay_in_Micro_Sec(const unsigned long num_micro_seconds);
void Delay_in_Milli_Sec(const unsigned long num_milli_seconds);
unsigned int Str_Fill_Char(char *const dest_str, const unsigned char num_chars, const char fill_char);
unsigned int Str_Cmp(const char *const left_str, const char *const right_str, const unsigned char start_char_pos, const unsigned char num_chars);
unsigned int Str_Join(char *const str_join_str, const char *const left_src_str, const char *const right_src_str); 

#endif

/*-------------------------------------------------------------------
  ------------------------ END OF FILE ------------------------------
-------------------------------------------------------------------*/
