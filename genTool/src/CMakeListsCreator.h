#ifndef MakeListsCreator_h__
#define MakeListsCreator_h__
#include "typeDef.h"

class app;
class CMakeListsCreator
{
public:
    CMakeListsCreator (const char* szSrcDir, app* pApp);
    ~CMakeListsCreator ();
private:
	dword init(const char* szFilename, app* pApp);
};
#endif
