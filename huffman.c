// C program for Huffman Coding
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>  
//#include "huffman.h"

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

typedef struct{
    uint8_t data;       //Data being coded
    uint32_t ccode;     //Character code for data
    int32_t no_bits;    //Number of bits in char code
    int32_t freq;       //frequency of data
}huffman_table_t;

huffman_table_t huffman_table[MAX_CHAR] = {{.data = 'x', .ccode = 0, .no_bits = 0}};

static void printFreq(int freq[],char arr[], int freq_new[]);
static int getFrequency(uint8_t string[]);
static void decode_file(struct MinHeapNode* root, uint8_t *s, uint8_t *decode);
static int encode_string(char *message, uint8_t *buffer, size_t nbytes);
static void gen_huffman_header(void);
static void print_lut(void);
static void decode_string(uint8_t encoded_buffer[], uint8_t encoded_bits, uint8_t decoded_buffer[]);


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
int cc_arr[10][MAX_TREE_HT] = {{0},{0}};

void printCodes(struct MinHeapNode* root, int arr[],
                int top)
  
{
    static int cc_no = 0, idx = 0;

    // Assign 0 to left edge and recur
    if (root->left) {
  
        arr[top] = 0;
        cc_arr[cc_no][idx] = 0;
        idx++;
        printCodes(root->left, arr, top + 1);
    }
  
    // Assign 1 to right edge and recur
    if (root->right) {
  
        arr[top] = 1;
        cc_arr[cc_no][idx] = 1;
        idx++;
        printCodes(root->right, arr, top + 1);
    }
  
    // If this is a leaf node, then
    // it contains one of the input
    // characters, print the character
    // and its code from arr[]
    if (isLeaf(root)) {
  
        printf("%c: ", root->data);
        printArr(arr, top);
        huffman_table[root->data].data = root->data;
        huffman_table[root->data].no_bits = top;
        for (int i = 0; i < top; i++) {
            if (arr[i] == 1) {
                huffman_table[root->data].ccode += 1*(arr[i]<<(top-1-i));
            }       
        }
        //huffman_table[cc_no].ccode = arr;//should use strcat;
        cc_no++;
    }


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
            int this_write = fmin(8 - bits_written, no_bits);

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

    return buffer_idx + min_idx;
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
    uint8_t decodedstring[100] = {0};
    uint8_t string[] = "oh my god daddy just dont be my baddy";
    uint8_t data_to_encode[] = "oh my god daddy just dont be my baddy";
    int ctr = 0;
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

    print_lut();
    //gen_huffman_header();

    uint8_t buff[1024];

    int indexes = encode_string(data_to_encode, buff, sizeof(buff));//string encoded here

    //decode here itself
    //decode_file(root, buff, decodedstring);
    decode_string(buff, encoded_bits, decodedstring);

    printf("\n%s\n", decodedstring);

    if(!strncmp(data_to_encode,decodedstring,strlen(data_to_encode))){
        printf("Encode = decode\r\n");
    }else
        printf("Fail!");

  
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


void decode_string(uint8_t encoded_buffer[], uint8_t encoded_bits, uint8_t decoded_buffer[])
{
    uint8_t *cc_buf = encoded_buffer;
    uint8_t enc_idx = 0, dec_idx = 0;
    uint8_t ccode = 0x00, ccode_len = 1;
    uint8_t i = 1, j = 0, curr_pos = 0;//indexes

    while(encoded_bits != 0){

        ccode |= (cc_buf[enc_idx] & 0x80) >> 7;//read the char codes in reverse
    

        while(huffman_table[i].data != '\0'){//till end of LUT is not reached
            if(ccode_len == huffman_table[i].no_bits){//length match
                if(ccode == huffman_table[i].ccode){//ccode match
                    decoded_buffer[dec_idx++] = huffman_table[i].data;
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

        encoded_bits--;

    }
    decoded_buffer[dec_idx] = '\0';
}


// void decode_string(uint8_t encoded_buffer[], uint8_t encoded_bits, uint8_t decoded_buffer[])
// {
//     printf("encoded bits = %d\n", encoded_bits);
//     //huffman_code_t *huffman_code;
//     //huffman_code= huff_code;
//     uint8_t *buf = encoded_buffer;
//     printf("encoded bits = %d\n", encoded_bits);
//     uint8_t ebuf_id = 0, dbuf_id = 0;
//     uint8_t code1 = 0, code_length = 1;
//     uint8_t i = 1, j = 0, bit_pos=0;
    
//     printf("buf = %p, encoded_buffer = %p\n", buf, encoded_buffer);
    
//     while(encoded_bits != 0)
//     {
//         // printf("hell");
//         // printf("lklj\n");
//         code1 |= (buf[ebuf_id] & 0x80) >> 7;
//         // printf("logic = %X\n",buf[ebuf_id]);
//         // if(buf[ebuf_id] & 0x80)
//         // {
//         //  code1 |= 0x01;
//         // }

//         //stuck 
//         //printf("hello");
//         printf("code = %d, code_length = %d\n", code1, code_length);

//         printf("data:%d",huffman_table[i].data);
//         //replace \0 with end symbol
//         while(huffman_table[i].data != '\0')
//         {
//             printf("Checking with symbol %c\n",huffman_table[i].data );
//             if(code_length == huffman_table[i].no_bits)
//             {
//                 printf("length matched at %d\n", code_length);
//                 if(code1 == huffman_table[i].ccode)
//                 {
//                     printf("matched code = %d\n", code1);
//                     printf("symbol = %c\n", huffman_table[i].data);
//                     decoded_buffer[dbuf_id++] = huffman_table[i].data;
//                     code1 = 0;
//                     code_length = 0;
//                     break;
//                 }
//             }
//             i++;
//         }
//         i=1;
//         code_length++;
//         code1 <<= 1;
//         buf[ebuf_id] <<= 1;
//         bit_pos++;
//         if(bit_pos == 8)
//         {
//             bit_pos = 0;
//             ebuf_id++;
//         }
//         encoded_bits--;
//         printf("\n");
        
//     }
//     decoded_buffer[dbuf_id]= '\0';
//     printf("%s\n", decoded_buffer);
// }