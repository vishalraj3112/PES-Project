/***********************************************************************************************************************
* File Name    : huffman_tree_gen.c
* Project      : PES Final Project
* Version      : 1.0
* Description  : Contains the huffman tree and look-up table generation code for huffman algorithm.
* Author       : Vishal Raj & referred from https://www.geeksforgeeks.org/huffman-coding-greedy-algo-3/.
* Creation Date: 12.09.21
***********************************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
//#include "huffman_tree_gen.h" //using internally generated look-up table in this code
#include <assert.h>

//MACROS
#define TREE_HEIGHT     100   
#define TOTAL_CHARS     128           
#define MASK(x)         (0xFF >> (8-x))

int encoded_bits = 0;
  
//Huffman tree node
struct tree_node {
  
    //data at the node
    char data;
  
    // Frequency of the data
    unsigned freq;
  
    // Left and right child of this node
    struct tree_node *left, *right;
};
  
//collection of tree_nodes
struct Tree_heap {
  
    // Heap size
    unsigned size;
  
    // Heap capacity
    unsigned capacity;
  
    // array of tree_heap pointers
    struct tree_node** array;
};

//***tree generation struct
 typedef struct{
    uint8_t data;       //Data being coded
     uint32_t ccode;     //Character code for data
     int32_t no_bits;    //Number of bits in char code
     int32_t freq;       //frequency of data
 }huffman_table_t;

huffman_table_t huffman_table[TOTAL_CHARS] = {{.data = 'x', .ccode = 0, .no_bits = 0}};
//***tree generation struct

static void printFreq(int freq[],char arr[], int freq_new[]);
static int getFrequency(uint8_t string[]);
static int encode_string(char *message, uint8_t *buffer, size_t nbytes);
static void gen_huffman_header(void);
static void print_lut(void);
static void decode_string(uint8_t encoded_buffer[], uint16_t encoded_bits, uint8_t decoded_buffer[]);
static void store_data(char data, int arr[],int no_bits);
static int min(int a,int b);
static void test_huffman_encode_decode(void);

char encoded_string[100] = {0};


  

struct tree_node* gen_node(char data, unsigned freq)//l:231
{
    struct tree_node* temp = (struct tree_node*)malloc(sizeof(struct tree_node));
  
    temp->left = temp->right = NULL;
    temp->data = data;
    temp->freq = freq;
  
    return temp;
}
  

struct Tree_heap* createTree_heap(unsigned capacity)
  
{
    struct Tree_heap* new_heap = (struct Tree_heap*)malloc(sizeof(struct Tree_heap));
  
    new_heap->size = 0;
  
    new_heap->capacity = capacity;
  
    new_heap->array = (struct tree_node**)malloc(new_heap->capacity * sizeof(struct tree_node*));

    return new_heap;
}
  

void swap_nodes(struct tree_node** x,struct tree_node** y)
  
{
  
    struct tree_node* temp = *y;

    *y = *x;
    *x = temp;
}
  

void gen_heap(struct Tree_heap* heap, int idx)
  
{ 
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;
  
    if (left < heap->size && (heap->array[left]->freq < heap->array[smallest]->freq))
        smallest = left;
  
    if (right < heap->size && (heap->array[right]->freq < heap->array[smallest]->freq))
        smallest = right;
  
    if (smallest != idx) {
        swap_nodes(&heap->array[smallest],&heap->array[idx]);
        gen_heap(heap, smallest);
    }
}
  

int check_heap_one(struct Tree_heap* heap)//l:216
{
  
    return (heap->size == 1);
}
  

struct tree_node* get_min_node(struct Tree_heap* heap)//l:220,221
  
{
  
    struct tree_node* temp = heap->array[0];
    heap->array[0] = heap->array[heap->size - 1];
  
    --heap->size;
    gen_heap(heap, 0);
  
    return temp;
}
  

void insert_node(struct Tree_heap* heap, struct tree_node* node)//l:236
{
  
    ++heap->size;
    int i = heap->size - 1;
  
    while (i
           && node->freq
                  < heap->array[(i - 1) / 2]->freq) {
  
        heap->array[i] = heap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
  
    heap->array[i] = node;
}
  

void build_heap(struct Tree_heap* heap)
{
  
    int n = heap->size - 1;
    int i;
  
    for (i = (n - 1) / 2; i >= 0; --i)
        gen_heap(heap, i);
}

  

int isLeaf(struct tree_node* root)
  
{
  
    return !(root->left) && !(root->right);
}
  

struct Tree_heap* build_tree_heap(char data[], int freq[], int size)
  
{
  
    struct Tree_heap* heap = createTree_heap(size);
  
    for (int i = 0; i < size; ++i)
        heap->array[i] = gen_node(data[i], freq[i]);
  
    heap->size = size;
    build_heap(heap);
  
    return heap;
}
  

struct tree_node* gen_huffman_tree(char data[], int freq[], int size)
  
{
    struct tree_node *left, *right, *top;
  

    struct Tree_heap* heap = build_tree_heap(data, freq, size);
  
    // Continue to genrate until size of heap becomes one
    while (!check_heap_one(heap)) {
  
        left = get_min_node(heap);
        right = get_min_node(heap);
  
        top = gen_node('$', left->freq + right->freq);
  
        top->left = left;
        top->right = right;
  
        insert_node(heap, top);
    }
  

    return get_min_node(heap);
}
  

void store_char_codes(struct tree_node* root, int arr[], int top)
  
{
    //assign 0 to left node
    if (root->left) {
        arr[top] = 0;
        store_char_codes(root->left, arr, top + 1);
    }
  
    // assign 1 to right node
    if (root->right) {
        arr[top] = 1;
        store_char_codes(root->right, arr, top + 1);
    }
  
    /*if a node end is reached store the value in huffman
    lookup table*/
    if (isLeaf(root)) {
        huffman_table[root->data].data = root->data;
        huffman_table[root->data].no_bits = top;
        store_data(root->data,arr,top);
    }
}

