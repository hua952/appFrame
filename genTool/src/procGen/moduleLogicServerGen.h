#ifndef _moduleLogicServerGen_h__
#define _moduleLogicServerGen_h__
#include <memory>
class moduleGen;
class moduleLogicServerGen
{
public:
    moduleLogicServerGen ();
    ~moduleLogicServerGen ();
	int  startGen (moduleGen& rMod);
private:
	int  genH (moduleGen& rMod);
	int  genCpp (moduleGen& rMod);
	int  genOnFrameFun (moduleGen& rMod, const char* szServerName);
	int  genPackFun (moduleGen& rMod, const char* szServerName);
};
#endif
