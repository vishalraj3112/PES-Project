/***********************************************************************************************************************
* File Name    : cbfifo.h
* Project      : PES Assignment 6
* Version      : 2.0
* Description  : Contains all the function prototypes along with variable declarations for cbfifo
* Author       : Howdy Pierce and Vishal Raj
* Creation Date: 11.10.21
***********************************************************************************************************************/
#ifndef _CBFIFO_H_
#define _CBFIFO_H_

#include <stdlib.h>  // for size_t

#define MAX_SIZE    256//128

typedef struct cbfifo_s cbfifo_t;


extern cbfifo_t rx;
extern cbfifo_t tx;

/***********************************************************************************************
* Name			   : cbfifo_enqueue
* Description 	   : used to enqueue data in fifo
* Parameters 	   : The fifo, the buffer in question and no of bytes to enqueue
* RETURN 		   : no of bytes enqueued
***********************************************************************************************/
size_t cbfifo_enqueue(cbfifo_t* fifo, void *buf, size_t nbyte);


/***********************************************************************************************
* Name			   : cbfifo_dequeue
* Description 	   : Used to dequeue data from the fifo
* Parameters 	   : The fifo,the buf where dequeued data needs to be stored and no of bytes to be dequeued
* RETURN 		   : no of actual bytes dequeued or 0 on error case
***********************************************************************************************/
size_t cbfifo_dequeue(cbfifo_t* fifo, void *buf, size_t nbyte);


/***********************************************************************************************
* Name			   : cbfifo_length
* Description 	   : Used to find the length of the fifo
* Parameters 	   : The fifo
* RETURN 		   : The length of fifo in bytes
***********************************************************************************************/
size_t cbfifo_length(cbfifo_t* fifo);


/***********************************************************************************************
* Name			   : cbfifo_capacity
* Description 	   : Used to find the overall capacity of the fifo
* Parameters 	   : The fifo
* RETURN 		   : The fifo capacity in bytes
***********************************************************************************************/
size_t cbfifo_capacity(cbfifo_t* fifo);


#endif // _CBFIFO_H_
/*[EOF]*/
