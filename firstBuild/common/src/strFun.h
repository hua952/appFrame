#ifndef _str_fun_h__
#define _str_fun_h__
#include <memory>
#include <sstream>
extern "C"
{
void strNCpy(char* szD, int nSize, const char* szS);
void wcsNCpy(wchar_t* szD, int nSize, const wchar_t* szS);
int strR(char* szTex, char sp, char** pBuf, int BufNum);
int strRS(char* szText, char* szP, char** pBuf, int BufNum);
int strCaseCmp (const char* str1, const char* str2);
void toWord (char* szWord);
}
void myW2U8(const wchar_t* wcText, std::unique_ptr<char[]>& var);
void myA2W(const char* szText, std::unique_ptr<wchar_t[]>& var);
void myU8ToW(const char* szText, std::unique_ptr<wchar_t[]>& var);
void strCpy (const char* szSrc, std::unique_ptr<char[]>& pDec);
void strCpy (const wchar_t* szSrc, std::unique_ptr<wchar_t[]>& pDec);
void wstrCpy (const wchar_t* szSrc, std::unique_ptr<wchar_t[]>& pDec);

//void TraceStack(std::stringstream& oss);
#endif
