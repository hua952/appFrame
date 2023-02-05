#include<stdlib.h>
#include<string.h>
#include"strFun.h"

void strNCpy(char* szD, int nSize, const char* szS)
{
    strncpy(szD, szS, nSize);
    szD[nSize - 1] = 0;
}

void strCpy (const char* szSrc, std::unique_ptr<char[]>& pDec)
{
	auto len = strlen (szSrc);
	pDec = std::make_unique<char []> (len + 1);
	auto p = pDec.get();
	strNCpy (p, len + 1, szSrc);
}

int strR(char* szText, char sp, char** pBuf, int BufNum)
{
	if(NULL == szText || NULL == pBuf || 0 == BufNum)
	{
		return 0;
	}
	int nRet = 0;
	char* szB = szText;
	char* szC = szText;
	while(*szC)
	{
		if(*szC == sp)
		{
			if(szC != szB)
			{
				pBuf[nRet++] = szB;
				if(nRet >= BufNum)
				{
					return nRet;
				}
			}
			*szC = 0;
			szB = szC+1;
		}
		szC++;
	}
	if(szC != szB && nRet < BufNum)
	{
		pBuf[nRet++] = szB;
	}
	return nRet;
}

int strRS(char* szText, char* szP, char** pBuf, int BufNum)
{
	if(NULL == szText || NULL == szP || NULL == pBuf || 0 == BufNum)
	{
		return 0;
	}
	int nPL = strlen(szP);
	int nRet = 0;
	char* szB = szText;
	char* szC = strstr(szText, szP);
	while(szC)
	{
		if(szB != szC)
		{
			pBuf[nRet++] = szB;
			if(nRet >= BufNum)
			{
				return nRet;
			}
		}
		
		*szC = 0;
		szC += nPL;
		szB = szC;
		szC = strstr(szC, szP);
	}
	if(*szB && nRet < BufNum)
	{
		pBuf[nRet++] = szB;
	}
	return nRet;
}
