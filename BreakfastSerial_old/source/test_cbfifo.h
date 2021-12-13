/***********************************************************************************************************************
* File Name    : test_cbfifo.h
* Project      : PES Assignment 6
* Version      : 1.0
* Description  : Contains all the function prototypes along with variable declarations for cbfifo test suite
* Author       : Vishal Raj
* Creation Date: 11.10.21
***********************************************************************************************************************/
#ifndef _TEST_CBFIFO_H_
#define _TEST_CBFIFO_H_
#include "cbfifo.h"

#include <stdio.h>

//extern variables 
extern unsigned char fifo[MAX_SIZE];
extern char uufifo[MAX_SIZE];
extern unsigned char data0[MAX_SIZE];

extern short head,tail;


/***********************************************************************************************
* Name			   : test_cbfifo
* Description 	   : used to run test on cbfifo multiple instances.
* Parameters 	   : None
* RETURN 		   : None
***********************************************************************************************/
void test_cbfifo();


#endif // _TEST_CBFIFO_H_
/*[EOF]*/
