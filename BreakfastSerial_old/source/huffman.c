/*
 * huffman.c
 *
 *  Created on: 11-Dec-2021
 *      Author: vishalraj
 */
#include "huffman.h"
#include "huff.h"
#include <string.h>
#include <stdio.h>

#define MASK(x)     (0xFF >> (8-x))

static void decode_string(uint8_t encoded_buffer[], uint16_t encoded_bits, uint8_t decoded_buffer[]);
static int min(int a,int b);
static void test_syswrite_encoding(void);

int encoded_bits = 0;

void huffman(void)
{

	test_syswrite_encoding();

	uint8_t decodedstring[200] = {0};

	char data_to_encode[] = "entering a random strin";
//
//	printf("%s",data_to_encode);//syswrite test

	uint8_t buff[1024];

	//encoding
	uint16_t indexes = encode_string(data_to_encode, buff, sizeof(buff));//string encoded here

	//print the encoded buffer
	for(int i = 0; i < 14; i++){//105 = 13.125 bits
		//printf("buff[%d]:%x\r\n",i,buff[i]);
		printf("%x",buff[i]);
	}
	printf("\r\n");
	printf("encoded bits:%d\r\n",indexes);

	//decoding
	decode_string(buff, indexes, decodedstring);

	printf("\n%s\r\n", decodedstring);

	if(!strncmp((char *)data_to_encode,(char *)decodedstring,strlen(data_to_encode))){
		printf("Encode = decode\r\n");
	}else
		printf("Fail!\r\n");

	//Now send the encoded string over UART

}

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

int encode_string(char *message, uint8_t *buffer, size_t nbytes)
{
    if(*message == '\0') {
        printf("Error: Empty Message");
        return 0;
    }

//    int min_idx = 0;
    int i;
    int bits_written = 0;
    int buffer_idx = 0;

    memset(buffer, 0, nbytes);

    //while(*message != '\0') {
    for (char *p = message; *p != '\0'; p++) {
        for (i = 0; huffman_table[i].data != *p; i++);

        //uint32_t code = (huffman_table[i].ccode>>1);//struct memory alignment issue hence need to shift->when using LUT
        uint32_t code = huffman_table[i].ccode;//when not using LUT
        int no_bits = huffman_table[i].no_bits;


        while(no_bits > 0) {
            int this_write = min(8 - bits_written, no_bits);

            int readshift = no_bits - this_write;
            uint32_t temp = (code >> readshift) & MASK(this_write)/*((1UL << this_write) -1)*/;

            int write_shift = 8 - bits_written - this_write;
            buffer[buffer_idx] |= temp << write_shift;

            bits_written += this_write;
            no_bits -= this_write;

            if (bits_written == 8) {
                bits_written = 0;
                buffer_idx++;
            }
        }
    }

    encoded_bits = (buffer_idx * 8) + bits_written;
//    if (bits_written > 0)
//        min_idx = 1;

    return 8*buffer_idx + bits_written;
}

int min(int a,int b)
{
	return (a <= b) ? a : b;
}

void decode_string(uint8_t enc_buff[], uint16_t enc_bits, uint8_t dec_buff[])
{
    uint8_t *cc_buf = enc_buff;//problem of this is that the input buffer would be lost
    uint16_t enc_idx = 0, dec_idx = 0;
    uint16_t ccode = 0x00, ccode_len = 1;
    uint16_t i = 1, curr_pos = 0;//indexes

    while(enc_bits != 0){

        ccode |= (cc_buf[enc_idx] & 0x80) >> 7;//read the char codes in reverse


        while(huffman_table[i].data != '\0'){//till end of LUT is not reached
            if(ccode_len == huffman_table[i].no_bits){//length match
                if(ccode == huffman_table[i].ccode){//ccode match
                    dec_buff[dec_idx++] = huffman_table[i].data;
                    ccode = 0;
                    ccode_len = 0;
                    break;
                }
            }
            i++;
        }
        i = 1; //if match then restart

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

