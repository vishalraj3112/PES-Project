/***********************************************************************************************************************
* File Name    : main.c
* Project      : PES Assignment 6
* Version      : 1.0
* Description  : Contains all the function calls code to the various functions for command terminal operation
* Author       : Vishal Raj
* Creation Date: 11.10.21
* Remarks	   : makefile.defs referred from 'https://embeddedartistry.com/blog/2019/02/18/using-custom-build-steps-with
* 				 -eclipse-auto-generated-makefiles/'
***********************************************************************************************************************/
#include "sysclock.h"
#include "uart.h"
#include "stdint.h"
#include <stdio.h>
#include "cbfifo.h"
#include "MKL25Z4.h"
#include "test_cbfifo.h"
#include "command_process.h"
#include "Log.h"
#include "huff.h"

// ***********************************************************************************************
// * Name :            	main
// * Description :     	Program entry point, calls all other functions
// * Inputs :			None
// * RETURN :			None
// ***********************************************************************************************
int main(void)
{
	uint8_t user_input[80];
	uint8_t* user_input_ptr;
	sysclock_init();
	Init_UART0();

	#ifdef DEBUG
	test_cbfifo();//Test cbfifo once
	#endif

	//printf("Test string\r\n");

	huffman();

	printf("Welcome to BreakfastSerial!\r\n");
	printf("? ");
	while (1) {
		user_input_ptr = accumulator(user_input);
		process_cmd((char *)user_input_ptr);
		printf("? ");
	}

	return 0 ;
}
/*[EOF]*/
