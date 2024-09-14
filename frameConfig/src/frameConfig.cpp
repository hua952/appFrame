#include "frameConfig.h"
#include <memory>
#include <sstream>
#include <fstream>
#include <vector>
#include "strFun.h"

frameConfig::frameConfig ()
{
	m_allocDebug = false;
	m_appNetType = 0;
	m_clearTag = false;
	m_delSaveTokenTime = 50000;
	m_detachServerS = 1;
	m_dumpMsg = false;
	strCpy("", m_endPoint);
	strCpy("", m_frameConfigFile);
	strCpy("forClientIp:127.0.0.1*forServerIp:127.0.0.1*startPort:12000*netServerNum:4+forClientIp:127.0.0.1*forServerIp:127.0.0.1*startPort:22000*netServerNum:4", m_gateInfo);
	strCpy("127.0.0.1", m_ip);
	strCpy("cppLevel0L", m_level0);
	strCpy("", m_logFile);
	m_logLevel = 2;
	strCpy("", m_logicModel);
	strCpy("", m_modelName);
	strCpy("", m_modelS);
	strCpy("libeventSession", m_netLib);
	m_netNum = 4;
	strCpy("", m_runWorkNum);
	m_savePackTag = 0;
	strCpy("protobufSer", m_serializePackLib);
	m_srand = true;
	m_startPort = 12000;
	m_testTag = 1234;
	strCpy("", m_workDir);
	
}
bool  frameConfig::allocDebug ()
{
    return m_allocDebug;
}

void  frameConfig::setAllocDebug (bool v)
{
	m_allocDebug = v;
}

word  frameConfig::appNetType ()
{
    return m_appNetType;
}

void  frameConfig::setAppNetType (word v)
{
	m_appNetType = v;
}

bool  frameConfig::clearTag ()
{
    return m_clearTag;
}

void  frameConfig::setClearTag (bool v)
{
	m_clearTag = v;
}

udword  frameConfig::delSaveTokenTime ()
{
    return m_delSaveTokenTime;
}

void  frameConfig::setDelSaveTokenTime (udword v)
{
	m_delSaveTokenTime = v;
}

ubyte  frameConfig::detachServerS ()
{
    return m_detachServerS;
}

void  frameConfig::setDetachServerS (ubyte v)
{
	m_detachServerS = v;
}

bool  frameConfig::dumpMsg ()
{
    return m_dumpMsg;
}

void  frameConfig::setDumpMsg (bool v)
{
	m_dumpMsg = v;
}

const char*  frameConfig::endPoint ()
{
    return m_endPoint.get();
}

void  frameConfig::setEndPoint (const char* v)
{
	strCpy(v, m_endPoint);
}

const char*  frameConfig::frameConfigFile ()
{
    return m_frameConfigFile.get();
}

void  frameConfig::setFrameConfigFile (const char* v)
{
	strCpy(v, m_frameConfigFile);
}

const char*  frameConfig::gateInfo ()
{
    return m_gateInfo.get();
}

void  frameConfig::setGateInfo (const char* v)
{
	strCpy(v, m_gateInfo);
}

const char*  frameConfig::ip ()
{
    return m_ip.get();
}

void  frameConfig::setIp (const char* v)
{
	strCpy(v, m_ip);
}

const char*  frameConfig::level0 ()
{
    return m_level0.get();
}

void  frameConfig::setLevel0 (const char* v)
{
	strCpy(v, m_level0);
}

const char*  frameConfig::logFile ()
{
    return m_logFile.get();
}

void  frameConfig::setLogFile (const char* v)
{
	strCpy(v, m_logFile);
}

word  frameConfig::logLevel ()
{
    return m_logLevel;
}

void  frameConfig::setLogLevel (word v)
{
	m_logLevel = v;
}

const char*  frameConfig::logicModel ()
{
    return m_logicModel.get();
}

void  frameConfig::setLogicModel (const char* v)
{
	strCpy(v, m_logicModel);
}

const char*  frameConfig::modelName ()
{
    return m_modelName.get();
}

void  frameConfig::setModelName (const char* v)
{
	strCpy(v, m_modelName);
}

const char*  frameConfig::modelS ()
{
    return m_modelS.get();
}

void  frameConfig::setModelS (const char* v)
{
	strCpy(v, m_modelS);
}

const char*  frameConfig::netLib ()
{
    return m_netLib.get();
}

