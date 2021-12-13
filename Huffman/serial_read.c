#include <Windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include "huffman.h"

static uint8_t string_to_hex(char array[]);
static void decode_string(uint8_t enc_buff[], uint16_t enc_bits, uint8_t dec_buff[]);
static void verify_encoded_data(uint8_t decoded_string[]);

int main()
{
    HANDLE hComm;
    DCB dcbParams = { 0 };
    char buffer[64] = {0};
    DWORD bytesRead;
    DWORD dwEventMask;//change name
    int loop = 0, i =0;
    bool status;
    char ReadData; //change name

    hComm = CreateFileA("\\\\.\\COM5",                //port name
                        GENERIC_READ | GENERIC_WRITE, //Read/Write
                        0,                            // No Sharing
                        NULL,                         // No Security
                        OPEN_EXISTING,// Open existing port only
                        0,            // Non Overlapped I/O
                        NULL);        // Null for Comm Devices

    if (hComm == INVALID_HANDLE_VALUE){
        printf("Error in opening serial port");
        goto exit;
    }//else
        //printf("opening serial port successful");

    //Setting serial port parameters
    dcbParams.DCBlength = sizeof(dcbParams);
    status = GetCommState(hComm, &dcbParams);
    if (status == FALSE)
    {
        printf("\nError to Get the Com state\n\n");
        goto exit;
    }

    dcbParams.BaudRate = CBR_38400;      //BaudRate = 9600
    dcbParams.ByteSize = 8;             //ByteSize = 8
    dcbParams.StopBits = TWOSTOPBITS;    //StopBits = 1
    dcbParams.Parity = NOPARITY;      //Parity = None
    status = SetCommState(hComm, &dcbParams);
    if (status == FALSE)
    {
        printf("\nError to Setting DCB Structure\n\n");
        goto exit;
    }

    //Read port
    //Setting Receive Mask
    // status = SetCommMask(hComm, EV_RXCHAR);
    // if (status == FALSE)
    // {
    //     printf("\nError to in Setting CommMask\n\n");
    //     goto exit;
    // }
    //Setting WaitComm() Event
    // status = WaitCommEvent(hComm, &dwEventMask, NULL); //Wait for the character to be received
    // if (status == FALSE)
    // {
    //     printf("\nError! in Setting WaitCommEvent()\n\n");
    //     goto exit;
    // }

    //Read data and store in a buffer
    status = ReadFile(hComm, &ReadData, sizeof(ReadData), &bytesRead, NULL);
    while(bytesRead > 0){
        buffer[i++] = ReadData;
        status = ReadFile(hComm, &ReadData, sizeof(ReadData), &bytesRead, NULL);
    }

    int j =0;
    for(j = 0; j < i; j++){
        printf("buff[%d]: %c\n",j,buffer[j]);
    }

    // do
    // {
    //     status = ReadFile(hComm, &ReadData, sizeof(ReadData), &bytesRead, NULL);
    //     buffer[i++] = ReadData;
    // }
    // while (bytesRead > 0);
    // --i; //Get Actual length of received data
    //printf("\nNumber of bytes received = %d\n\n", loop);
    //print receive data on console
    //printf("\n\n");
    // int j = 0;
    // for (j = 0; j < i; ++j)
    // {
    //     printf("buff[%d]: %c\n",j,buffer[j]);
    // }
    // printf("\n\n");
    uint8_t temp[3] = {0}, num = 0, buff_2[20] = {0};
    uint16_t idx = 0;
    
    for(i = 0 ; i < 32 ; i += 2){//15 bytes + 1
        temp[0] = buffer[i];
        temp[1] = buffer[i+1];
        temp[2] = '\0';

        num = string_to_hex(temp);
        buff_2[idx++] = num;

        printf("Hex val = %x",num);
    }

    printf("\n");
    printf("Converted buffer:\n");
    for(i = 0; i < sizeof(buff_2); i++)
    {
        printf("%x",buff_2[i]);
    }
    
    uint8_t decodedstring[200] = {0};

    //decoding
	decode_string(buff_2,(uint16_t) 109, decodedstring);

	printf("\n%s\r\n", decodedstring);

    verify_encoded_data(decodedstring);

exit:
    CloseHandle(hComm);//Closing the Serial Port

  return 0;
}

void verify_encoded_data(uint8_t decoded_string[])
{
    char data_sent[] = "entering a random strin";

    if(!strncmp(data_sent,decoded_string,strlen(data_sent))){
		printf("Encode = decode\r\n");
	}else
		printf("Fail!\r\n");
}



// ***********************************************************************************************
// * Name :            	string_to_hex
// * Description :     	Used to convert user string to hex
// * Inputs :			string to be converted
// * RETURN :			hex value of string
// ***********************************************************************************************
uint8_t string_to_hex(char array[])
{

    int i = 0,mult = 1, temp = 0;

    i = strlen(array)+1;
    while(i){

        if(array[i-1] <= 'F' && array[i-1] >= 'A'){
            temp += mult*(array[i-1] - 'A' + 10);
            mult *= 16;
        }else if(array[i-1] >= 'a' && array[i-1] <= 'f'){
            temp += mult*(array[i-1] - 'a'+ 10);
            mult *= 16;
        }
        else if(array[i-1]<0x3A && array[i-1]>0x2F){
            temp += mult*(array[i-1] - 0x30);
            mult *= 16;
        }
        i--;
    }

     return temp;
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