/* ********************************************************************
FILE                   : main.c

PROGRAM DESCRIPTION    : Program for the 8051 for a DC Motor control through DC Driver is given by Microcontroller.
                         DC Motor control command is entered in UART terminal. For DC Motor command to stop running
                         Enter in UART Terminal : "MOTOR 1 OFF" , for DC Motor command to trigger running in clockwise direction : "MOTOR 1 ON"                       									 
	 
AUTHOR                : K.M. Arun Kumar alias Arunkumar Murugeswaran
	 
KNOWN BUGS            : 

NOTE                  :  	   							
                                    
CHANGE LOGS           : 

*****************************************************************************/

#include "main.h"

typedef enum 
{
	MOTOR_ALREADY_OFF = NUM_ERR_STATUS, MOTOR_TURNED_OFF, MOTOR_ALREADY_RUN_CLOCKWISE, MOTOR_TURNED_CLOCKWISE, 
    ERR_MOTOR_UNKNOWN_STATE, ERR_MOTOR_ID_INVALID, ERR_CMD_MOTOR_TURN_INVALID 
} appl_status_err_t;
			  
typedef struct 
{
   unsigned int motor_id            : 3;
   unsigned int cur_fsm_motor_state : 3;  
   unsigned int                     : 2;
}  motor_data_t;   

static motor_data_t	motor_ids[NUM_MOTORS];		  
static char motor_id_clockwise_cmd_str[MOTOR_ID_NUM_CHARS + 3], motor_id_off_cmd_str[MOTOR_ID_NUM_CHARS + 4],
	motor_cmd_rcvd_str[STR_MAX_NUM_CHARS];
static const char motor_1_str[] = "MOTOR 1 ", motor_off_cmd_str[] = "OFF", motor_clockwise_cmd_str[] = "ON";	
static int motor_cmd_rcvd_str_num_chars = 0; 

static unsigned int UART_Motor_Menu(const unsigned char motor_id);
static unsigned int FSM_Motor_Proc(const unsigned char motor_id, const unsigned char motor_cmd);
static unsigned int FSM_Motor_Cmd_Proc(const unsigned char motor_id, const unsigned char motor_cmd);
static unsigned int DC_Motor_Init(const unsigned char cur_motor_id, const unsigned char init_fsm_motor_state);

/*------------------------------------------------------------*-
FUNCTION NAME  : main

DESCRIPTION    :          
								
INPUT          : none

OUTPUT         : 

NOTE           : 
-*------------------------------------------------------------*/

void main(void)
{
	 unsigned char ret_status;
	 char rx_char;      	 
	 
     /* ========= begin : USER CODE INITIALIZE ========== */
	 ERROR_LED = 0;         //output for MCU	 
	 UART_Init(); 
     DC_Motor_Init(0, FSM_MOTOR_IN_OFF); 	 
	 Str_Join(motor_id_off_cmd_str, motor_1_str, motor_off_cmd_str);
	 Str_Join(motor_id_clockwise_cmd_str, motor_1_str, motor_clockwise_cmd_str);	
	 FSM_Motor_Cmd_Proc(0, MOTOR_1_INIT_CMD);	 
	 	  
	 /* ========= end : USER CODE INITIALIZE ========== */
	   
     while(1) // Super Loop
     {
		 if((motor_cmd_rcvd_str_num_chars + 1) >= STR_MAX_NUM_CHARS )
		 {
			 ERROR_LED = STATE_ON;
			 return;
		 }
		 rx_char = UART_Rx_Char(); 		   
		 switch(rx_char)	
		 {
		     case BACKSPACE_CHAR:  
		         //backspace char received, delete previous received char
		         if(motor_cmd_rcvd_str_num_chars > 0)
			     {
			  	    UART_Tx_Char(rx_char);
				    motor_cmd_rcvd_str[motor_cmd_rcvd_str_num_chars] = NULL_CHAR;				
			         --motor_cmd_rcvd_str_num_chars;
				 }					
			 break;
             case TERMINATOR_CHAR:
			    UART_Tx_Char(rx_char);
			    motor_cmd_rcvd_str[motor_cmd_rcvd_str_num_chars] = NULL_CHAR;
			    UART_Tx_Str("Entered Motor command : ");
			    UART_Tx_Str(motor_cmd_rcvd_str);
			    UART_Tx_Char(NEXT_LINE_CHAR);
				if((ret_status = Str_Cmp(motor_1_str, motor_cmd_rcvd_str, 0, MOTOR_ID_NUM_CHARS )))
				{
				  	ret_status = ERR_MOTOR_ID_INVALID; 		
                    rx_char = NUM_MOTORS;
				}					
				else 
				{
					if((ret_status = Str_Cmp(motor_id_clockwise_cmd_str, motor_cmd_rcvd_str, MOTOR_ID_NUM_CHARS, (STR_MAX_NUM_CHARS - MOTOR_ID_NUM_CHARS))) == 0)
						 ret_status = CMD_MOTOR_TURN_CLOCKWISE; 					
				    else if((ret_status = Str_Cmp(motor_id_off_cmd_str, motor_cmd_rcvd_str, MOTOR_ID_NUM_CHARS, (STR_MAX_NUM_CHARS - MOTOR_ID_NUM_CHARS))) == 0 )				    
			      	     ret_status = CMD_MOTOR_TURN_OFF;  
					        else
							   ret_status = ERR_CMD_MOTOR_TURN_INVALID;
				    //get motor id from received motor cmd, since only on motor, so only one valid motor id = 0 	   
					rx_char = 0;	   
				}
				FSM_Motor_Cmd_Proc(rx_char, ret_status);									
		     break;			
             default:
			    UART_Tx_Char(rx_char);
			    motor_cmd_rcvd_str[motor_cmd_rcvd_str_num_chars] = rx_char;
			    ++motor_cmd_rcvd_str_num_chars;	
		   }						 
	 }	   
	 return; 
}

