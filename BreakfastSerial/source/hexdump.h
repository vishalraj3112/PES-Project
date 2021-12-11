/***********************************************************************************************************************
* File Name    : hexdump.h
* Project      : PES Assignment 6
* Version      : 1.0
* Description  : Contains all the function prototypes of hexdump
* Author       : Vishal Raj
* Creation Date: 11.10.21
***********************************************************************************************************************/

#ifndef HEXDUMP_H_
#define HEXDUMP_H_
#include <stdint.h>

/***********************************************************************************************
* Name			   : hexdump
* Description 	   : used print hexdump for specific input.
* Parameters 	   : start address and length upto which hexdump is to be printed
* RETURN 		   : None
***********************************************************************************************/
void hexdump(uint32_t start, uint32_t len);


#endif /* HEXDUMP_H_ */


/*[EOF]*/
