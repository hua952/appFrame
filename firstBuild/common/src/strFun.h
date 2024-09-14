#ifndef _str_fun_h__
#define _str_fun_h__
#include <memory>
#include <string>
#include <sstream>
#include <vector>
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


template<class T1, class T2> bool stringToTwoValue(const char* src, const char delim, T1& first, T2& second)
{
	bool bRet = false;
	auto ret = stringSplit (src, delim, false);
	if (2 == ret.size()) {
		{std::stringstream s1(ret[0]);s1>>first;}
		{std::stringstream s2(ret[1]);s2>>second;}
		bRet = true;
	}
	return bRet;
}

template<class T1, class T2, class T3> bool stringToThreadValue(const char* src, const char delim, T1& first, T2& second, T3& thread)
{
	bool bRet = false;
	auto ret = stringSplit (src, delim, false);
	if (2 == ret.size()) {
		{std::stringstream s1(ret[0]);s1>>first;}
		{std::stringstream s2(ret[1]);s2>>second;}
		{std::stringstream s3(ret[2]);s3>>thread;}
		bRet = true;
	}
	return bRet;
}

template<class T1, class T2, class T3, class T4> bool stringToFourValue(const char* src, const char delim, T1& first, T2& second, T3& three, T4& four)
{
	bool bRet = false;
	auto ret = stringSplit (src, delim, false);
	if (2 == ret.size()) {
		{std::stringstream s1(ret[0]);s1>>first;}
		{std::stringstream s2(ret[1]);s2>>second;}
		{std::stringstream s3(ret[2]);s3>>three;}
		{std::stringstream s4(ret[3]);s4>>four;}
		bRet = true;
	}
	return bRet;
}
//void TraceStack(std::stringstream& oss);
#endif