void store_data(char data, int arr[],int no_bits)
{
    uint32_t temp = 0;
    for(int i = 0; i < no_bits; i++){
            if(arr[i] == 1)
                temp += (1 << (no_bits-1-i));
    }   
    huffman_table[data].ccode = temp;
}



int encode_string(char *message, uint8_t *buffer, size_t nbytes)
{
    if(*message == '\0') {
        printf("Error: Empty Message");
        return 0;
    }

    int min_idx = 0;
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
    if (bits_written > 0)
        min_idx = 1;

    return 8*buffer_idx + bits_written;
}
  

struct tree_node* gen_huffman_codes(char data[], int freq[], int size)
  
{
    // Generate tree
    struct tree_node* root = gen_huffman_tree(data, freq, size);
  
    int arr[TREE_HEIGHT], top = 0;
  
    store_char_codes(root, arr, top);

    return root;
}


int main()
{
    char * string;
    long files_bytes = 0;
    int ctr = 0;
    
    //***File read begins
    char * filename = "input2.txt";
    FILE *fp = fopen(filename,"r");

    if(fp == NULL){
        printf("Error reading file %s",filename);
        return 1;
    }

    /*Get no of bytes in file*/
    fseek(fp,0L,SEEK_END);
    files_bytes = ftell(fp);

    /*reset file pointer to start*/
    fseek(fp,0L,SEEK_SET);

    /* allocate sufficient memory*/
    string = (char*)calloc(files_bytes,sizeof(char));
    if(string == NULL){
        return 1;
    }

    fread(string,sizeof(char),files_bytes,fp);
    fclose(fp);

    //***File read ends

    int size = getFrequency(string);
    char arr[size];
    int freq[size];

    for (int i = 0; i < 128; i++) {
        if (huffman_table[i].freq > 0) {
            arr[ctr] = huffman_table[i].data;
            freq[ctr++] = huffman_table[i].freq;
        }
    }
  
    struct tree_node* root;

    root = gen_huffman_codes(arr, freq, size);//generating huffman tree here
    //----tree generation over here
    //print_lut();
    gen_huffman_header();

    test_huffman_encode_decode();

    free(string);
  
    return 0;
}

