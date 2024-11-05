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
	if (szSrc) {
		auto len = strlen (szSrc);
		pDec = std::make_unique<char []> (len + 1);
		auto p = pDec.get();
		strNCpy (p, len + 1, szSrc);
	}
}

void strCpy (const wchar_t* szSrc, std::unique_ptr<wchar_t[]>& pDec)
{
	wstrCpy (szSrc, pDec);
}

void wstrCpy (const wchar_t* szSrc, std::unique_ptr<wchar_t[]>& pDec)
{
	if (szSrc) {
		auto len = wcslen(szSrc);
		pDec = std::make_unique<wchar_t[]> (len + 1);
		auto p = pDec.get();
		wcsNCpy(p, len + 1, szSrc);
	}
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

void toWord (char* szWord)
{
	if ('a' <= *szWord && *szWord <= 'z') {
		*szWord -= 32;
	}
}

std::vector<std::string> stringSplit (const char* src, const char delim, bool trim)
{
	std::stringstream ss(src);
	return stringSplit (ss, delim, trim);
}

std::vector<std::string> stringSplit (std::istream& src, const char delim, bool trim)
{
	std::vector<std::string> ret;
	std::string token;
	while (getline(src, token, delim))
	{
		if (trim) {
			std::stringstream ss(token);
			ss>>token;
		}
		ret.push_back(token);
	}
	return ret;
}

template<> void stringToValue<bool>(const char* str,  bool& v)
{
	{std::stringstream s2(str);int a;s2>>a;v=a;}
}

template<> void stringToValue<unsigned char>(const char* str, unsigned char& v)
{
	{std::stringstream s2(str);int a;s2>>a;v=(unsigned char)a;}
}
