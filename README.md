# PES-Project
Contains code files of PES project

Contains two code folders:
1. PES-Project : The MCU-Xpresso project for the KL25Z side
2. HUffman folder- contains huffman tree generation code, huffman_tree_gen.c and the PC side decompression program-serial_read.c and the header of lookup table generated from input.txt file.

The lookup-table has been generated from Linux Syslog.

Uart parameters:Baud rate : 38400bps, Data bits : 8, Parity : N, Stop Bits : 2.

To generate the lookup tree for a given input, input the data in "input.txt". Then run the program by building huffman_tree_gen.c and output the executable data to huffman.h by the following command:
Suppose, the name of the executable is huffman.exe, command will be:
./huffman.exe > huffman.h.

# To run the actual compression and decompression program:
1.open the project PES-Project on MCU-Xpresso and enter in debugging.
2.Open the PC side program and edit the line number 46 with the actual COM port number assigned.
3.Build the PC side program serial_read.c
4.Run the PC side program and it should print 'Waiting for encoded input from target controller'.
5.At this point, run the KL-25Z program.
6.At this point, an output similar to 'snap_2' in the screenshots folder will be seen.
7.At times, the PC-side program may not run smoothly or on the first try, so reset the controller and 
execute again.
8.The encoded string hex-dump and the decoded input with all the test cases results will be displayed on the terminal.

NOTE:The project is compiled and tested on MCUXpressoIDE version 11.2.0_4120 running on Windows 10 and other C program have been build and tested on GCC version 11.2.0.

To run the project- Clone this repository in your local drive.Import the project in your workspace.Build and run debug in debug to execute the program.For the tree generation and the PC side serial_read.c program, a C compiler will be needed.