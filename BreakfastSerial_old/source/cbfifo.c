/***********************************************************************************************************************
* File Name    : cbfifo.c
* Project      : PES Assignment 6
* Version      : 2.0
* Description  : Contains all the function implementation code along with variable definitions for cbfifo
* Author       : Vishal Raj
* Creation Date: 11.10.21
***********************************************************************************************************************/
#include "cbfifo.h"
//#include "test_cbfifo.h"

char uufifo[MAX_SIZE];//For seeing dequedued data
unsigned char data0[MAX_SIZE]={0};//For making nodes 0

struct cbfifo_s
{
	unsigned char fifo_arr[MAX_SIZE];
	int head;
	int tail;
	size_t length;
	size_t capacity;
};

cbfifo_t rx = {.head = -1, .tail = -1,.length = 0, .capacity = 0};
cbfifo_t tx = {.head = -1, .tail = -1,.length = 0, .capacity = 0};

/***********************************************************************************************
* Name			   : datacpy        	
* Description 	   : used to copy data from one location to another
* Parameters 	   : data1-desitination , data2-source, bytes-no of bytes to store
* RETURN 		   : Pointer to newly created instance
***********************************************************************************************/
unsigned char * datacpy(unsigned char * data1,unsigned char * data2 , size_t bytes){

	unsigned char * ptr = data1;

	if(data1 == NULL)//return null if no memory is alloted to desitination
		return NULL;

	for(int i=0 ; i < bytes ; i++){
		*data1 = *data2;
		data1++;
		data2++;
	}

	//*data1 = '\0';

	return ptr;

}
/***********************************************************************************************
* Name			   : cbfifo_enqueue        	
* Description 	   : used to enqueue data in fifo
* Parameters 	   : The fifo, the buffer in question and no of bytes to enqueue
* RETURN 		   : no of bytes enqueued
***********************************************************************************************/
size_t cbfifo_enqueue(cbfifo_t *fifo, void* buf, size_t nbyte){

	size_t i =0;

	if(buf == 0)//If null pointer received
		return -1;

	if(nbyte < 0)
		return -1;

	if((fifo->head == -1)){//first element
		fifo->head = 0;
		fifo->tail = 0;
	 }

	fifo->length = cbfifo_length(fifo);//get current length
	fifo->capacity = MAX_SIZE;

	if(fifo->length == fifo->capacity)//ignore new inputs if buffer is full
		return 0;

	if(nbyte > (fifo->capacity - fifo->length))
		nbyte = (fifo->capacity - fifo->length);//modify nbyte if nbyte > cap -len(free space)

	if((fifo->tail + nbyte) > MAX_SIZE){//reset tail to start location
		
		for(i = 0;i < nbyte ; i++){//fill queue from buffer byte by byte
			datacpy((fifo->fifo_arr+fifo->tail),(buf+i),1);
			fifo->tail += 1;
			if(fifo->tail == MAX_SIZE)
				fifo->tail = 0;
		}
	}else{
		datacpy((fifo->fifo_arr+fifo->tail),buf,nbyte); //move nbytes in fifo from current tail
		fifo->tail = fifo->tail + nbyte;//move tail nbytes ahead
	}

	if(fifo->tail >= MAX_SIZE)
		fifo->tail = 0;
	
	//printf("bytes enqued: %ld<-\n",nbyte);//remove this only for test
	return nbyte;
}
/***********************************************************************************************
* Name			   : cbfifo_dequeue        	
* Description 	   : Used to dequeue data from the fifo
* Parameters 	   : The fifo,the buf where dequeued data needs to be stored and no of bytes to be dequeued
* RETURN 		   : no of actual bytes dequeued or 0 on error case
***********************************************************************************************/
size_t cbfifo_dequeue(cbfifo_t* fifo, void *buf, size_t nbyte){

	size_t i = 0;

	if(fifo->head == -1)
		return 0;//error case-fifo empty
	else{

		fifo->length = cbfifo_length(fifo);//get current length
		if(nbyte > fifo->length)//nbyte cannot be greater than length
			nbyte = fifo->length;

		if((fifo->head + nbyte) >= MAX_SIZE){ // if input is greater than available read locations
			
			for(i = 0 ; i < nbyte ; i++){
				datacpy((buf+i),(fifo->fifo_arr+fifo->head),1); //copy data in buf by byte
				datacpy((fifo->fifo_arr+fifo->head),data0,1); //make the data 0 by byte
				fifo->head += 1;
				if(fifo->head == MAX_SIZE)
					fifo->head = 0;
			}
		}
		else{
			for(i = 0;i < nbyte ; i++){
				datacpy((buf+i),(fifo->fifo_arr+fifo->head),1); //copy data in buf
				datacpy((fifo->fifo_arr+fifo->head),data0,1); //make the data 0
				fifo->head += 1;
				if(fifo->head ==/*>=*/ fifo->tail){ //newly added '==' instead of '>='---
					nbyte = (i+1); //save nbyte before breaking
					break;
				}
			}
		}
	}
	//printf("bytes dequed: %ld->\n",nbyte);//remove this only for test
	return nbyte;
}
/***********************************************************************************************
* Name			   : cbfifo_length        	
* Description 	   : Used to find the length of the fifo
* Parameters 	   : The fifo
* RETURN 		   : The length of fifo in bytes
***********************************************************************************************/
size_t cbfifo_length(cbfifo_t* fifo){

	size_t node=0;//check this array size can be larger than 128
	size_t lngt=0 , i = 0;

	
	for(i=0 ; i < MAX_SIZE ; i++){
		if(*(fifo->fifo_arr+node) != 0)
			lngt++;
		node++;
	}

	return lngt;
}
/***********************************************************************************************
* Name			   : cbfifo_capacity        	
* Description 	   : Used to find the overall capacity of the fifo
* Parameters 	   : The fifo
* RETURN 		   : The fifo capacity in bytes
***********************************************************************************************/
size_t cbfifo_capacity(cbfifo_t* fifo){

	return sizeof(fifo->fifo_arr);
}

/*[EOF]*/
