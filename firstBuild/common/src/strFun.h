#ifndef _str_fun_h__
#define _str_fun_h__
#include <memory>
#include <string>
#include <sstream>
#include <vector>
#include <cstring>
extern "C"
{
void strNCpy(char* szD, int nSize, const char* szS);
void wcsNCpy(wchar_t* szD, int nSize, const wchar_t* szS);
int strR(char* szTex, char sp, char** pBuf, int BufNum);
int strRS(char* szText, char* szP, char** pBuf, int BufNum);
int strCaseCmp (const char* str1, const char* str2);
void toWord (char* szWord);
}

std::vector<std::string> stringSplit (const char* src, const char delim, bool trim = true);
std::vector<std::string> stringSplit (std::istream& src, const char delim, bool trim = true);
void myW2U8(const wchar_t* wcText, std::unique_ptr<char[]>& var);
void myA2W(const char* szText, std::unique_ptr<wchar_t[]>& var);
void myU8ToW(const char* szText, std::unique_ptr<wchar_t[]>& var);
void strCpy (const char* szSrc, std::unique_ptr<char[]>& pDec);
void strCpy (const wchar_t* szSrc, std::unique_ptr<wchar_t[]>& pDec);
void wstrCpy (const wchar_t* szSrc, std::unique_ptr<wchar_t[]>& pDec);
void repChar (char* szSrc, const char cFrom, const char cTo);
void repwChar (wchar_t* szSrc, const wchar_t cFrom, const wchar_t cTo);

template<class T> void stringToValue(const char* str,  T& v)
{
	std::stringstream ss(str);
	ss>>v;
}

void stringToValue(const char* str,  bool& v);
void stringToValue(const char* str, unsigned char& v);

template<class T2> bool stringMatchValue(const char* srcKey, const char* srcValue, const char* key, T2& v)
{
	bool bRet = false;
	if (strcmp(srcKey, key) == 0) {
		stringToValue(srcValue, v);
		bRet = true;
	}
	return bRet;
}

template<class T1> bool getValueFromArgKV(const char* szArgKV, const char* szKey, T1& retValue, const char delim = '=')
{
	bool bRet = false;
	auto ret = stringSplit (szArgKV, delim, false);
	if (2 == ret.size()) {
		if (ret[0] == szKey) {
			stringToValue(ret[1].c_str(), retValue);
			bRet = true;
		}
	}
	return bRet;
}

template<class T1, class T2> bool stringToTwoValue(const char* src, T1& first, T2& second, const char delim = '=')
{
	bool bRet = false;
	auto ret = stringSplit (src, delim, false);
	if (2 == ret.size()) {
		stringToValue(ret[0].c_str(), first);
		stringToValue(ret[1].c_str(), second);
		bRet = true;
	}
	return bRet;
}

template<class T1, class T2, class T3> bool stringToThreadValue(const char* src, const char delim, T1& first, T2& second, T3& three)
{
	bool bRet = false;
	auto ret = stringSplit (src, delim, false);
	if (3 == ret.size()) {
		stringToValue(ret[0].c_str(), first);
		stringToValue(ret[1].c_str(), second);
		stringToValue(ret[2].c_str(), three);
		bRet = true;
	}
	return bRet;
}

template<class T1, class T2, class T3, class T4> bool stringToFourValue(const char* src, const char delim, T1& first, T2& second, T3& three, T4& four)
{
	bool bRet = false;
	auto ret = stringSplit (src, delim, false);
	if (4 == ret.size()) {
		stringToValue(ret[0].c_str(), first);
		stringToValue(ret[1].c_str(), second);
		stringToValue(ret[2].c_str(), three);
		stringToValue(ret[3].c_str(), four);
		bRet = true;
	}
	return bRet;
}

template<class T1> bool  getOnceValueFromArgS (int argC, char** argS, const char* key, T1& v, const char delim = '=')
{
	bool bRet = false;
	for (decltype (argC) i = 0; i < argC; i++) {
		auto ret = stringSplit (argS[i], delim);
		if (ret.size() != 2) {
			continue;
		}
		if (ret[0] == key) {
			{std::stringstream s1(ret[1]);s1>>v;}
			bRet = true;
			break;
		}
	}
	return bRet;
}

template<class T1, class T2> bool  getTwoValueFromArgS (int argC, char** argS, const char* key1, const char* key2, T1& v1, T2& v2, const char delim = '=')
{
	bool b1 = false;
	bool b2 = false;
	for (decltype (argC) i = 0; i < argC; i++) {
		auto ret = stringSplit (argS[i], delim);
		if (ret.size() != 2) {
			continue;
		}
		if (ret[0] == key1) {
			{std::stringstream s1(ret[1]);s1>>v1;}
			b1 = true;
		} else if (ret[0] == key2) {
			{std::stringstream s1(ret[1]);s1>>v2;}
			b2 = true;
		} 
		if (b1&&b2) {
			break;
		}
	}
	return b1&&b2;
}

void getStringValuesFromArgS (int argC, char** argS, void** keyValues, int keyNum);
//void TraceStack(std::stringstream& oss);
#endif
