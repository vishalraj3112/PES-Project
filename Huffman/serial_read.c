#include <Windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

//change printf to printf

int main()
{
  HANDLE hComm;
  DCB dcbParams = { 0 };
  char buffer[64] = {0};
  DWORD bytesRead;
  DWORD dwEventMask;//change name
  int loop = 0;
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
  }else
      printf("opening serial port successful");

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
  status = SetCommMask(hComm, EV_RXCHAR);
  if (status == FALSE)
  {
      printf("\nError to in Setting CommMask\n\n");
      goto exit;
  }
  //Setting WaitComm() Event
  status = WaitCommEvent(hComm, &dwEventMask, NULL); //Wait for the character to be received
  if (status == FALSE)
  {
      printf("\nError! in Setting WaitCommEvent()\n\n");
      goto exit;
  }
  //Read data and store in a buffer
  do
  {
      status = ReadFile(hComm, &ReadData, sizeof(ReadData), &bytesRead, NULL);
      buffer[loop] = ReadData;
      ++loop;
  }
  while (bytesRead > 0);
  --loop; //Get Actual length of received data
  printf("\nNumber of bytes received = %d\n\n", loop);
  //print receive data on console
  printf("\n\n");
  int index = 0;
  for (index = 0; index < loop; ++index)
  {
      printf("%c", buffer[index]);
  }
  printf("\n\n");

exit:
    CloseHandle(hComm);//Closing the Serial Port

  return 0;
}