/*------------------------------------------------------------*
FUNCTION NAME  : UART_Motor_Menu

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        :   
-*------------------------------------------------------------*/
unsigned int UART_Motor_Menu(const unsigned char motor_id)
{
	motor_data_t *cur_motor_id_ptr; 
	unsigned int ret_status = SUCCESS;
	unsigned char cur_motor_id ;
	
	motor_cmd_rcvd_str_num_chars = 0;
    if(((ret_status = Str_Fill_Char(motor_cmd_rcvd_str, STR_MAX_NUM_CHARS, NULL_CHAR))) != SUCCESS)
   	   return FAILURE; 
   
    switch(motor_id)
	{
		case 0:
	       UART_Tx_Str(motor_1_str);
		   ret_status = SUCCESS;
		   cur_motor_id = motor_id;           		   
		break;
        default:
		   ret_status = ERR_MOTOR_ID_INVALID;	
		   cur_motor_id = 0;
	}
	for (; cur_motor_id < NUM_MOTORS ; ++cur_motor_id)
	{
		UART_Tx_Str(motor_1_str);
		cur_motor_id_ptr = &motor_ids[cur_motor_id];
	    switch(cur_motor_id_ptr->cur_fsm_motor_state)
        {
	         case FSM_MOTOR_IN_UNKNOWN_STATE:			  
	            UART_Tx_Str("in unknown state \r");
		        ret_status = ERR_MOTOR_UNKNOWN_STATE;
	         case FSM_MOTOR_IN_OFF:
	         case FSM_MOTOR_IN_CLOCKWISE: 
	            if(cur_motor_id_ptr->cur_fsm_motor_state == FSM_MOTOR_IN_OFF)
		   	          UART_Tx_Str("is in OFF state \r");			   
                else if (cur_motor_id_ptr->cur_fsm_motor_state == FSM_MOTOR_IN_CLOCKWISE)
                         UART_Tx_Str("is running in clockwise\r");
             break;   
		 }
		 if(motor_id < NUM_MOTORS)
	         break;
	}
	
	if(motor_id < NUM_MOTORS)
		cur_motor_id = motor_id;
	else
		cur_motor_id = 0;	
	
	UART_Tx_Str("\rMOTOR COMMAND MENU\r");
	for (; cur_motor_id < NUM_MOTORS ; ++cur_motor_id)
	{
		cur_motor_id_ptr = &motor_ids[cur_motor_id];
	    switch(cur_motor_id_ptr->cur_fsm_motor_state)
        {
	         case FSM_MOTOR_IN_UNKNOWN_STATE:	            
	         case FSM_MOTOR_IN_OFF:
	         case FSM_MOTOR_IN_CLOCKWISE:
			      UART_Tx_Str(motor_1_str); 
	             if(cur_motor_id_ptr->cur_fsm_motor_state != FSM_MOTOR_IN_OFF)
		         {
			          UART_Tx_Str("to stop running : ");
			          UART_Tx_Str(motor_id_off_cmd_str);
					  UART_Tx_Char(NEXT_LINE_CHAR);
		         } 
                 if(cur_motor_id_ptr->cur_fsm_motor_state != FSM_MOTOR_IN_CLOCKWISE)	
		         {			  
		              UART_Tx_Str("to run clockwise : ");
		              UART_Tx_Str(motor_id_clockwise_cmd_str);	
					  UART_Tx_Char(NEXT_LINE_CHAR);
		         }		      	  
             break;   
		 }
		 if(motor_id < NUM_MOTORS)
	         break;
	}		  
    UART_Tx_Str("\rINPUT: Motor command, Enter : ");  
    return ret_status; 
}

