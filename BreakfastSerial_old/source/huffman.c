/***********************************************************************************************************************
* File Name    : huffman.c
* Project      : PES Final Project
* Version      : 1.0
* Description  : Contains all the function implementation code for huffman encode-deocode on KL25Z.
* Author       : Vishal Raj & encoding algorithm referred from Howdy Pierce-Media compression PES lecture 26.
* Creation Date: 12.11.21
***********************************************************************************************************************/
#include "huffman.h"
#include "huff.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>

#define MASK(x)     (0xFF >> (8-x))

static void decode_string(uint8_t encoded_buffer[], uint16_t encoded_bits, uint8_t decoded_buffer[]);
static int min(int a,int b);
static void test_syswrite_encoding(void);
static void test_huffman_encode_decode(void);


/***********************************************************************************************
* Name			   : exe_huffman_task
* Description 	   : used to conduct test syswrite test, by compressing data here and decompress
* 					 is performed on the P, then compression-decompression is performed on the
* 					 KL25Z itself
* Parameters 	   : None
* RETURN 		   : None
***********************************************************************************************/
void exe_huffman_task(void)
{

	test_syswrite_encoding();

	test_huffman_encode_decode();//Kl25 encode-decode test

}

/***********************************************************************************************
* Name			   : test_syswrite_encoding
* Description 	   : KL25Z compression is performed in syswrite function over here.
* Parameters 	   : None
* RETURN 		   : None
***********************************************************************************************/
void test_syswrite_encoding(void)
{
	char data_to_encode1[] = "entering a random strin";
	char data_to_encode2[] = "test string 12345 RaNDommmmm cAsinggg";
	char data_to_encode3[] = "Just gonna stand there and watch me cry";
	char data_to_encode4[] = "Dec 11 04:05:45 vishal-Lenovo-ideapad-520S-14";
	char data_to_encode5[] = "Dec 11 04:09:45 vishal-Lenovo-ideapad-520S-14IKB org.gnome.Shell.desktop[2145]: #0 0x7ffed15c99d0 I   resource:///org/gnome/gjs/modules/";

	printf("%s",data_to_encode1);//syswrite test
	printf("%s",data_to_encode2);//syswrite test
	printf("%s",data_to_encode3);//syswrite test
	printf("%s",data_to_encode4);//syswrite test
	printf("%s",data_to_encode5);//syswrite test

}

/***********************************************************************************************
* Name			   : test_huffman_encode_decode
* Description 	   : KL25Z encode-decode unit test function, used to test the itegrity of the
* 					 algorithm.
* Parameters 	   : None
* RETURN 		   : None
***********************************************************************************************/
void test_huffman_encode_decode(void)
{
    uint8_t decodedstring[200] = {0};

    char data_to_encode[] = "Dec 11 04:05:45 vishal-Lenovo-ideapad-520S-14IKB rsyslogd:";
    char data_to_encode2[] = "Just gonna stand there and watch me burn Well, that's alright, because I like the way it hurts Just gonna stand there and hear me cry";
    char data_to_encode3[] = "Entering a random string";
    char data_to_encode4[] = "test string 12345 RaNDommmmm cAsinggg";

    uint8_t buff[100];

    //test for input - 1
    uint16_t indexes = encode_string(data_to_encode, buff, sizeof(buff));//string encoded here
    decode_string(buff, indexes, decodedstring);
    assert(!strncmp((char*)data_to_encode,(char*)decodedstring,strlen(data_to_encode)));
    memset(buff, 0, sizeof(buff));
    memset(decodedstring,0,sizeof(decodedstring));

    //test for input - 2
    indexes = encode_string(data_to_encode2, buff, sizeof(buff));//string encoded here
    decode_string(buff, indexes, decodedstring);
    assert(!strncmp((char*)data_to_encode2,(char*)decodedstring,strlen(data_to_encode2)));
    memset(buff, 0, sizeof(buff));
    memset(decodedstring,0,sizeof(decodedstring));

    //test for input - 3
    indexes = encode_string(data_to_encode3, buff, sizeof(buff));//string encoded here
    decode_string(buff, indexes, decodedstring);
    assert(!strncmp((char*)data_to_encode3,(char*)decodedstring,strlen(data_to_encode3)));
    memset(buff, 0, sizeof(buff));
    memset(decodedstring,0,sizeof(decodedstring));

    //test for input - 4
    indexes = encode_string(data_to_encode4, buff, sizeof(buff));//string encoded here
    decode_string(buff, indexes, decodedstring);
    assert(!strncmp((char*)data_to_encode4,(char*)decodedstring,strlen(data_to_encode4)));


    printf("\r\n");
    printf("All KL25Z encode-decode tests passed!\r\n");

}