void test_huffman_encode_decode(void)
{
    uint8_t decodedstring[200] = {0};

    uint8_t data_to_encode[] = "Dec 11 04:05:45 vishal-Lenovo-ideapad-520S-14IKB rsyslogd:";
    uint8_t data_to_encode2[] = "Dec 11 04:09:45 vishal-Lenovo-ideapad-520S-14IKB org.gnome.Shell.desktop[2145]: #0 0x7ffed15c99d0 I   resource:///org/gnome/gjs/modules/";
    uint8_t data_to_encode3[] = "Entering a random string";
    uint8_t data_to_encode4[] = "test string 12345 RaNDommmmm cAsinggg";


    uint8_t buff[100];

    //test for input - 1
    uint16_t indexes = encode_string(data_to_encode, buff, sizeof(buff));//string encoded here
    //printf("index:%d",indexes );

    decode_string(buff, indexes, decodedstring);

    printf("\n%s\n", decodedstring);

    assert(!strncmp(data_to_encode,decodedstring,strlen(data_to_encode)));
    if(!strncmp(data_to_encode,decodedstring,strlen(data_to_encode))){
        printf("Encode = decode\r\n");
    }else
        printf("Fail test 1!\r\n");

    memset(buff, 0, sizeof(buff));
    memset(decodedstring,0,sizeof(decodedstring));

    //test for input - 2
    indexes = encode_string(data_to_encode2, buff, sizeof(buff));//string encoded here

    decode_string(buff, indexes, decodedstring);

    printf("\n%s\n", decodedstring);

    assert(!strncmp(data_to_encode2,decodedstring,strlen(data_to_encode2)));
    if(!strncmp(data_to_encode2,decodedstring,strlen(data_to_encode2))){
        printf("Encode = decode\r\n");
    }else
        printf("Fail test 2!\r\n");

    memset(buff, 0, sizeof(buff));
    memset(decodedstring,0,sizeof(decodedstring));

    //test for input - 3
    indexes = encode_string(data_to_encode3, buff, sizeof(buff));//string encoded here

    decode_string(buff, indexes, decodedstring);

    printf("\n%s\n", decodedstring);

    assert(!strncmp(data_to_encode3,decodedstring,strlen(data_to_encode3)));
    if(!strncmp(data_to_encode3,decodedstring,strlen(data_to_encode3))){
        printf("Encode = decode\r\n");
    }else
        printf("Fail test 3!\r\n");


    memset(buff, 0, sizeof(buff));
    memset(decodedstring,0,sizeof(decodedstring));

    //test for input - 4
    indexes = encode_string(data_to_encode4, buff, sizeof(buff));//string encoded here
    decode_string(buff, indexes, decodedstring);
    printf("\n%s\n", decodedstring);
    assert(!strncmp(data_to_encode4,decodedstring,strlen(data_to_encode4)));
 

    printf("\n");
    printf("All tests passed!\r\n");

}

void gen_huffman_header(void)
{
    printf("#ifndef _HUFFMAN_H_\n");
    printf("#define _HUFFMAN_H_\n\n");
    printf("#include<stdint.h>\n\n");
    printf("typedef struct {\n\tuint8_t data;\n\tuint32_t ccode;\n\tint32_t no_bits;\n\tint32_t freq;\n} huffman_table_t;\n\n");

    printf("huffman_table_t huffman_table[] = {\n");
    for (int i = 0; i < TOTAL_CHARS; i++) {
        if (i != TOTAL_CHARS-1)
            printf("{%d, 0x%02x, %d},\n", huffman_table[i].data, huffman_table[i].ccode, huffman_table[i].no_bits);//Check %02x !!
        else
            printf("{%d, 0x%02x, %d} };\n\n", huffman_table[i].data, huffman_table[i].ccode, huffman_table[i].no_bits);
    }

    printf("#endif");
}

void print_lut(void)
{
    printf("Huffman Lookup table below:\r\n");
    printf("huffman_table_t huffman_table[] = {\n");
    for (int i = 0; i < TOTAL_CHARS; i++) {
        if (i != TOTAL_CHARS-1)
            printf("{%d, 0x%02x, %d},\n", huffman_table[i].data, (huffman_table[i].ccode), huffman_table[i].no_bits);//Check %02x !!
        else
            printf("{%d, 0x%02x, %d} };\n\n", huffman_table[i].data, (huffman_table[i].ccode), huffman_table[i].no_bits);
    }
}

//Code for finding frequency of characters in c:

int getFrequency(uint8_t string[])
{
    int i = 0;
    int cnt = 0;

    while(string[i] != '\0'){
        huffman_table[string[i++]].freq++;
    }

    for (int i = 0; i < 128; i++) {
        huffman_table[i].data = i;
        if (huffman_table[i].freq > 0)
            cnt++;
    }

    return cnt;
}

void decode_string(uint8_t enc_buff[], uint16_t enc_bits, uint8_t dec_buff[])
{
    uint8_t *cc_buf = enc_buff;
    uint16_t enc_idx = 0, dec_idx = 0;
    uint16_t ccode = 0x00, ccode_len = 1;
    uint16_t i = 1, j = 0, curr_pos = 0;//indexes

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

int min(int a,int b)
{
    return (a <= b) ? a : b;
}