/***********************************************************************************************************************
* File Name    : huff.h
* Project      : PES Final Project
* Version      : 1.0
* Description  : Contains the function declarations for huffman function.
* Author       : Vishal Raj
* Creation Date: 12.10.21
***********************************************************************************************************************/


#ifndef HUFF_H_
#define HUFF_H_

/***********************************************************************************************
* Name			   : exe_huffman_task
* Description 	   : used to conduct test syswrite test, by compressing data here and decompress
* 					 is performed on the P, then compression-decompression is performed on the
* 					 KL25Z itself
* Parameters 	   : None
* RETURN 		   : None
***********************************************************************************************/
void exe_huffman_task(void);

/***********************************************************************************************
* Name			   : encode_string
* Description 	   : encoding of input string performed here, called from syswrite, uses huffman
* 					 look-up table for ccodes.
* Parameters 	   : string to be encoded, buffer to store the encoded values, size of the buffer
* RETURN 		   : None
***********************************************************************************************/
int encode_string(char *message, uint8_t *buffer, size_t nbytes);

#endif /* HUFF_H_ */
