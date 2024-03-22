#ifndef _gloableData_h__
#define _gloableData_h__
#include <memory>
#include "mainLoop.h"
class gloableData  // Thread safety
{
public:
	using  muServerNumS = std::unique_ptr<loopHandleType[]>;
    gloableData ();
    ~gloableData ();
private:
	muServerNumS m_muServerPairS[c_serverLevelNum];
};
#endif
