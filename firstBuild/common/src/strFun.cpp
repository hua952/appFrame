#include<stdlib.h>
#include<string.h>
#include"strFun.h"
extern "C"
{
void strNCpy(char* szD, int nSize, const char* szS)
{
    strncpy(szD, szS, nSize);
    szD[nSize - 1] = 0;
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

void stringToValue(const char* str,  bool& v)
{
	{std::stringstream s2(str);int a;s2>>a;v=a;}
}

void stringToValue(const char* str, unsigned char& v)
{
	{std::stringstream s2(str);int a;s2>>a;v=(unsigned char)a;}
}

void getStringValuesFromArgS (int argC, char** argS, void** keyValues, int keyNum)
{
	for (decltype (argC) i = 0; i < argC; i++) {
		auto ret = stringSplit (argS[i], '=');
		if (ret.size() != 2) {
			continue;
		}
		auto key = (const char*)(keyValues[i * 2]);
		auto value = (std::unique_ptr <char[]>*)(keyValues[i*2 + 1]);
		for (decltype (keyNum) j = 0; j < keyNum; j++) {
			if (ret[0] == key) {
				strCpy(ret[1].c_str(), *value);
				break;
			}
		}
	}
}

void repChar (char* szSrc, const char cFrom, const char cTo)
{
	auto pCur = szSrc;
	while (*pCur) {
		if (cFrom == *pCur) {
			*pCur = cTo;
		}
		pCur++;
	}
}

void repwChar (wchar_t* szSrc, const wchar_t cFrom, const wchar_t cTo)
{
	auto pCur = szSrc;
	while (*pCur) {
		if (cFrom == *pCur) {
			*pCur = cTo;
		}
		pCur++;
	}
}
