#ifndef _str_fun_h__
#define _str_fun_h__
extern "C"
{
void strNCpy(char* szD, int nSize, const char* szS);
int strR(char* szTex, char sp, char** pBuf, int BufNum);
int strRS(char* szText, char* szP, char** pBuf, int BufNum);
int strCaseCmp (const char* str1, const char* str2);
}
#endif
