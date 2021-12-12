// C program for Huffman Coding
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
//#include <math.h>  
#include "huffman.h"

// This constant can be avoided by explicitly
// calculating height of Huffman Tree
#define MAX_TREE_HT 100
#define MAX_CHAR    128
#define MASK(x)     (0xFF >> (8-x))

int encoded_bits = 0;
  
// A Huffman tree node
struct MinHeapNode {
  
    // One of the input characters
    char data;
  
    // Frequency of the character
    unsigned freq;
  
    // Left and right child of this node
    struct MinHeapNode *left, *right;
};
  
// A Min Heap:  Collection of
// min-heap (or Huffman tree) nodes
struct MinHeap {
  
    // Current size of min heap
    unsigned size;
  
    // capacity of min heap
    unsigned capacity;
  
    // Array of minheap node pointers
    struct MinHeapNode** array;
};

// typedef struct{
//     uint8_t data;       //Data being coded
//     uint32_t ccode;     //Character code for data
//     int32_t no_bits;    //Number of bits in char code
//     int32_t freq;       //frequency of data
// }huffman_table_t;

// huffman_table_t huffman_table[MAX_CHAR] = {{.data = 'x', .ccode = 0, .no_bits = 0}};

static void printFreq(int freq[],char arr[], int freq_new[]);
static int getFrequency(uint8_t string[]);
static void decode_file(struct MinHeapNode* root, uint8_t *s, uint8_t *decode);
static int encode_string(char *message, uint8_t *buffer, size_t nbytes);
static void gen_huffman_header(void);
static void print_lut(void);
static void decode_string(uint8_t encoded_buffer[], uint16_t encoded_bits, uint8_t decoded_buffer[]);
static void store_data(char data, int arr[],int no_bits);
static int min(int a,int b);

char encoded_string[100] = {0};


  
// A utility function allocate a new
// min heap node with given character
// and frequency of the character
struct MinHeapNode* newNode(char data, unsigned freq)//l:231
{
    struct MinHeapNode* temp = (struct MinHeapNode*)malloc(
        sizeof(struct MinHeapNode));
  
    temp->left = temp->right = NULL;
    temp->data = data;
    temp->freq = freq;
  
    return temp;
}
  
// A utility function to create
// a min heap of given capacity
struct MinHeap* createMinHeap(unsigned capacity)
  
{
  
    struct MinHeap* minHeap
        = (struct MinHeap*)malloc(sizeof(struct MinHeap));
  
    // current size is 0
    minHeap->size = 0;
  
    minHeap->capacity = capacity;
  
    minHeap->array = (struct MinHeapNode**)malloc(
        minHeap->capacity * sizeof(struct MinHeapNode*));
    return minHeap;
}
  
// A utility function to
// swap two min heap nodes
void swapMinHeapNode(struct MinHeapNode** a,
                     struct MinHeapNode** b)
  
{
  
    struct MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}
  
// The standard minHeapify function.
void minHeapify(struct MinHeap* minHeap, int idx)
  
