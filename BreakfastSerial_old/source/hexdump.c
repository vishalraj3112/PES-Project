/***********************************************************************************************************************
* File Name    : hexdump.c
* Project      : PES Assignment 6
* Version      : 1.0
* Description  : Contains all the function implementation code along with variable definitions for hexdump
* Author       : Vishal Raj & reference from Howdy Pierce Lec 22 slide
* Creation Date: 11.10.21
***********************************************************************************************************************/
#include "hexdump.h"
#include <stdio.h>

/***********************************************************************************************
* Name			   : hexdump
* Description 	   : used print hexdump for specific input.
* Parameters 	   : start address and length upto which hexdump is to be printed
* RETURN 		   : None
***********************************************************************************************/
void hexdump(uint32_t start, uint32_t len)
{

	uint8_t * start_addr =(uint8_t *) start;

	for(int i=0;i<len;i +=16 ){

		printf("%04X",((start+i)>>16));
		printf("_");
		printf("%04X	",((start+i) & 0xFFFF));

		for(int j = 0 ; j < 16 ; j++){

			if(i+j > (len-1)){
				printf("\n\r");
				return;
			}
			printf("%02X ", *(start_addr+i+j));
		}

		printf("\n\r");
	}

}

/*[EOF]*/
