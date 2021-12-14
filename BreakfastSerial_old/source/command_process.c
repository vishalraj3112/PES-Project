/***********************************************************************************************************************
* File Name    : command_process.c
* Project      : PES Final Project
* Version      : 2.0
* Description  : Contains all the function implementation code along with variable definitions for command processor
* Author       : Vishal Raj & reference from Howdy Pierce Lec 22 slide
* Creation Date: 12.12.21
***********************************************************************************************************************/
#include "command_process.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "hexdump.h"
#include "huff.h"

//handler prototypes
static void author_handler(int, char * argv[]);
static void dump_handler(int, char * argv[]);
static void help_handler(int ,char * argv[]);
static void info_handler(int ,char * argv[]);
static bool valid_arg(char *c, int len);
static void huffman_handler(int, char * argv[]);

//variable definitions
typedef void (*cmd_handler_t)(int, char * argv[]);
static char *Author = "Vishal Raj";

typedef struct{
	const char *cmd_name;
	cmd_handler_t handler;
	const char * help_string;
}cmd_table_t;

//Add new command parameters here...
static const cmd_table_t commands[] = {
		//command table entries here
		{"author",author_handler,"Print the author of this code\r\n"},
		{"dump",dump_handler,"Output a hexdump starting at location loc.\r\n\tAlways specify loc in hex; specify len in hex (\"0XNN\") or dec (\"NN\")\r\n"},
		{"help",help_handler,"Print this help message\r\n"},
		{"info",info_handler,"Print various build info\r\n"},
		{"huffman",huffman_handler,"Executes the huffman function"}
};

static const int cmd_nos = sizeof(commands) / sizeof(cmd_table_t);
/***********************************************************************************************
* Name			   : accumulator
* Description 	   : used to accumulate input line from user.
* Parameters 	   : None
* RETURN 		   : None
***********************************************************************************************/
uint8_t* accumulator(uint8_t user_input[])
{
	uint8_t c=0;
	uint8_t ip_idx = 0;

	//receive
	while((c != '\r' ) && (c != '\n')){
		c = getchar();
		if(c == '\b' && (ip_idx)){
			ip_idx--;
			printf("\b \b");
		}else{
			sprintf((char *) user_input+ip_idx,(char *) &c);//try putchar
			ip_idx++;
			printf("%c",(char)c);
		}
	}
	printf("\r\n");
	ip_idx = 0;
	c = 0;//reset for next cycle
	return user_input;
}
/***********************************************************************************************
* Name			   : process_cmd
* Description 	   : Processes user input in seperate words
* Parameters 	   : User input line to be processed
* RETURN 		   : None
***********************************************************************************************/
void process_cmd(char *input)
{
	bool token = false;
	char *argv[10];
	int argc = 0;

	char *p = input;
	char *end;

	uint8_t valid_cmd = 0;

	//find the end of string
	for(end=input ; *end!= '\0' ; end++)
		;
	memset(argv,0,sizeof(argv));

	//lexical analysis
	for(p = input; p < end; p++){

		switch(token){

		case false:{
			if(((*p>='a')&&(*p<='z')) || ((*p>='A')&&(*p<='Z')) || ((*p>='0')&&(*p<='9'))){
				argv[argc] = p;
				argc++;
				token = true;
			}
		}
		break;
		case true:{
			if((*p == ' ') || (*p == '\t') || (*p == '\r')){//space found
				*p = '\0';
				token = false;
			}
			if(p == end-3){//if last character received,ignoring \r&\n
				*p = '\0';
				token = false;
			}
		}
		break;
		}
	}
	if(argc == 0){//no command found
		printf("No command found!\r\n");
		return;
	}

	//Todo:Handle the command
	for (int i=0; i < cmd_nos; i++) {
	    if (strcasecmp(argv[0], commands[i].cmd_name) == 0) {
	      commands[i].handler(argc, argv);
	      valid_cmd = 1;
	      break;
	    }
	  }

	//print error message if no proper command received from user
	if(!valid_cmd)
		printf("Unknown command: %s\r\n",argv[0]);

}
/***********************************************************************************************
* Name			   : author_handler
* Description 	   : author handler function
* Parameters 	   : argc, argv
* RETURN 		   : None
***********************************************************************************************/
void author_handler(int a, char * argv[])
{
	//printf("Vishal Raj\r\n");
	printf("%s\r\n",Author);
}

/***********************************************************************************************
* Name			   : dump_handler
* Description 	   : dump handler function
* Parameters 	   : argc, argv
* RETURN 		   : None
***********************************************************************************************/
void dump_handler(int a, char * argv[])
{
	uint32_t len = 0;
	uint32_t start = 0;

	if((a > 3) || (a < 3)){//argc
		printf("Invalid no of arguments!\r\n");
		printf("Enter two arguments- start, length\r\n");
		return;
	}

	for(uint8_t i = 1 ; i < 3 ; i++){
		if(!valid_arg(argv[i],strlen(argv[i])))
			return;
	}

	sscanf(argv[1],"%x",&start);

	if(strstr((char*)argv[2],"0x") || strstr((char*)argv[2],"0X"))//if input is in hex
		sscanf(argv[2],"%x",&len);
	else
		sscanf(argv[2],"%d",&len);

	if(len>640){
		printf("Length out of range.Enter length between 1 to 640 or 0x01 to 0x280\r\n");
		return;
	}

	hexdump(start,len);
}
/***********************************************************************************************
* Name			   : valid_arg
* Description 	   : used to check valid argument
* Parameters 	   : argv, length of argv
* RETURN 		   : valid argument or nor
***********************************************************************************************/
bool valid_arg(char *c, int len){

	for(int i = 0; i < len ; i++){
		if((c[i] < '0' || c[i] > '9') && (c[i] < 'A' || c[i] > 'F') &&
				(c[i] < 'a' || c[i] > 'f') && (c[i] != 'x' && c[i] != 'X'))
		{
			printf("Invalid argument\r\n");
			return false;
		}
	}
	return true;
}
/***********************************************************************************************
* Name			   : help_handler
* Description 	   : help handler function
* Parameters 	   : argc, argv
* RETURN 		   : None
***********************************************************************************************/
void help_handler(int argc,char * argv[])
{
	printf("author\r\n\t");
	printf("%s",commands[0].help_string);
	printf("dump <loc><len>\r\n\t");
	printf("%s",commands[1].help_string);
	printf("help\r\n\t");
	printf("%s",commands[2].help_string);
	printf("info\r\n\t");
	printf("%s",commands[3].help_string);
}
/***********************************************************************************************
* Name			   : info_handler
* Description 	   : info handler function
* Parameters 	   : argc, argv
* RETURN 		   : None
***********************************************************************************************/
void info_handler(int argc,char * argv[])
{
	printf("%s ", VERSION_TAG);
	printf("built on %s ", VERSION_BUILD_MACHINE);
	printf("at %s\n\r", VERSION_BUILD_DATE);
	printf("Commit %s\n\r", VERSION_BUILD_INFO);
}

void huffman_handler(int argc, char * argv[])
{
	printf("Huffman case\r\n");
	exe_huffman_task();
}
/*[EOF]*/
