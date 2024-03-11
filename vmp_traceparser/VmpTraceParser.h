#pragma once
#include <windows.h>
#include "DataContainer.h"

#define VMPTRACEPARSER_FATALERROR -1
#define VMPTRACEPARSER_FILEOPENERROR -2
#define VMPTRACEPARSER_ALLOCATEERROR -3

class VmpTraceParser
{
private:
	HANDLE OpenFile(wchar_t* strFileName, int flag);
	int ReadData(HANDLE hHandle, DataContainer* strBuf, DataContainer* objBefore);
	int WriteData(HANDLE hHandle, DataContainer* objBuf);
	int SearchStringsOffset(DataContainer*  objBufferAddr, int *intOffset, const char* strSearchStrings);
	int SearchStringsInRow(DataContainer* objBufferAddr, int intOffset, const char* strSearchStrings);
	int HexCharCheck(unsigned char cChar);
	long long HexToParam(DataContainer* objData, int* intOffset);
	long long HexCharToParam(unsigned char cChar);
	int CalAddress(DataContainer* objData);

public:
	VmpTraceParser();
	~VmpTraceParser();

	int ParseFunctionLog(wchar_t* strDataFileName, wchar_t* strOutFileName);
};

