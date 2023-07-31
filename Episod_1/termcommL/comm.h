#pragma once

#include <string.h>
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>   //--- File Control Definitions          
#include <termios.h> //--- POSIX Terminal Control Definitions
#include <unistd.h>  //--- UNIX Standard Definitions 	   
#include <errno.h>   //--- ERROR Number Definitions       

#define EN_ADDINFO	0
//#define EN_PRINT	
#define PORT		"/dev/ttyUSB0"
#define LENBUFFER   256
#define LENPRIHOD	150

#define MAX_SVAL	17
#define MAX_SIMB	8

//--- Добавить в stArrPayval, генерировать в ардуино и гнать в порт 
typedef struct {
	uint32_t 	prt;
    uint32_t 	alarm;
}stPortVal;

typedef struct {
    float f0;
    float f1;
    float f2;
}payval;

typedef struct {
	payval pv;    
	payval Acc;
	payval Gir;
	payval Mag; 
    payval Bme;
}stArrPayval;

class clPort
{
public:

	clPort();
	~clPort();

    void Open();
	void SetAttributes();
	void Close();
    void Work(stArrPayval& dt, stPortVal& port);

	char read_buffer[LENBUFFER];    //--- Buffer to store the data received  

protected:

private:

	int fd;							//--- File Descriptor
    int CheckBuff(char *buf, int len);
	void StrToFloat(char ( &sv )[MAX_SVAL][MAX_SIMB], stArrPayval& dt);

	//--- Setting the Attributes of the serial port using termios structure 
	struct termios SerialPortSettings;
	struct termios oldSerialPortSettings;
};