{
  
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;
  
    if (left < minHeap->size
        && minHeap->array[left]->freq
               < minHeap->array[smallest]->freq)
        smallest = left;
  
    if (right < minHeap->size
        && minHeap->array[right]->freq
               < minHeap->array[smallest]->freq)
        smallest = right;
  
    if (smallest != idx) {
        swapMinHeapNode(&minHeap->array[smallest],
                        &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}
  
// A utility function to check
// if size of heap is 1 or not
int isSizeOne(struct MinHeap* minHeap)//l:216
{
  
    return (minHeap->size == 1);
}
  
// A standard function to extract
// minimum value node from heap
struct MinHeapNode* extractMin(struct MinHeap* minHeap)//l:220,221
  
{
  
    struct MinHeapNode* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
  
    --minHeap->size;
    minHeapify(minHeap, 0);
  
    return temp;
}
  
// A utility function to insert
// a new node to Min Heap
void insertMinHeap(struct MinHeap* minHeap,
                   struct MinHeapNode* minHeapNode)//l:236
  
{
  
    ++minHeap->size;
    int i = minHeap->size - 1;
  
    while (i
           && minHeapNode->freq
                  < minHeap->array[(i - 1) / 2]->freq) {
  
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
  
    minHeap->array[i] = minHeapNode;
}
  
// A standard function to build min heap
void buildMinHeap(struct MinHeap* minHeap)
  
{
  
    int n = minHeap->size - 1;
    int i;
  
    for (i = (n - 1) / 2; i >= 0; --i)
        minHeapify(minHeap, i);
}
  
// A utility function to print an array of size n
void printArr(int arr[], int n)
{
    int i;
    for (i = 0; i < n; ++i)
        printf("%d", arr[i]);
  
    printf("\n");
}
  
// Utility function to check if this node is leaf
int isLeaf(struct MinHeapNode* root)
  
{
  
    return !(root->left) && !(root->right);
}
  
// Creates a min heap of capacity
// equal to size and inserts all character of
// data[] in min heap. Initially size of
// min heap is equal to capacity
struct MinHeap* createAndBuildMinHeap(char data[],
                                      int freq[], int size)
  
{
  
    struct MinHeap* minHeap = createMinHeap(size);
  
    for (int i = 0; i < size; ++i)
        minHeap->array[i] = newNode(data[i], freq[i]);
  
    minHeap->size = size;
    buildMinHeap(minHeap);
  
    return minHeap;
}
  
// The main function that builds Huffman tree
struct MinHeapNode* buildHuffmanTree(char data[],
                                     int freq[], int size)
  
{
    struct MinHeapNode *left, *right, *top;
  
    // Step 1: Create a min heap of capacity
    // equal to size.  Initially, there are
    // modes equal to size.
    struct MinHeap* minHeap
        = createAndBuildMinHeap(data, freq, size);
  
    // Iterate while size of heap doesn't become 1
    while (!isSizeOne(minHeap)) {
  
        // Step 2: Extract the two minimum
        // freq items from min heap
        left = extractMin(minHeap);
        right = extractMin(minHeap);
  
        // Step 3:  Create a new internal
        // node with frequency equal to the
        // sum of the two nodes frequencies.
        // Make the two extracted node as
        // left and right children of this new node.
        // Add this node to the min heap
        // '$' is a special value for internal nodes, not
        // used
        top = newNode('$', left->freq + right->freq);
  
        top->left = left;
        top->right = right;
  
        insertMinHeap(minHeap, top);
    }
  
    // Step 4: The remaining node is the
    // root node and the tree is complete.
    return extractMin(minHeap);
}
  
// Prints huffman codes from the root of Huffman Tree.
// It uses arr[] to store codes
void printCodes(struct MinHeapNode* root, int arr[],
                int top)
  
{
    // Assign 0 to left edge and recur
    if (root->left) {
        arr[top] = 0;
        printCodes(root->left, arr, top + 1);
    }
  
    // Assign 1 to right edge and recur
    if (root->right) {
  
        arr[top] = 1;
        printCodes(root->right, arr, top + 1);
    }
  
    // If this is a leaf node, then
    // it contains one of the input
    // characters, print the character
    // and its code from arr[]
    if (isLeaf(root)) {
  
        //printf("%c: ", root->data);
        //printArr(arr, top);
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
                temp += (1<< (no_bits-1-i));
                //huffman_table[data].ccode += (1<< (no_bits-1-i));
    }  
    //printf("temp:%d ",temp);  
    huffman_table[data].ccode = temp;
    // for(int i = 0; i < no_bits; i++){
    //     if(arr[i] == 1)
    //         temp += (1<< (i));
    // }
    // printf("temp:%d ",temp);  
    // huffman_table[data].ccode = temp;
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
  
// The main function that builds a
// Huffman Tree and print codes by traversing
// the built Huffman Tree
struct MinHeapNode* HuffmanCodes(char data[], int freq[], int size)
  
{
    // Construct Huffman Tree
    struct MinHeapNode* root
        = buildHuffmanTree(data, freq, size);
  
    // Print Huffman codes using
    // the Huffman tree built above
    int arr[MAX_TREE_HT], top = 0;
  
    printCodes(root, arr, top);

    return root;
}


int main()
{
    uint8_t decodedstring[200] = {0};
    //uint8_t string[256] = "";
    char * string;
    uint8_t data_to_encode[] = "Just gonna stand there and watch me burn? Well, that's alright, because I like the way it hurts Just gonna stand there and hear me cry?";
    long files_bytes = 0;
    int ctr = 0;
    
    //***File read
    char * filename = "input.txt";
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
    printf("File data:");
    printf("%s\r\n",string);

    /*free allocated memory*/
    //free(string);

    // const uint32_t MAX_LEN = 256;
    // char buffer[MAX_LEN];

    // printf("File data:");
    // while(fgets(string,MAX_LEN,fp));
    //     printf("%s\r\n",string);

    // fclose(fp);
    //***File read

    int size = getFrequency(string);
    char arr[size];
    int freq[size];

    for (int i = 0; i < 128; i++) {
        if (huffman_table[i].freq > 0) {
            arr[ctr] = huffman_table[i].data;
            freq[ctr++] = huffman_table[i].freq;
        }
    }
  
    struct MinHeapNode* root;

    root = HuffmanCodes(arr, freq, size);//generating huffman tree here
    //----tree generation over here
    print_lut();
    //gen_huffman_header();

    uint8_t buff[1024];

    uint16_t indexes = encode_string(data_to_encode, buff, sizeof(buff));//string encoded here

    //decode here itself
    //decode_file(root, buff, decodedstring);
    decode_string(buff, indexes, decodedstring);

    printf("\n%s\n", decodedstring);

    if(!strncmp(data_to_encode,decodedstring,strlen(data_to_encode))){
        printf("Encode = decode\r\n");
    }else
        printf("Fail!");

    free(string);
  
    return 0;
}

void gen_huffman_header(void)
{
    printf("#ifndef _HUFFMAN_H_\n");
    printf("#define _HUFFMAN_H_\n\n");
    printf("#include<stdint.h>\n\n");
    printf("typedef struct {\n\tuint8_t data;\n\tuint32_t ccode;\n\tint32_t no_bits;\n\tint32_t freq;\n} huffman_table_t;\n\n");

    printf("huffman_table_t huffman_table[] = {\n");
    for (int i = 0; i < MAX_CHAR; i++) {
        if (i != MAX_CHAR-1)
            printf("{%d, 0x%02x, %d},\n", huffman_table[i].data, huffman_table[i].ccode, huffman_table[i].no_bits);//Check %02x !!
        else
            printf("{%d, 0x%02x, %d} };\n\n", huffman_table[i].data, huffman_table[i].ccode, huffman_table[i].no_bits);
    }

    printf("#endif");
}

void print_lut(void)
{
    printf("huffman_table_t huffman_table[] = {\n");
    for (int i = 0; i < MAX_CHAR; i++) {
        if (i != MAX_CHAR-1)
            printf("{%d, 0x%02x, %d},\n", huffman_table[i].data, (huffman_table[i].ccode>>1), huffman_table[i].no_bits);//Check %02x !!
        else
            printf("{%d, 0x%02x, %d} };\n\n", huffman_table[i].data, (huffman_table[i].ccode>>1), huffman_table[i].no_bits);
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

void decode_file(struct MinHeapNode* root, uint8_t *s, uint8_t *decode)
{
    int j = 0;
    int decoded_bits = 0;
    struct MinHeapNode* curr = root;

    // int temp = (encoded_bits & 7);
    // printf("Encoded_bits %d\n", (encoded_bits >> 3 + 1));
    for (int i = 0; i < (encoded_bits >> 3) + 1; i++)
    {
        int bit = 8;
        while(bit){
            if ((s[i] & 0x80) == 0)
            //if (s[i] == '0')
                curr = curr->left;
            else
                curr = curr->right;
            s[i] <<= 1;
            bit--;
            
            // reached leaf node
            if (curr->left==NULL && curr->right==NULL)
            {
                decode[j++] = curr->data;
                //printf("%c", curr->data);
                curr = root;
            }

            decoded_bits++;
            if (decoded_bits == encoded_bits)
                break;
        }
    }
    decode[j]='\0';
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