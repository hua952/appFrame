#include "main.h"
#include "msgTool.h"
#include "appMgr.h"
#include "rpcMgr.h"

int main (int argNum, char* argS[])
{
	int nRet = 0;
	tSingleton<msgTool>::createSingleton ();
	tSingleton<appMgr>::createSingleton ();
	tSingleton<rpcMgr>::createSingleton();
	tSingleton<msgTool>::single ().init (argNum, argS);
	return nRet;
}
