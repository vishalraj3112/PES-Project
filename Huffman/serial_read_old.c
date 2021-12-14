/***********************************************************************************************************************
* File Name    : serial_read.c
* Project      : PES Final Project
* Version      : 1.0
* Description  : Contains all the PC side decompression code for huffman project
* Author       : Vishal Raj & referred from https://www.xanthium.in/Serial-Port-Programming-using-Win32-API.
* Creation Date: 12.11.21
***********************************************************************************************************************/
#include <Windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include "huffman_tree_gen.h"
#include <assert.h>

//static function declarations
static uint8_t string_to_hex(char array[]);
static void decode_string(uint8_t enc_buff[], uint16_t enc_bits, uint8_t dec_buff[]);
static void test_decoded_data(uint8_t decoded_string[], uint8_t input_no);

bool tests_passed = true;
uint16_t enc_buff_size[10] = {0}; // used for analysis
float average_compression_ratio = 0.0;

/***********************************************************************************************
* Name             : main
* Description      : main function calls all other sub functions, used to set the uart parameters
*                    and seperated the encoded frames to provide to the decoding function.
* Parameters       : void
* RETURN           : None
***********************************************************************************************/
int main(void)
{

    printf("Waiting for encoded inputs from target controller...\r\n");

    HANDLE hComm;
    DCB dcbParams = { 0 };
    char buffer[200] = {0};
    DWORD bytesRead;
    int loop = 0, i =0;
    bool status;
    char ReadData; //change name

    hComm = CreateFileA("\\\\.\\COM5",                //change port number here
                        GENERIC_READ | GENERIC_WRITE, //Read, Write enable
                        0,                            
                        NULL,                        
                        OPEN_EXISTING,
                        0,            
                        NULL);        

    if (hComm == INVALID_HANDLE_VALUE){
        printf("Error in opening serial port");
        goto exit;
    }

    dcbParams.DCBlength = sizeof(dcbParams);
    status = GetCommState(hComm, &dcbParams);
    if (status == FALSE)
    {
        printf("\nError to Get the Com state\n\n");
        goto exit;
    }

    /* Baud rate parameters are set here*/ 
    dcbParams.BaudRate = CBR_38400;      //BaudRate = 38400
    dcbParams.ByteSize = 8;             //ByteSize = 8
    dcbParams.StopBits = TWOSTOPBITS;    //StopBits = 2
    dcbParams.Parity = NOPARITY;      //Parity = None
    status = SetCommState(hComm, &dcbParams);
    if (status == FALSE)
    {
        printf("\nError to Setting DCB Structure\n\n");
        goto exit;
    }

   while(1){
   
        //Read data and store in a buffer

        status = ReadFile(hComm, &ReadData, sizeof(ReadData), &bytesRead, NULL);
        while(bytesRead > 0){
            buffer[i++] = ReadData;
            status = ReadFile(hComm, &ReadData, sizeof(ReadData), &bytesRead, NULL);
        }

        int j =0;
        // for(j = 0; j < i; j++){ //for debugging
        //     printf("buff[%d]: %c\n",j,buffer[j]);
        // }


        uint8_t temp[3] = {0}, num = 0, buff_2[200] = {0}, enc_buff[100] = {0};
        uint16_t idx = 0;
        int length = 0, prev_i = 0;
        uint8_t decodedstring[200] = {0}, input_no = 1;

        i = 0;

        /*Here the input buffer is read from the serial port and seperated into
        various token using the 0xFF end of token seperator given during encoding.
        The data is then given to the decode function and an automated test is
        run to test the inputs provided by KL25Z.*/
        while(buffer[i] != '\0'){

            num = buffer[i];
            buff_2[i] = num;
            //printf("buf[%d]:%x\r\n",i,num);

            if(num == 0xff){

                length = i - prev_i;

                if((length > 0) && (length <= 32)){//32 data bytes + 1 size byte
                    idx = buff_2[i-1];
                    enc_buff_size[input_no] = idx;//store the size for compression calc
                }else{
                    idx = buff_2[i-2];//Lower nibble
                    uint16_t shift_temp = buff_2[i-1];
                    idx |= shift_temp << 8;
                    enc_buff_size[input_no] = idx;//store the size for compression calc
                }

                prev_i = i;

                if(idx){ //if there is some size in input string
                    //printf("size bits:%d\r\n",idx);

                    //decoding
                    uint16_t start = 0;
                    if(i-length)
                        start = i - length + 1;
                    else
                        start = i - length;

                    uint16_t upto = length - 1;

                    memcpy(enc_buff, (buff_2 + start), upto);
                    printf("encoded string dump:(hex)\n\r");
                    for(int k=0;k<length;k++){//dump of encoded string
                        
                        if((k == 0) || (k%16 == 0))
                            printf("\r\n");
                        
                        printf("%02x ",enc_buff[k]);
                    }

                    decode_string(enc_buff, idx, decodedstring);

                    printf("\n\nDecoded string:\n%s\r\n\n", decodedstring);

                    test_decoded_data(decodedstring,input_no);
                    input_no++;
                    printf("--------------------------------------------------------------------------------------------\n");
                }                   

            }

            i++;
        }


        if(idx){ //if there is some size in input string

            //clear all buffers
            bytesRead = 0;
            ReadData = 0;
            idx = 0;
            memset(buffer,0,sizeof(buffer));
            memset(buff_2,0,sizeof(buff_2));
            memset(decodedstring,0,sizeof(decodedstring));
        }   

    }//while(1)

exit:
    CloseHandle(hComm);//Closing the Serial Port

  return 0;
}


