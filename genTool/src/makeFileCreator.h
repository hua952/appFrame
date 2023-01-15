#ifndef _makeFileCreator_h_
#define _makeFileCreator_h_
#include "constVar.h"

class app;
class makeFileCreator
{
public:
	makeFileCreator(const char* szFilename, app* pApp);
private:
	dword init(const char* szFilename, app* pApp);
};
#endif
