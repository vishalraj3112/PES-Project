/***********************************************************************************************************************
* File Name    : command_process.h
* Project      : PES Assignment 6
* Version      : 1.0
* Description  : Contains all the function prototypes along with variable declarations for command processor
* Author       : Vishal Raj
* Creation Date: 11.10.21
***********************************************************************************************************************/
#ifndef COMMAND_PROCESS_H_
#define COMMAND_PROCESS_H_

#include <stdint.h>


/***********************************************************************************************
* Name			   : process_cmd
* Description 	   : Processes user input in seperate words
* Parameters 	   : User input line to be processed
* RETURN 		   : None
***********************************************************************************************/
void process_cmd(char *input);

/***********************************************************************************************
* Name			   : accumulator
* Description 	   : used to accumulate input line from user.
* Parameters 	   : None
* RETURN 		   : None
***********************************************************************************************/
uint8_t* accumulator(uint8_t user_input[]);


#endif /* COMMAND_PROCESS_H_ */
/*[EOF]*/