/***********************************************************************************************
* Name             : test_decoded_data
* Description      : automated test function to check the incoming strings of encoeded inputs
*                    and check if the decoded inputs match the originally sent input.
* Parameters       : string which was decoded and the test case number
* RETURN           : None
***********************************************************************************************/
void test_decoded_data(uint8_t decoded_string[], uint8_t input_no)
{
    char data_sent_1[] = "entering a random strin";
    char data_sent_2[] = "test string 12345 RaNDommmmm cAsinggg";
    char data_sent_3[] = "Just gonna stand there and watch me cry";
    char data_sent_4[] = "Dec 11 04:05:45 vishal-Lenovo-ideapad-520S-14";
    char data_sent_5[] = "Dec 11 04:09:45 vishal-Lenovo-ideapad-520S-14IKB org.gnome.Shell.desktop[2145]: #0 0x7ffed15c99d0 I   resource:///org/gnome/gjs/modules/";

    float input_1_size = (float)sizeof(data_sent_1) * 8;//size in bits
    float input_2_size = (float)sizeof(data_sent_2) * 8;//size in bits
    float input_3_size = (float)sizeof(data_sent_3) * 8;//size in bits
    float input_4_size = (float)sizeof(data_sent_4) * 8;//size in bits
    float input_5_size = (float)sizeof(data_sent_5) * 8;//size in bits

    float get_input1_size = (float)enc_buff_size[1];
    float get_input2_size = (float)enc_buff_size[2];
    float get_input3_size = (float)enc_buff_size[3];
    float get_input4_size = (float)enc_buff_size[4];
    float get_input5_size = (float)enc_buff_size[5];

    input_1_size = get_input1_size/input_1_size * 100;
    input_2_size = get_input2_size/input_2_size * 100;
    input_3_size = get_input3_size/input_3_size * 100;
    input_4_size = get_input4_size/input_4_size * 100;
    input_5_size = get_input5_size/input_5_size * 100;


    average_compression_ratio = (input_1_size + input_2_size + input_3_size + input_4_size + input_5_size)/5;


    if(input_no == 1){

        assert(!strncmp(data_sent_1,decoded_string,strlen(data_sent_1)));
        if(!strncmp(data_sent_1,decoded_string,strlen(data_sent_1)))
		  printf("Test 1 passed!\r\n");
	    else{
		  printf("Fail!\r\n");
          tests_passed = false;
        }
    }else if(input_no == 2){

        assert(!strncmp(data_sent_2,decoded_string,strlen(data_sent_2)));
        if(!strncmp(data_sent_2,decoded_string,strlen(data_sent_2)))
          printf("Test 2 passed!\r\n");
        else{
          printf("Failed test 2!\r\n");
          tests_passed = false;
        }
    }else if(input_no == 3){

        assert(!strncmp(data_sent_3,decoded_string,strlen(data_sent_3)));
        if(!strncmp(data_sent_3,decoded_string,strlen(data_sent_3)))
          printf("Test 3 passed!\r\n");
        else{
          printf("Failed test 3!\r\n");
          tests_passed = false;
        }
    }
    else if(input_no == 4){

        assert(!strncmp(data_sent_4,decoded_string,strlen(data_sent_4)));
        if(!strncmp(data_sent_4,decoded_string,strlen(data_sent_4)))
          printf("Test 4 passed!\r\n");
        else{
          printf("Failed test 4!\r\n");
          tests_passed = false;
        }
    }
    else if(input_no == 5){

        assert(!strncmp(data_sent_5,decoded_string,strlen(data_sent_5)));
        if(!strncmp(data_sent_5,decoded_string,strlen(data_sent_5)))
          printf("Test 5 passed!\r\n");
        else{
          printf("Failed test 5!\r\n");
          tests_passed = false;
        }
    }

    if((tests_passed == true) && (input_no == 5)){
        printf("\n---------------------------------\n");
        printf("   All PC decode tests passed!\r\n");
        printf("---------------------------------\r\n");
        printf("\nAverage compression ratio achieved:%f%\r\n",average_compression_ratio);
    }

    printf("\r\n");
}


/***********************************************************************************************
* Name             : decode_string
* Description      : function to decode an encoded input using the huffman LUT
* Parameters       : encoded buffer, no of encoded bits and the buffer to store the decoded bits.
* RETURN           : min between a and b
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