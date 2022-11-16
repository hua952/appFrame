#ifndef _str_fun_h__
#define _str_fun_h__
extern "C"
{
void strNCpy(char* szD, const char* szS, int nSize);
int strR(char* szTex, char sp, char** pBuf, int BufNum);
int strRS(char* szText, char* szP, char** pBuf, int BufNum);
}
#endif
