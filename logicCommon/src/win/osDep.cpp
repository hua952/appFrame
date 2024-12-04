#include <windows.h>
#include <io.h>
#include <cstring>
#include <memory>
#include <string>
#include <objbase.h>

int  createUuid (char* outPut)
{
	int nRet = 0;
	do
	{
		GUID& guid = *((GUID*)(&outPut[0]));
		CoCreateGuid(&guid);
	} while (0);
	return nRet;
}

