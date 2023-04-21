#include "moduleLogicServerGen.h"
#include "strFun.h"
#include "moduleGen.h"
#include "fromFileData/moduleFile.h"
#include "rLog.h"
#include <fstream>
#include <string>

moduleLogicServerGen:: moduleLogicServerGen ()
{
}

moduleLogicServerGen:: ~moduleLogicServerGen ()
{
}

int   moduleLogicServerGen:: startGen (moduleGen& rMod)
{
    int   nRet = 0;
	do {
		auto& rData = rMod.moduleData ();
		auto modName = rData.moduleName ();
		int nR = 0;
		nR = genH (rMod);
		if (nR) {
			rError ("moduleLogicServerGen write h file error modName = "<<modName<<" nR = "<<nR);
			nRet = 1;
			break;
		}
		nR = genCpp (rMod);
		if (nR) {
			rError ("moduleLogicServerGen write cpp file error modName = "<<modName<<" nR = "<<nR);
			nRet = 2;
			break;
		}
	} while (0);
	return nRet;
}

int   moduleLogicServerGen:: genH (moduleGen& rMod)
{
	int   nRet = 0;
	do {
		auto genPath = rMod.genPath ();
		std::string strFilename = genPath;	
		strFilename += "/logicServer.h";
		std::ofstream os(strFilename);
		if (!os) {
			rError ("open file for write error fileName = "<<strFilename.c_str ());
			nRet = 1;
			break;
		}
		os<<R"(#ifndef logicServerMgr_h__
#define logicServerMgr_h__

#include "mainLoop.h"
class logicServerMgr
{
public:
    logicServerMgr ();
    ~logicServerMgr ();
	void  afterLoad(ForLogicFun* pForLogic);
private:
};
#endif)";
		} while (0);
return nRet;
}

int   moduleLogicServerGen:: genCpp (moduleGen& rMod)
{
    int   nRet = 0;
    do {
		auto genPath = rMod.genPath ();
		std::string strFilename = genPath;	
		strFilename += "/logicServer.cpp";
		std::ofstream os(strFilename);
		if (!os) {
			rError ("open file for write error fileName = "<<strFilename.c_str ());
			nRet = 1;
			break;
		}
		os<<R"(#include "logicServerMgr.h"
#include "myAssert.h"
#include "gLog.h"
#include "strFun.h"
#include "loopHandleS.h"

logicServerMgr::logicServerMgr ()
{
}
logicServerMgr::~logicServerMgr ()
{
}

static int OnFrameCli(void* pArgS)
{
	return procPacketFunRetType_del;
}

void  logicServerMgr::afterLoad(ForLogicFun* pForLogic)
{
	auto fnCreateLoop = pForLogic->fnCreateLoop;
	auto fnRegMsg = pForLogic->fnRegMsg;
})";

    } while (0);
    return nRet;
}

