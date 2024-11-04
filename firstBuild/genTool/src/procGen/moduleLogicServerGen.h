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

	int  genWorkerH (moduleGen& rMod, const char* serverName);
	int  genWorkerCpp (moduleGen& rMod, const char* serverName);

	int  genOnWorkerInitCpp (moduleGen& rMod, const char* serverName);
	int  genOnLoopBegin (moduleGen& rMod, const char* serverName);
	int  genOnLoopEnd(moduleGen& rMod, const char* serverName);
	int  genOnLoopFrame(moduleGen& rMod, const char* serverName);
	int  genOnCreateChannelRet (moduleGen& rMod, const char* serverName);

	int  genWorkerMgrH (moduleGen& rMod);
	int  genWorkerMgrCpp (moduleGen& rMod);

	int  genCpp (moduleGen& rMod);
	int  genMgrCpp (moduleGen& rMod, const char* genPath);
	int  genServerReadOnlyCpp (moduleGen& rMod);
	// int  genOnFrameFun (moduleGen& rMod, const char* szServerName);
	int  genPackFun (moduleGen& rMod, const char* szServerName);
};
#endif
