/* ********************************************************************
FILE                   : main.c

PROGRAM DESCRIPTION    : Program for the 8051 for a DC Motor control through DC Driver is given by Microcontroller. 
                         There are 2 DC motors MOTOR 1 and MOTOR 2. 
                         DC Motor control command is entered in UART terminal. 
						 For DC Motor 1 command to stop running, enter in UART Terminal, CLI Command : "MOTOR 1 OFF" , 
						 For DC Motor 1 command to trigger running in clockwise direction, CLI Command : "MOTOR 1 ON",
                         To know the DC MOTOR 1 current state, CLI Command : "MOTOR 1 STATUS" 
						 For trigger MOTOR 1 and MOTOR 2 or know its status, CLI Command: MOTOR 3 ON.
						 or MOTOR 3 STATUS. MOTOR 3 indicates all DC motors ( ie in our case,  MOTOR 1 & MOTOR 2) in CLI command. 
						 ERR_RESET_SW is used to reset error condition when motor command input exceeds max chars. EMER_SW is used in
             emergency condition to stop running all MOTORS. 						 
	 
AUTHOR                : K.M. Arun Kumar alias Arunkumar Murugeswaran
	 
KNOWN BUGS            : 

NOTE                  :  	   							
                                    
CHANGE LOGS           : 

*****************************************************************************/

#include "main.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum 
{
	MOTOR_ALREADY_OFF = NUM_ERR_STATUS, MOTOR_TURNED_OFF, MOTOR_ALREADY_RUN_CLOCKWISE, MOTOR_TURNED_CLOCKWISE, 
	ERR_MOTOR_UNKNOWN_STATE, ERR_MOTOR_ID_INVALID, ERR_CMD_MOTOR_TURN_INVALID, FSM_RCVD_CMD_CHAR = 0, FSM_PROC_RCVD_INPUT_CMD,
  FSM_EXCEEDS_INPUT_CHARS, FSM_EMERGENCY_MOTORS_OFF	
} appl_status_err_t;
			  
typedef struct 
{
   unsigned int motor_id            : 3;
   unsigned int cur_fsm_motor_state : 3;  
   unsigned int init_state          : 2;
}  motor_data_t;   

typedef struct
{
	unsigned int error_reset_sw_enable_flag : 1;
    unsigned int emergency_stop_enable_flag : 1; 	
	unsigned int emergency_stop_occured_flag : 1;
	unsigned int fsm_system                  : 5;
} ctrl_status_system_t;


static ctrl_status_system_t  ctrl_status_system;  
static motor_data_t	motor_ids[NUM_MOTORS];		  
static char *motor_cmd_rcvd_str;
static const char motor_str[] = "MOTOR", motor_off_cmd_str[] = "OFF", motor_clockwise_cmd_str[] = "ON", motor_status_cmd_str[] = "STATUS";	
static unsigned char motor_cmd_rcvd_str_num_chars = 0; 
static char motor_id_str[MOTOR_ID_NUM_CHARS + 1];

void Ext_int_Init();
static unsigned char UART_Motor_Menu(const unsigned int motor_id);
static unsigned char FSM_Motor_Proc(const unsigned int motor_id, const unsigned char motor_cmd);
static unsigned char DC_Motor_Init(const unsigned int cur_motor_id, const unsigned char init_fsm_motor_state);
static unsigned char DC_Motor_Cmd_Off(const unsigned int motor_id);
static unsigned char DC_Motor_Cmd_Clockwise(const unsigned int motor_id);
/*------------------------------------------------------------*-
FUNCTION NAME  : main

DESCRIPTION    :          
								
INPUT          : none

OUTPUT         : 

NOTE           : 
-*------------------------------------------------------------*/