void  frameConfig::setNetLib (const char* v)
{
	strCpy(v, m_netLib);
}

uword  frameConfig::netNum ()
{
    return m_netNum;
}

void  frameConfig::setNetNum (uword v)
{
	m_netNum = v;
}

const char*  frameConfig::runWorkNum ()
{
    return m_runWorkNum.get();
}

void  frameConfig::setRunWorkNum (const char* v)
{
	strCpy(v, m_runWorkNum);
}

udword  frameConfig::savePackTag ()
{
    return m_savePackTag;
}

void  frameConfig::setSavePackTag (udword v)
{
	m_savePackTag = v;
}

const char*  frameConfig::serializePackLib ()
{
    return m_serializePackLib.get();
}

void  frameConfig::setSerializePackLib (const char* v)
{
	strCpy(v, m_serializePackLib);
}

bool  frameConfig::srand ()
{
    return m_srand;
}

void  frameConfig::setSrand (bool v)
{
	m_srand = v;
}

uword  frameConfig::startPort ()
{
    return m_startPort;
}

void  frameConfig::setStartPort (uword v)
{
	m_startPort = v;
}

udword  frameConfig::testTag ()
{
    return m_testTag;
}

void  frameConfig::setTestTag (udword v)
{
	m_testTag = v;
}

const char*  frameConfig::workDir ()
{
    return m_workDir.get();
}

void  frameConfig::setWorkDir (const char* v)
{
	strCpy(v, m_workDir);
}




int  frameConfig:: dumpConfig (const char* szFile)
{
	int nRet = 0;
	
	std::unique_ptr<char[]>	dirBuf;
	strCpy (szFile,dirBuf);
	auto pDir = dirBuf.get();
	upDir (pDir);
	do {
		std::ofstream ofs (szFile);
		if (!ofs) {
			nRet = 1;
			break;
		}
		ofs<<R"(allocDebug=false)"<<std::endl;
		ofs<<R"(appNetType=0)"<<std::endl;
		ofs<<R"(clearTag=false)"<<std::endl;
		ofs<<R"(delSaveTokenTime=50000)"<<std::endl;
		ofs<<R"(detachServerS=1)"<<std::endl;
		ofs<<R"(dumpMsg=false)"<<std::endl;
		ofs<<R"(endPoint=)"<<std::endl;
		ofs<<R"(frameConfigFile=  ## 框架配置文件)"<<std::endl;
		ofs<<R"(gateInfo=forClientIp:127.0.0.1*forServerIp:127.0.0.1*startPort:12000*netServerNum:4+forClientIp:127.0.0.1*forServerIp:127.0.0.1*startPort:22000*netServerNum:4  ## gate IP 等)"<<std::endl;
		ofs<<R"(ip=127.0.0.1)"<<std::endl;
		ofs<<R"(level0=cppLevel0L)"<<std::endl;
		ofs<<R"(logFile=)"<<std::endl;
		ofs<<R"(logLevel=2)"<<std::endl;
		ofs<<R"(logicModel=)"<<std::endl;
		ofs<<R"(modelName=)"<<std::endl;
		ofs<<R"(modelS=)"<<std::endl;
		ofs<<R"(netLib=libeventSession)"<<std::endl;
		ofs<<R"(netNum=4)"<<std::endl;
		ofs<<R"(runWorkNum=  ## client进程及线程启动的数量)"<<std::endl;
		ofs<<R"(savePackTag=0)"<<std::endl;
		ofs<<R"(serializePackLib=protobufSer)"<<std::endl;
		ofs<<R"(srand=true)"<<std::endl;
		ofs<<R"(startPort=12000)"<<std::endl;
		ofs<<R"(testTag=1234)"<<std::endl;
		ofs<<R"(workDir=)"<<std::endl;

	} while (0);
	return nRet;
}

