#include <windows.h>
#include <dbghelp.h>
#include <stdio.h>
#include <memory>

#if _MSC_VER
#define snprintf _snprintf
#endif

#define STACK_INFO_LEN  1024

void showTraceStack(std::unique_ptr<char[]>& pBuff)
{
    static const int MAX_STACK_FRAMES = 20;
    void *pStack[MAX_STACK_FRAMES];
	pBuff = std::make_unique<char[]>(STACK_INFO_LEN * MAX_STACK_FRAMES);
	auto szStackInfo = pBuff.get();
    //static char szStackInfo[STACK_INFO_LEN * MAX_STACK_FRAMES];
    static char szFrameInfo[STACK_INFO_LEN];

    HANDLE process = GetCurrentProcess();
    SymInitialize(process, NULL, TRUE);
    WORD frames = CaptureStackBackTrace(0, MAX_STACK_FRAMES, pStack, NULL);
    strcpy(szStackInfo,  "stack traceback:\n");

    for (WORD i = 0; i < frames; ++i) {
        DWORD64 address = (DWORD64)(pStack[i]);

        DWORD64 displacementSym = 0;
        char buffer[sizeof(SYMBOL_INFO)+MAX_SYM_NAME * sizeof(TCHAR)];
        PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;
        pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        pSymbol->MaxNameLen = MAX_SYM_NAME;

        DWORD displacementLine = 0;
        IMAGEHLP_LINE64 line;
        line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

        auto rAddr = SymFromAddr(process, address, &displacementSym, pSymbol);
        if (rAddr) {
			auto r64 = SymGetLineFromAddr64(process, address, &displacementLine, &line);
			if (r64) {
            snprintf(szFrameInfo, sizeof(szFrameInfo), "\t%s() at %s:%d(0x%x)\n", 
            	pSymbol->Name, line.FileName, line.LineNumber, pSymbol->Address);
			} else {
				snprintf(szFrameInfo, sizeof(szFrameInfo), "\tSymGetLineFromAddr64 error: %d\n", GetLastError());
			}
        } else {
            snprintf(szFrameInfo, sizeof(szFrameInfo), "\terror: %d\n", GetLastError());
        }
        strcat(szStackInfo, szFrameInfo);
    }

    //printf("%s", szStackInfo); // 输出到控制台，也可以打印到日志文件中
}