void main(void)
{
	 char *token_str;
	 unsigned char ret_status, num_token;
	 unsigned long int sw_timeout = SW_DEBOUNCE_COUNT;
	 unsigned int motor_id, cur_motor_id;
	 
     /* ========= begin : USER CODE INITIALIZE ========== */
	 ERROR_LED = 0;         //output for MCU
	 ERR_RESET_SW = 1;
	 ERROR_LED = 0; 
	 EA  = 1;	// Enable global interrupt 
	 UART_Init(); 
	 Ext_int_Init();
	 ctrl_status_system.emergency_stop_enable_flag = STATE_ON;
	 ctrl_status_system.error_reset_sw_enable_flag = STATE_OFF;
     DC_Motor_Init(0, FSM_MOTOR_IN_OFF);
 	 DC_Motor_Init(1, FSM_MOTOR_IN_OFF);   	
     UART_Tx_Str("\rINPUT: Motor Commmand - ENTER : ");
	 ctrl_status_system.fsm_system = FSM_RCVD_CMD_CHAR;
     /* ========= end : USER CODE INITIALIZE ========== */
	  
     while(1) // Super Loop
     {
		 switch(ctrl_status_system.fsm_system)
		 {
			 case FSM_RCVD_CMD_CHAR:
			  if(uart_data.rcvd_terminator_char_flag == STATE_NO)
				{
					if(uart_data.rcvd_overflow_flag == STATE_YES)
					{
						ctrl_status_system.error_reset_sw_enable_flag = STATE_ON;
                        ctrl_status_system.fsm_system = FSM_EXCEEDS_INPUT_CHARS;	
                        ERROR_LED = STATE_ON;
				        UART_Tx_Str("\r ERR: Input chars > MAX Chars");
				        UART_Tx_Str("\r REQ: Press ERROR RESET SW \r");	
					}
				}
				else
				{
					UART_Read(&motor_cmd_rcvd_str, &motor_cmd_rcvd_str_num_chars);
					UART_Tx_Str("Entered Motor command : ");
			        UART_Tx_Str(motor_cmd_rcvd_str); 
			        UART_Tx_Char(NEXT_LINE_CHAR); 
					ctrl_status_system.fsm_system = FSM_PROC_RCVD_INPUT_CMD;
				}
              break;
			  case FSM_PROC_RCVD_INPUT_CMD:
			    num_token = 1;
			    ret_status = SUCCESS;
			    token_str = strtok(motor_cmd_rcvd_str, " ");
				while(token_str != NULL && ret_status != ERR_CMD_MOTOR_TURN_INVALID && num_token <= 4)
				{
					switch(num_token)
					{
                       case 1:    //motor
                          if(!strcmp(motor_str, token_str))
						  {
							   ++num_token;
						  }
                          else
                          {
							    ret_status = ERR_CMD_MOTOR_TURN_INVALID;
								UART_Tx_Str("\r ERR: Due to MOTOR \r");
                          } 
                       break;
					   case 2: 
					     motor_id = (unsigned int) atoi(token_str);
						 if(motor_id > NUM_MOTORS + 1 || motor_id <= 0)
						 {
							   ret_status = ERR_CMD_MOTOR_TURN_INVALID;
							 	 UART_Tx_Str("\r ERR: Due to MOTOR ID \r");
						 }
						 else
						 {
                              sprintf(motor_id_str, "%d",motor_id);
							  if(!strcmp(token_str, motor_id_str))
							  {
								  ++num_token;
							  }
							  else
							  {
								     ret_status = ERR_CMD_MOTOR_TURN_INVALID;
									   UART_Tx_Str("\r ERR: Due to MOTOR ID is not num \r");
							  }
						 }
                       break;
                       case 3:
                          if(!strcmp(motor_clockwise_cmd_str, token_str))
                             ret_status = CMD_MOTOR_TURN_CLOCKWISE;
                          else if(!strcmp(motor_off_cmd_str, token_str))
                               ret_status = CMD_MOTOR_TURN_OFF;
                               else if(!strcmp(motor_status_cmd_str, token_str))
									    ret_status = CMD_MOTOR_STATUS;
									 else
									 {
                                         ret_status = ERR_CMD_MOTOR_TURN_INVALID;
										 UART_Tx_Str("\r ERR: Due to MOTOR command \r");
								     }
                          ++num_token;							   
                       	break;
                        default:
                          ret_status = ERR_CMD_MOTOR_TURN_INVALID;	
                          UART_Tx_Str("\r ERR: Due to exceeds token count \r"); 												
					}
					token_str = strtok(NULL, " ");
				}
				UART_Reset_Datas(UART_RESET_ALL_DATAS);
				if(num_token != 4 || token_str != NULL || ret_status == ERR_CMD_MOTOR_TURN_INVALID)
				{
					 ret_status = ERR_CMD_MOTOR_TURN_INVALID;
					 motor_id = NUM_MOTORS;					 
					 UART_Motor_Menu(motor_id);
					 ERROR_LED = STATE_ON;
				}
				else
				{
                     --motor_id;
					  if(ret_status == CMD_MOTOR_STATUS)
					  {							  
						   UART_Motor_Menu(motor_id);
					  }
			    	  else
          		      {	
				         switch(motor_id)
						 {						 
						     case 0:
		                     case 1:
	                           cur_motor_id = motor_id;           		   
		                     break;
                             default:
		                        cur_motor_id = 0;
						 }								
                         UART_Tx_Str("\r MOTOR(S) COMMAND STATE \r");
	                     UART_Tx_Str("==========================\r");
                         for (; cur_motor_id < NUM_MOTORS ; ++cur_motor_id)
	                     {
		                     UART_Tx_Str(motor_str);
		                     sprintf(motor_id_str, " %u ", cur_motor_id + 1);
		                     UART_Tx_Str(motor_id_str);
    						 FSM_Motor_Proc(cur_motor_id, ret_status); 
                             if(motor_id < NUM_MOTORS)
                                   break;								 
						 }
					  }							
				}
                UART_Tx_Str("\rINPUT: Motor Commmand - ENTER : ");
				ctrl_status_system.fsm_system = FSM_RCVD_CMD_CHAR;
		     break;			 
			 case FSM_EXCEEDS_INPUT_CHARS:
			     if(ctrl_status_system.error_reset_sw_enable_flag == STATE_ON)
				 {
			        sw_timeout = SW_DEBOUNCE_COUNT;
				    if(ERR_RESET_SW == 0) // sw pressed
				    {
				     	Delay_in_Milli_Sec(SW_DEBOUNCE_TIMEOUT);
				    	while(ERR_RESET_SW == 0 && --sw_timeout != 0);
                        UART_Reset_Datas(UART_RESET_ALL_DATAS);	
						ERROR_LED = STATE_OFF;
						UART_Tx_Str("\rINPUT: Motor Commmand - ENTER : ");							
						ctrl_status_system.error_reset_sw_enable_flag = STATE_OFF;
						ctrl_status_system.fsm_system = FSM_RCVD_CMD_CHAR;
					}			   
		        }
             break;
			 case FSM_EMERGENCY_MOTORS_OFF:		
                if(ctrl_status_system.emergency_stop_occured_flag == STATE_ON)
		         {
					 ctrl_status_system.emergency_stop_enable_flag = STATE_OFF;
			         for(cur_motor_id = 0; cur_motor_id < NUM_MOTORS; ++cur_motor_id)
			             DC_Motor_Cmd_Off(cur_motor_id);
			        UART_Reset_Datas(UART_RESET_ALL_DATAS);	
			        ERROR_LED = STATE_OFF;
			        ctrl_status_system.emergency_stop_occured_flag = STATE_OFF;
			        UART_Tx_Str("\r Due to Emergency - Motors OFF");
			        UART_Tx_Str("\rINPUT: Motor Commmand - ENTER : ");
					ctrl_status_system.fsm_system = FSM_RCVD_CMD_CHAR;	
                    EX0 = 1;
                    IT0 = 1;					
		        } 			 
		     break;		
		 }
	 }		 
	return; 
}

