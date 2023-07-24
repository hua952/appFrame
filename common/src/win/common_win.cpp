#include <windows.h>
//#include <dbghelp.h>
#include <io.h>
#include<direct.h>
#include <imagehlp.h>
#include <cstring>
#include "comFun.h"
#include "strFun.h"
#include <memory>
#include <string>
#include "depOSFun.h"

bool  isPathExit (const char* szPath)
{
	return _access(szPath, 0) == 0;
}

int strCaseCmp (const char* str1, const char* str2)
{
	return _stricmp(str1, str2);
}

int  myMkdir (const char* szPath)
{
	auto nL = strlen (szPath);
	auto pB = std::make_unique<char[]> (nL + 2);
	auto pp = pB.get();
	for (decltype (nL) i = 0; i < nL; i++) {
		auto c = szPath[i];
		pp[i] = c;
		if ('\\' == c || '/' == c) {
			pp [i + 1] = 0;
			auto bE = isPathExit (pp);
			if (bE) {
				continue;
			}
			auto nR = _mkdir(pp);
			if (0 != nR) {
				return errno;
			}
		}
	}

	if (szPath[nL - 1] != '\\'|| szPath[nL - 1] != '/') {
		pp[nL] = '\\';
		pp[nL + 1] = 0;

		auto bE = isPathExit (pp);
		if (!bE) {
			auto nR = _mkdir(pp);
			if (0 != nR) {
				return errno;
			}	
		}
	}
	return 0;
}

void* loadDll (const char* szFile)
{
	void* pRet = nullptr;
	auto szName = szFile;
	//HINSTANCE hdll;
	auto hdll = LoadLibraryA(szName);
	//myAssert(hdll);
	do {
		if(!hdll){
			break;
		}
		pRet = hdll;
	}while(0);
	return pRet;
}

void* getFun (void* handle, const char* szFunName)
{
	void* pRet = nullptr;
	pRet = (GetProcAddress((HINSTANCE)handle, szFunName));
	return pRet;
}

int   unloadDll (void* handle)
{
	int nRet = 0;
	FreeLibrary ((HMODULE)handle);
	return nRet;
}

void myW2U8(const wchar_t* wcText, std::unique_ptr<char[]>& var)
{
	auto tl = wcslen (wcText);
	auto len= WideCharToMultiByte(CP_UTF8,0, wcText, tl, NULL,0, NULL ,NULL );
	var = std::make_unique<char[]> (len+1);
	auto str = var.get();
	WideCharToMultiByte(CP_UTF8,0, wcText, tl, str, len, NULL ,NULL );
	str[len]= '\0';
}

void myA2W(const char* szText, std::unique_ptr<wchar_t[]>& var)
{
	auto sl = strlen (szText);
	auto len = MultiByteToWideChar(CP_ACP, 0, szText, sl, NULL, 0);
	var = std::make_unique <wchar_t[]> (len + 1);
	auto pV = var.get();
	MultiByteToWideChar(CP_ACP, 0, szText, sl, pV, len + 1);
	pV[len] = 0;
}

void myU8ToW(const char* szText, std::unique_ptr<wchar_t[]>& var)
{
	auto szU8 = szText;
    int wcsLen = ::MultiByteToWideChar(CP_UTF8, NULL, szU8, strlen(szU8), NULL, 0);
	var = std::make_unique<wchar_t[]>(wcsLen + 1);
    wchar_t* wszString = var.get ();
    ::MultiByteToWideChar(CP_UTF8, NULL, szU8, strlen(szU8), wszString, wcsLen);
    wszString[wcsLen] = 0;
}

int             getCurModelPath (std::unique_ptr<char[]>& pathBuf)
{
	const auto c_pathMax = 512;
	auto tempBuf = std::make_unique<char[]>(c_pathMax + 1);
	int nRet = 0;
	auto pBuff = tempBuf.get();
	if (GetModuleFileNameA(NULL, pBuff, c_pathMax) == 0) {
		nRet = 1;
	} else {
		int len = (int)(strlen(pBuff));
		auto resLen = len;
		for (int i = len - 1; i > 0; i--) {
			if (pBuff[i] == '\\' || pBuff[i] == '/') {
				pBuff[i + 1] = 0;
				resLen = i + 1;
				break;
			}
		}
		pathBuf = std::make_unique<char[]>(resLen + 1);
		strNCpy (pathBuf.get(), resLen + 1, pBuff);
	}
	return nRet;
}


bool isPathInterval (char c)
{
	return c == '\\' || c == '/';
}
/*
void TraceStack(std::stringstream& oss)
{
	static const int MAX_STACK_FRAMES = 20;
	
	void *pStack[MAX_STACK_FRAMES];
 
	HANDLE process = GetCurrentProcess();
	SymInitialize(process, NULL, TRUE);
	WORD frames = CaptureStackBackTrace(0, MAX_STACK_FRAMES, pStack, NULL);
 
	;
	oss << "stack traceback: " << std::endl;
	for (WORD i = 0; i < frames; ++i) {
		DWORD64 address = (DWORD64)(pStack[i]);
 
		DWORD64 displacementSym = 0;
		char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(wchar_t)];
		PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;
		pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
		pSymbol->MaxNameLen = MAX_SYM_NAME;
 
		DWORD displacementLine = 0;
		IMAGEHLP_LINE64 line;
		//SymSetOptions(SYMOPT_LOAD_LINES);
		line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
 
		if (SymFromAddr(process, address, &displacementSym, pSymbol)
		 && SymGetLineFromAddr64(process, address, &displacementLine, &line)) {
			oss << "\t" << pSymbol->Name << " at " << line.FileName << ":" << line.LineNumber << "(0x" << std::hex << pSymbol->Address << std::dec << ")" << std::endl;
		}
		else {
			oss << "\terror: " << GetLastError() << std::endl;
		}
	}
}
*/
