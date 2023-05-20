/**************************************************************************
   FILE          :    conf.h
 
   PURPOSE       :   main peripherial configuration Header
 
   AUTHOR        :    K.M. Arun Kumar alias Arunkumar Murugeswaran
 
  KNOWN BUGS     :
	
  NOTE           :   
  
  CHANGE LOGS    :
	   
 **************************************************************************/
 
 #ifndef _CONF_H
 #define _CONF_H
 
 /* -------------------------------- debug conf -----------------------------------------*/
#define TRACE                                   (1U)
#define TRACE_ERROR                             (2U)

 /* -------------------------------- OSC Freq conf -------------------------------------*/
 /* set crystal Oscillator(stable operation) frequency (in Hz) here we set 11.0592MHz to get precise baud rate */
#define OSC_FREQ            (11059200UL)

// Number of oscillations required to execute per instruction or increment a timer (12, etc)
// 12 – Original 8051 / 8052 and numerous modern versions
// 6 – Various Infineon and Philips devices, etc.
// 4 – Dallas 320, 520 etc.
// 1 – Dallas 420, etc.

#define OSC_PER_INST           (12)

/* -------------------------------- UART conf -----------------------------------------*/
#define TERMINATOR_CHAR         ('\r')
#define UART_EVENT              INTERRUPT

/* -------------------------------- Appl  -----------------------------------------*/
typedef enum {FSM_MOTOR_IN_OFF, FSM_MOTOR_IN_CLOCKWISE, FSM_MOTOR_IN_UNKNOWN_STATE,  
              NUM_FSM_MOTOR_STATES, CMD_MOTOR_TURN_OFF = 0, CMD_MOTOR_TURN_CLOCKWISE,
              CMD_MOTOR_STATUS } fsm_cmd_motor_states_t;

#define STR_MAX_NUM_CHARS       (18)
#define MOTOR_ID_NUM_CHARS      (4)
#define NUM_MOTORS              (2) 

#define SW_DEBOUNCE_TIMEOUT     (100UL)
#define SW_DEBOUNCE_COUNT       (200UL) 

#endif

/*------------------------------------------------------------------*-
  ------------------------ END OF FILE ------------------------------
-*------------------------------------------------------------------*/