/*------------------------------------------------------------*
FUNCTION NAME  : Ext_int_Init

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        :   
-*------------------------------------------------------------*/
void Ext_int_Init()				
{
	//INT0 = 0;
	EX0 = 1;      	/* Enable Ext. interrupt0 */
	IT0 = 1;      	/* Select Ext. interrupt0 on falling edge */	
}

void External0_ISR() interrupt 0
{
	EX0 = 0;      	/* Disable Ext. interrupt0 */
	ctrl_status_system.emergency_stop_occured_flag = STATE_ON;
	ctrl_status_system.fsm_system = FSM_EMERGENCY_MOTORS_OFF;	
} 

/*------------------------------------------------------------*
FUNCTION NAME  : UART_Motor_Menu

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        :   
-*------------------------------------------------------------*/
unsigned char UART_Motor_Menu(const unsigned int motor_id)
{
 	unsigned int cur_motor_id;	
	
    switch(motor_id)
	{
		case 0:
		case 1:
	      cur_motor_id = motor_id;           		   
		break;
        default:
		   cur_motor_id = 0;			   
	}
	UART_Tx_Str("\r MOTOR(S) CURRENT STATE \r");
	UART_Tx_Str("==========================\r");
	for (; cur_motor_id < NUM_MOTORS ; ++cur_motor_id)
	{
		UART_Tx_Str(motor_str);
		sprintf(motor_id_str, " %u ", cur_motor_id + 1);
		UART_Tx_Str(motor_id_str);
		switch(motor_ids[cur_motor_id].cur_fsm_motor_state)
        {
	         case FSM_MOTOR_IN_UNKNOWN_STATE:			  
	            UART_Tx_Str("in unknown state\r");		       
		         break; 		
	         case FSM_MOTOR_IN_OFF:
	            UART_Tx_Str("is in OFF state\r");
					 break;
             case FSM_MOTOR_IN_CLOCKWISE:			 
                UART_Tx_Str("is running in clockwise\r");
						 break;               						 
		 }
     if(motor_id < NUM_MOTORS)
         break;			 
	}
	return SUCCESS; 
}

