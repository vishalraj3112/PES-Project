#ifndef _HUFF_H_
#define _HUFF_H_


//prototypes of functions
int huffman_exe(void);
int encode_string(char *message, uint8_t *buffer, size_t nbytes);
void decode_string(uint8_t encoded_buffer[], uint16_t encoded_bits, uint8_t decoded_buffer[]);

#endif