/*------------------------------------------------------------*
FUNCTION NAME  : FSM_Motor_Proc

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        :   
-*------------------------------------------------------------*/
unsigned int FSM_Motor_Proc(const unsigned char motor_id, const unsigned char motor_cmd)
{
	motor_data_t *cur_motor_id_ptr; 
	const char already_str[] = "already in ";	
	unsigned char cur_motor_id = motor_id;
	unsigned int ret_status = SUCCESS;
	
	switch(motor_id)
	{
		case 0:
	       UART_Tx_Str(motor_1_str);
		   break;
        default:
		    UART_Tx_Str("ERR: Invalid Motor ID");
			cur_motor_id = NUM_MOTORS;
            ret_status = ERR_MOTOR_ID_INVALID;
	}	
	if(ret_status == SUCCESS)
	{
         switch(motor_cmd)
	     {
		     case CMD_MOTOR_TURN_OFF:
		     case CMD_MOTOR_TURN_CLOCKWISE:
		     break;
		     default:
		        UART_Tx_Str(motor_1_str);
		        UART_Tx_Str(": Invalid motor command");
                ret_status = ERR_CMD_MOTOR_TURN_INVALID;
		 }				
	}	
    if(ret_status == SUCCESS)	
    {		
        cur_motor_id_ptr = &motor_ids[motor_id]; 
     	switch(cur_motor_id_ptr->cur_fsm_motor_state)
    	{
	    	case FSM_MOTOR_IN_OFF:
		        switch(motor_cmd)
		        {
		            case CMD_MOTOR_TURN_OFF: 
				      UART_Tx_Str(already_str); 
		         	 // UART_Tx_Str(motor_off_cmd_str); 
					  UART_Tx_Str("OFF");
				      ret_status = MOTOR_ALREADY_OFF; 
                    break;					  
		        }			   
		    break;
		    case FSM_MOTOR_IN_CLOCKWISE:
		        switch(motor_cmd)
			    {
		            case CMD_MOTOR_TURN_CLOCKWISE:
				      UART_Tx_Str(already_str); 
					  UART_Tx_Str(motor_clockwise_cmd_str); 	
				      ret_status = MOTOR_ALREADY_RUN_CLOCKWISE; 	
                    break;					  
		       }		  
		    break;
		    default:
		       cur_motor_id_ptr->cur_fsm_motor_state = FSM_MOTOR_IN_UNKNOWN_STATE;
               UART_Tx_Str("in unknown state\r"); 
               UART_Tx_Str(motor_1_str);		   
		}			   
	}
	if(ret_status == SUCCESS)
	{
	    UART_Tx_Str("triggered ");
	    switch(motor_cmd)
	    {
			 case CMD_MOTOR_TURN_OFF:
		        if(cur_motor_id_ptr->cur_fsm_motor_state != FSM_MOTOR_IN_OFF)
		        {					
					MOTOR_1_DRIVER_EN1 = STATE_OFF;
					MOTOR_1_DRIVER_IN1 = STATE_OFF;
					MOTOR_1_DRIVER_IN2 = STATE_OFF;
					MOTOR_1_ON_LED = STATE_OFF;
					MOTOR_1_CLOCKWISE_LED = STATE_OFF;
			        cur_motor_id_ptr->cur_fsm_motor_state = FSM_MOTOR_IN_OFF;   
         	        // UART_Tx_Str(motor_off_cmd_str);	
                    UART_Tx_Str("OFF"); 				   
                    ret_status = MOTOR_TURNED_OFF;
		        }
	         break;
	      	 case CMD_MOTOR_TURN_CLOCKWISE:
		        if(cur_motor_id_ptr->cur_fsm_motor_state != FSM_MOTOR_IN_CLOCKWISE)
		        {
					MOTOR_1_DRIVER_EN1 = STATE_ON;
					MOTOR_1_DRIVER_IN1 = STATE_ON;
					MOTOR_1_DRIVER_IN2 = STATE_OFF;
					MOTOR_1_ON_LED = STATE_ON;
					MOTOR_1_CLOCKWISE_LED = STATE_ON;					
                    cur_motor_id_ptr->cur_fsm_motor_state = FSM_MOTOR_IN_CLOCKWISE;             
			        UART_Tx_Str(motor_clockwise_cmd_str);	
                    ret_status = MOTOR_TURNED_CLOCKWISE;
		        }
		    break; 			
            default:
               ;    			
	    }
	}
	UART_Tx_Char(NEXT_LINE_CHAR);
	if((UART_Motor_Menu(cur_motor_id)) != SUCCESS)
		ret_status = FAILURE;
		  
	return ret_status; 
}