/*------------------------------------------------------------*
FUNCTION NAME  : FSM_Motor_Proc

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        :   
-*------------------------------------------------------------*/
unsigned char FSM_Motor_Proc(const unsigned int motor_id, const unsigned char motor_cmd)
{
	const char already_str[] = "already ";	
	unsigned int cur_motor_id = motor_id;
	unsigned int ret_status = SUCCESS;
	
	
	switch(motor_id)
	{
		case 0:
		case 1:
	       cur_motor_id = motor_id;            	 		  
		break;
        default:
		   cur_motor_id = 0;			   
	}
	if(ret_status == SUCCESS)
	{
         switch(motor_cmd)
	     {
		     case CMD_MOTOR_TURN_OFF:
		     case CMD_MOTOR_TURN_CLOCKWISE:
		     break;
		     default:
		        UART_Tx_Str(": Invalid motor command");
            ret_status = ERR_CMD_MOTOR_TURN_INVALID;				   
		 }				
	}	
    if(ret_status == SUCCESS)	
    {		
       switch(motor_ids[motor_id].cur_fsm_motor_state)
    	{
	    	case FSM_MOTOR_IN_OFF:
		        switch(motor_cmd)
		        {
		            case CMD_MOTOR_TURN_OFF: 
				      UART_Tx_Str(already_str); 
		         	  UART_Tx_Str(motor_off_cmd_str); 
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
		       motor_ids[motor_id].cur_fsm_motor_state = FSM_MOTOR_IN_UNKNOWN_STATE;
           UART_Tx_Str("in unknown state\r"); 
           ret_status = ERR_MOTOR_UNKNOWN_STATE; 				
		 }			   
	}
	if(ret_status == SUCCESS)
	{
		  UART_Tx_Str("triggered ");
	    switch(motor_cmd)
	    {
			 case CMD_MOTOR_TURN_OFF:
		        if(motor_ids[motor_id].cur_fsm_motor_state != FSM_MOTOR_IN_OFF)
		        {	
			        DC_Motor_Cmd_Off(motor_id);                   
         	        UART_Tx_Str(motor_off_cmd_str);	
                    ret_status = MOTOR_TURNED_OFF;
		        }
	         break;
	      	 case CMD_MOTOR_TURN_CLOCKWISE:
		        if(motor_ids[motor_id].cur_fsm_motor_state != FSM_MOTOR_IN_CLOCKWISE)
		        {
					    DC_Motor_Cmd_Clockwise(motor_id);		                            
			        UART_Tx_Str(motor_clockwise_cmd_str);	
                    ret_status = MOTOR_TURNED_CLOCKWISE;
		        }
		    break; 			
            default:
               ;    			
	    }
	}
	UART_Tx_Char(NEXT_LINE_CHAR);
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
unsigned char DC_Motor_Init(const unsigned int cur_motor_id, const unsigned char init_fsm_motor_state)
{
	 if(cur_motor_id >= NUM_MOTORS || init_fsm_motor_state >= NUM_FSM_MOTOR_STATES)
	{
		UART_Tx_Str("ERR: Motor Init \r");
		ERROR_LED = STATE_ON;
		return ERR_DATA_NOT_IN_RANGE; 
	} 
	switch(cur_motor_id)
	{
		  case 0:
				 MOTOR_1_DRIVER_IN1 = 0;
	       MOTOR_1_DRIVER_IN2 = 0;
	       MOTOR_1_DRIVER_EN1 = 0;
	       MOTOR_1_OFF_LED = 0;
	       MOTOR_1_CLOCKWISE_LED = 0;
			break;
			case 1:
				 MOTOR_2_DRIVER_IN1 = 0;
	       MOTOR_2_DRIVER_IN2 = 0;
	       MOTOR_2_DRIVER_EN1 = 0;
	       MOTOR_2_OFF_LED = 0;
	       MOTOR_2_CLOCKWISE_LED = 0;
			break;	
	}
	motor_ids[cur_motor_id].init_state = init_fsm_motor_state;
	motor_ids[cur_motor_id].motor_id = cur_motor_id;
	switch(init_fsm_motor_state)
	{
		case CMD_MOTOR_TURN_OFF:
		    DC_Motor_Cmd_Off(cur_motor_id);				  
		break;
		case CMD_MOTOR_TURN_CLOCKWISE:
		   DC_Motor_Cmd_Clockwise(cur_motor_id);
		break;   
	}	
	return SUCCESS; 
}
	 
/*------------------------------------------------------------*
FUNCTION NAME  : DC_Motor_Cmd_Off

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        :   
-*------------------------------------------------------------*/
unsigned char DC_Motor_Cmd_Off(const unsigned int motor_id)
{
	switch(motor_id)
	{
		case 0:
		     MOTOR_1_DRIVER_IN1 = STATE_OFF;
	       MOTOR_1_DRIVER_IN2 = STATE_OFF;
	       MOTOR_1_DRIVER_EN1 = STATE_OFF;
	       MOTOR_1_OFF_LED = STATE_ON;
	       MOTOR_1_CLOCKWISE_LED = STATE_OFF;		  
        break;
        case 1:
           MOTOR_2_DRIVER_IN1 = STATE_OFF;
	       MOTOR_2_DRIVER_IN2 = STATE_OFF;
	       MOTOR_2_DRIVER_EN1 = STATE_OFF;
	       MOTOR_2_OFF_LED = STATE_ON;
	       MOTOR_2_CLOCKWISE_LED = STATE_OFF;	
        break;
        default:
          return ERR_MOTOR_ID_INVALID;	  		
	}
	 motor_ids[motor_id].cur_fsm_motor_state = FSM_MOTOR_IN_OFF;
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : DC_Motor_Cmd_Clockwise

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        :   
-*------------------------------------------------------------*/
unsigned char DC_Motor_Cmd_Clockwise(const unsigned int motor_id)
{
	switch(motor_id)
	{
		case 0:
		   MOTOR_1_DRIVER_EN1 = STATE_ON;
           MOTOR_1_DRIVER_IN1 = STATE_ON;
		   MOTOR_1_DRIVER_IN2 = STATE_OFF;
		   MOTOR_1_OFF_LED = STATE_OFF;
		   MOTOR_1_CLOCKWISE_LED = STATE_ON;
        break;
        case 1:
           MOTOR_2_DRIVER_EN1 = STATE_ON;
           MOTOR_2_DRIVER_IN1 = STATE_ON;
		   MOTOR_2_DRIVER_IN2 = STATE_OFF;
		   MOTOR_2_OFF_LED = STATE_OFF;
		   MOTOR_2_CLOCKWISE_LED = STATE_ON;
        break;
        default:
          return ERR_MOTOR_ID_INVALID;	  		
	}
	motor_ids[motor_id].cur_fsm_motor_state = FSM_MOTOR_IN_CLOCKWISE;
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

#ifdef UNUSED
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

#endif

/*------------------------------------------------------------------*-
  ---- END OF FILE -------------------------------------------------
-*------------------------------------------------------------------*/	 