/***********************************************************************************************
* Name			   : encode_string
* Description 	   : encoding of input string performed here, called from syswrite, uses huffman
* 					 look-up table for ccodes.
* Parameters 	   : string to be encoded, buffer to store the encoded values, size of the buffer
* RETURN 		   : None
***********************************************************************************************/
int encode_string(char *string, uint8_t *buff, size_t no_bytes)
{
    if(*string == '\0') {
        printf("Error: Empty Message");
        return 0;
    }

    int i;
    int bits_to_write = 0;
    int buffer_idx = 0;

    memset(buff, 0, no_bytes);

    for (char *p = string; *p != '\0'; p++) {
        for (i = 0; huffman_table[i].data != *p; i++);

        uint32_t code = huffman_table[i].ccode;//when not using LUT
        int no_bits = huffman_table[i].no_bits;


        while(no_bits > 0) {
            int this_write = min(8 - bits_to_write, no_bits);

            int readshift = no_bits - this_write;
            uint32_t temp = (code >> readshift) & MASK(this_write)/*((1UL << this_write) -1)*/;

            int write_shift = 8 - bits_to_write - this_write;
            buff[buffer_idx] |= temp << write_shift;

            bits_to_write += this_write;
            no_bits -= this_write;

            if (bits_to_write == 8) {
            	bits_to_write = 0;
                buffer_idx++;
            }
        }
    }

    return 8*buffer_idx + bits_to_write;
}

/***********************************************************************************************
* Name			   : min
* Description 	   : function to find the minimum of two values
* Parameters 	   : two parameters whose minimum is to be found
* RETURN 		   : min between a and b
***********************************************************************************************/
int min(int a,int b)
{
	return (a <= b) ? a : b;
}

/***********************************************************************************************
* Name			   : decode_string
* Description 	   : function to decode an encoded input using the huffman LUT
* Parameters 	   : encoded buffer, no of encoded bits and the buffer to store the decoded bits.
* RETURN 		   : min between a and b
***********************************************************************************************/
void decode_string(uint8_t enc_buff[], uint16_t enc_bits, uint8_t dec_buff[])
{
    uint8_t *cc_buf = enc_buff;//problem of this is that the input buffer would be lost
    uint16_t enc_idx = 0, dec_idx = 0;
    uint16_t ccode = 0x00, ccode_len = 1;
    uint16_t idx = 1, curr_pos = 0;//indexes

    while(enc_bits != 0){

        ccode |= (cc_buf[enc_idx] & 0x80) >> 7;//read the char codes in reverse


        while(huffman_table[idx].data != '\0'){//till end of LUT is not reached
            if(ccode_len == huffman_table[idx].no_bits){//length match
                if(ccode == huffman_table[idx].ccode){//ccode match
                    dec_buff[dec_idx++] = huffman_table[idx].data;
                    ccode = 0;
                    ccode_len = 0;
                    break;
                }
            }
            idx++;
        }
        idx = 1; //if match then restart

        ccode <<= 1;//shift 1 bits left for next read bit
        ccode_len++;

        cc_buf[enc_idx] <<= 1;//read next bit of data

        curr_pos++;
        if(curr_pos == 8){
            curr_pos = 0;
            enc_idx++;
        }

        enc_bits--;

    }
    dec_buff[dec_idx] = '\0';
}
//[EOF]