/*------------------------------------------------------------*
FUNCTION NAME  : FSM_Motor_Cmd_Proc

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        :   
-*------------------------------------------------------------*/
unsigned int FSM_Motor_Cmd_Proc(const unsigned char motor_id, const unsigned char motor_cmd)
{
	unsigned int ret_status = SUCCESS;
	
    ret_status = FSM_Motor_Proc(motor_id, motor_cmd); 
	switch(ret_status)
	{
	     case MOTOR_ALREADY_OFF:
	     case MOTOR_TURNED_OFF:
		 case MOTOR_ALREADY_RUN_CLOCKWISE:
	     case MOTOR_TURNED_CLOCKWISE:
	     case SUCCESS:
            ERROR_LED = STATE_OFF;
         break;
         default:
            ERROR_LED = STATE_ON;
	}
	
	return ret_status; 
} 
	
/*------------------------------------------------------------*
FUNCTION NAME  : DC_Motor_Init

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        :   
-*------------------------------------------------------------*/
unsigned int DC_Motor_Init(const unsigned char cur_motor_id, const unsigned char init_fsm_motor_state)
{
	if(cur_motor_id >= NUM_MOTORS || init_fsm_motor_state >= NUM_FSM_MOTOR_STATES)
	{
		UART_Tx_Str("ERR: Motor Init \r");
		return ERR_DATA_NOT_IN_RANGE; 
	}
	motor_ids[cur_motor_id].motor_id = cur_motor_id;
	motor_ids[cur_motor_id].cur_fsm_motor_state = init_fsm_motor_state; 
	switch(cur_motor_id)
	{
		case 0:
		   MOTOR_1_DRIVER_IN1 = 0;
	       MOTOR_1_DRIVER_IN2 = 0;
	       MOTOR_1_DRIVER_EN1 = 0;
	       MOTOR_1_ON_LED = 0;
	       MOTOR_1_CLOCKWISE_LED = 0;	       
		break;
	}
	return SUCCESS; 
}

/*------------------------------------------------------------*
FUNCTION NAME  : Delay_in_Micro_Sec

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        :   
-*------------------------------------------------------------*/
void Delay_in_Micro_Sec(const unsigned long num_micro_seconds)
{
	unsigned int i = 0;
	
	for (i = 0;  i < num_micro_seconds; ++i);
	return; 	
}

/*------------------------------------------------------------*
FUNCTION NAME  : Delay_in_Milli_Sec

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        :   
-*------------------------------------------------------------*/
void Delay_in_Milli_Sec(const unsigned long num_milli_seconds)
{
	unsigned int i = 0;
	
	for (i = 0;  i < num_milli_seconds; ++i)
	  Delay_in_Micro_Sec(500UL);
	return;	
}