int  frameConfig:: loadConfig (const char* szFile)
{
	int nRet = 0;
	do {
		std::ifstream ifs (szFile);
		if (!ifs) {
			dumpConfig (szFile);
			break;
		}

		std::stringstream ss;
		std::string strLine;
		std::vector <std::string> vecT;
		while(getline(ifs, strLine)) {
			auto nf = strLine.find ("##");
			if (nf != strLine.npos) {
				strLine = strLine.substr(0, nf);
			}
			vecT.push_back (strLine);
		}
		auto argS = std::make_unique<std::unique_ptr<char[]>[]> (vecT.size());
		auto ppArgS = std::make_unique<char*[]> (vecT.size());
		auto n = 0;
		for (auto it = vecT.begin (); it != vecT.end (); it++) {
			auto& arg = argS[n];
			strCpy (it->c_str(), arg);
			ppArgS[n] = arg.get();
			n++;
		}
		nRet = procCmdArgS (n, ppArgS.get());
	} while (0);
	return nRet;
}

int  frameConfig:: procCmdArgS (int nArg, char** argS)
{
	int nRet = 0;
	do {
		for (decltype (nArg) i = 0; i < nArg; i++) {
			std::unique_ptr<char[]> pArg;
			strCpy (argS[i], pArg);
			char* retS[3];
			auto pBuf = pArg.get ();
			auto nR = strR (pBuf, '=', retS, 3);
			if (2 != nR) {
				continue;
			}
			std::string strKey;
			std::string strVal;
			std::stringstream ssK (retS[0]);
			ssK>>strKey;
			std::stringstream ssV (retS[1]);
		if (strKey == "allocDebug") {
				ssV>>strVal;
	m_allocDebug = strVal == "true";
				continue;
			}
				if (strKey == "appNetType") {
				ssV>>m_appNetType;
				continue;
			}
				if (strKey == "clearTag") {
				ssV>>strVal;
	m_clearTag = strVal == "true";
				continue;
			}
				if (strKey == "delSaveTokenTime") {
				ssV>>m_delSaveTokenTime;
				continue;
			}
				if (strKey == "detachServerS") {
				m_detachServerS = (ubyte)(atoi(retS[1]));
				continue;
			}
				if (strKey == "dumpMsg") {
				ssV>>strVal;
	m_dumpMsg = strVal == "true";
				continue;
			}
				if (strKey == "endPoint") {
				ssV>>strVal;
	strCpy(strVal.c_str(), m_endPoint);
				continue;
			}
				if (strKey == "frameConfigFile") {
				ssV>>strVal;
	strCpy(strVal.c_str(), m_frameConfigFile);
				continue;
			}
				if (strKey == "gateInfo") {
				ssV>>strVal;
	strCpy(strVal.c_str(), m_gateInfo);
				continue;
			}
				if (strKey == "ip") {
				ssV>>strVal;
	strCpy(strVal.c_str(), m_ip);
				continue;
			}
				if (strKey == "level0") {
				ssV>>strVal;
	strCpy(strVal.c_str(), m_level0);
				continue;
			}
				if (strKey == "logFile") {
				ssV>>strVal;
	strCpy(strVal.c_str(), m_logFile);
				continue;
			}
				if (strKey == "logLevel") {
				ssV>>m_logLevel;
				continue;
			}
				if (strKey == "logicModel") {
				ssV>>strVal;
	strCpy(strVal.c_str(), m_logicModel);
				continue;
			}
				if (strKey == "modelName") {
				ssV>>strVal;
	strCpy(strVal.c_str(), m_modelName);
				continue;
			}
				if (strKey == "modelS") {
				ssV>>strVal;
	strCpy(strVal.c_str(), m_modelS);
				continue;
			}
				if (strKey == "netLib") {
				ssV>>strVal;
	strCpy(strVal.c_str(), m_netLib);
				continue;
			}
				if (strKey == "netNum") {
				ssV>>m_netNum;
				continue;
			}
				if (strKey == "runWorkNum") {
				ssV>>strVal;
	strCpy(strVal.c_str(), m_runWorkNum);
				continue;
			}
				if (strKey == "savePackTag") {
				ssV>>m_savePackTag;
				continue;
			}
				if (strKey == "serializePackLib") {
				ssV>>strVal;
	strCpy(strVal.c_str(), m_serializePackLib);
				continue;
			}
				if (strKey == "srand") {
				ssV>>strVal;
	m_srand = strVal == "true";
				continue;
			}
				if (strKey == "startPort") {
				ssV>>m_startPort;
				continue;
			}
				if (strKey == "testTag") {
				ssV>>m_testTag;
				continue;
			}
				if (strKey == "workDir") {
				ssV>>strVal;
	strCpy(strVal.c_str(), m_workDir);
				continue;
			}
		
		}
	} while (0);
	return nRet;
}
