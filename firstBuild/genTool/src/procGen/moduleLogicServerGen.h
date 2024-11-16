#ifndef _moduleLogicServerGen_h__
#define _moduleLogicServerGen_h__
#include <memory>
class appFile;
class moduleLogicServerGen
{
public:
    moduleLogicServerGen ();
    ~moduleLogicServerGen ();
	int  startGen (appFile& rApp);
private:
	int  genWorkerH (appFile& rApp, const char* serverName);
	int  genWorkerCpp (appFile& rApp, const char* serverName);

	int  genOnWorkerInitCpp (appFile& rApp, const char* serverName);
	int  genOnLoopBegin (appFile& rApp, const char* serverName);
	int  genOnLoopEnd(appFile& rApp, const char* serverName);
	int  genOnLoopFrame(appFile& rApp, const char* serverName);
	int  genOnCreateChannelRet (appFile& rApp, const char* serverName);

	int  genWorkerMgrH (appFile& rApp);
	int  genWorkerMgrCpp (appFile& rApp);

	int  genMgrCpp (appFile& rApp, const char* genPath);
	int  genServerReadOnlyCpp (appFile& rApp);
	int  genPackFun (appFile& rApp, const char* szServerName);
};
#endif