/*------------------------------------------------------------*
FUNCTION NAME  : Str_Fill_Char

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        :    
-*------------------------------------------------------------*/
unsigned int Str_Fill_Char(char *const dest_str, const unsigned char num_chars, const char fill_char)
{
	unsigned char index;
	
	if(dest_str == NULL_PTR || num_chars > STR_MAX_NUM_CHARS)
	{
		UART_Tx_Str("ERR: Str_Fill\r");
		return ERR_INVALID_DATA;
	}
	for (index = 0 ; index < num_chars ; ++index)
	   dest_str[index] = fill_char;
    dest_str[index] = NULL_CHAR;
	
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Str_Cmp

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        :    
-*------------------------------------------------------------*/
unsigned int Str_Cmp(const char *const left_str, const char *const right_str, const unsigned char start_char_pos, const unsigned char num_chars_str_cmp)
{
	unsigned int index;
	
	if(left_str == NULL_PTR || right_str == NULL_PTR || (start_char_pos + num_chars_str_cmp) > STR_MAX_NUM_CHARS)
	{
		UART_Tx_Str("ERR: Str_Cmp\r");
		return (STR_MAX_NUM_CHARS + 1);
	}	
	for (index = 0 ; index < num_chars_str_cmp ; ++index)
	{
		if((*(left_str + index + start_char_pos)) != (*(right_str + index + start_char_pos)))
		{
			/*#ifdef TRACE 
		      UART_Tx_Char(right_str[index + start_char_pos]);
			  UART_Tx_Char(NEXT_LINE_CHAR);
		    #endif */
			return (index + start_char_pos + 1); 
		}
		if ((*(right_str + index + start_char_pos)) == NULL_CHAR)
		   return 0 ;  
	}	
	 return 0 ;  
}

/*------------------------------------------------------------*
FUNCTION NAME  : Str_Join

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        :    
-*------------------------------------------------------------*/
unsigned int Str_Join(char *const str_join_str, const char *const left_src_str, const char *const right_src_str)
{
	unsigned char str_join_str_index, src_str_index;
	
	if(str_join_str == NULL_PTR || left_src_str == NULL_PTR || right_src_str == NULL_PTR)
	{
		UART_Tx_Str("ERR: Str_Join Ptr\r");
		return ERR_NULL_PTR;
	}
	
    for(str_join_str_index = 0 ; ((*(left_src_str + str_join_str_index)) != NULL_CHAR); ++str_join_str_index)
	{
		if(str_join_str_index + 1 >= STR_MAX_NUM_CHARS)
		{
			UART_Tx_Str("ERR: Str_Join left Str\r");
			return ERR_DATA_NOT_IN_RANGE;
		}
		*(str_join_str + str_join_str_index) = *(left_src_str + str_join_str_index);
	}
     
    for (src_str_index = 0; (*((right_src_str + src_str_index)) != NULL_CHAR); ++src_str_index, ++str_join_str_index) 
	{
		if(str_join_str_index + 1 >= STR_MAX_NUM_CHARS)
		{
			UART_Tx_Str("ERR: Str_Join right Str\r");
			return ERR_DATA_NOT_IN_RANGE;
		}
        *(str_join_str + str_join_str_index) = *(right_src_str + src_str_index); 
	}
    *(str_join_str + str_join_str_index) = NULL_CHAR;
	
	 /* #ifdef TRACE
	  UART_Tx_Str(str_join_str);
	  UART_Tx_Char(NEXT_LINE_CHAR);
	#endif  */
	
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Str_Cat

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        :    
-*------------------------------------------------------------*/
unsigned int Str_Cat(char *const str_cat_str, const char *const src_str)
{
	unsigned char str_cat_str_index = 0, src_str_index;
	
	if(str_cat_str == NULL_PTR || src_str == NULL_PTR)
	{
		UART_Tx_Str("ERR: Str_Cat Ptr\r");
		return ERR_NULL_PTR;
	}	
    while (((*(str_cat_str + str_cat_str_index)) != NULL_CHAR) )
	{
		if(str_cat_str_index + 1 >= STR_MAX_NUM_CHARS)
			return ERR_DATA_NOT_IN_RANGE;
        ++str_cat_str_index;
	} 
   // concatenate str_cat_str to src_str
    for (src_str_index = 0; ((*(src_str + src_str_index)) != NULL_CHAR); ++src_str_index, ++str_cat_str_index) 
	{
		if(str_cat_str_index + 1 >= STR_MAX_NUM_CHARS)
		{
			UART_Tx_Str("ERR: Str_Cat chars\r");
			return ERR_DATA_NOT_IN_RANGE;
		}
        *(str_cat_str + str_cat_str_index) = *(src_str + src_str_index);  
	}
    *(str_cat_str + str_cat_str_index) = NULL_CHAR;
	
	return SUCCESS;
}

/*------------------------------------------------------------------*-
  ---- END OF FILE -------------------------------------------------
-*------------------------------------------------------------------*/	 
