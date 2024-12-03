#include "frameConfig.h"
#include <memory>
#include <sstream>
#include <fstream>
#include <vector>
#include "strFun.h"

frameConfig::frameConfig ()
{
	m_allocDebug = false;
	m_appGroupId = 4;
	m_appNetType = 0;
	m_clearTag = false;
	m_dbgSleep = 0;
	m_delSaveTokenTime = 30000;
	m_dumpMsg = false;
	strCpy("", m_endPoint);
	strCpy("", m_frameConfigFile);
	strCpy("", m_frameHome);
	m_gateAppGroupId = 0;
	m_gateIndex = 0xffff;
	strCpy("forClientIp:127.0.0.1*forServerIp:127.0.0.1*startPort:12000+forClientIp:127.0.0.1*forServerIp:127.0.0.1*startPort:22000", m_gateInfo);
	m_gateRouteServerGroupId = 0;
	m_heartbeatSetp = 900000;
	strCpy("", m_homeDir);
	strCpy("127.0.0.1", m_ip);
	strCpy("cppLevel0L", m_level0);
	m_logCon = true;
	strCpy("", m_logFile);
	m_logLevel = 2;
	strCpy("", m_logicModel);
	strCpy("", m_modelName);
	strCpy("", m_modelS);
	strCpy("", m_netLib);
	m_netNum = 4;
	strCpy("", m_projectInstallDir);
	strCpy("", m_runWorkNum);
	m_savePackTag = 0;
	strCpy("protobufSer", m_serializePackLib);
	m_srand = true;
	m_startPort = 12000;
	m_testTag = 1234;
	
}
bool  frameConfig::allocDebug ()
{
    return m_allocDebug;
}

void  frameConfig::setAllocDebug (bool v)
{
	m_allocDebug = v;
}

uword  frameConfig::appGroupId ()
{
    return m_appGroupId;
}

void  frameConfig::setAppGroupId (uword v)
{
	m_appGroupId = v;
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

uword  frameConfig::dbgSleep ()
{
    return m_dbgSleep;
}

void  frameConfig::setDbgSleep (uword v)
{
	m_dbgSleep = v;
}

udword  frameConfig::delSaveTokenTime ()
{
    return m_delSaveTokenTime;
}

void  frameConfig::setDelSaveTokenTime (udword v)
{
	m_delSaveTokenTime = v;
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

const char*  frameConfig::frameHome ()
{
    return m_frameHome.get();
}

void  frameConfig::setFrameHome (const char* v)
{
	strCpy(v, m_frameHome);
}

uword  frameConfig::gateAppGroupId ()
{
    return m_gateAppGroupId;
}

void  frameConfig::setGateAppGroupId (uword v)
{
	m_gateAppGroupId = v;
}

uword  frameConfig::gateIndex ()
{
    return m_gateIndex;
}

void  frameConfig::setGateIndex (uword v)
{
	m_gateIndex = v;
}

const char*  frameConfig::gateInfo ()
{
    return m_gateInfo.get();
}

void  frameConfig::setGateInfo (const char* v)
{
	strCpy(v, m_gateInfo);
}

uword  frameConfig::gateRouteServerGroupId ()
{
    return m_gateRouteServerGroupId;
}

void  frameConfig::setGateRouteServerGroupId (uword v)
{
	m_gateRouteServerGroupId = v;
}

udword  frameConfig::heartbeatSetp ()
{
    return m_heartbeatSetp;
}

void  frameConfig::setHeartbeatSetp (udword v)
{
	m_heartbeatSetp = v;
}

const char*  frameConfig::homeDir ()
{
    return m_homeDir.get();
}

void  frameConfig::setHomeDir (const char* v)
{
	strCpy(v, m_homeDir);
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

bool  frameConfig::logCon ()
{
    return m_logCon;
}

void  frameConfig::setLogCon (bool v)
{
	m_logCon = v;
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

const char*  frameConfig::projectInstallDir ()
{
    return m_projectInstallDir.get();
}

void  frameConfig::setProjectInstallDir (const char* v)
{
	strCpy(v, m_projectInstallDir);
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
		ofs<<"allocDebug="<<allocDebug()<<std::endl;
		ofs<<"appGroupId="<<appGroupId()<<std::endl;
		ofs<<"appNetType="<<appNetType()<<std::endl;
		ofs<<"clearTag="<<clearTag()<<std::endl;
		ofs<<"dbgSleep="<<dbgSleep()<<std::endl;
		ofs<<"delSaveTokenTime="<<delSaveTokenTime()<<R"--(  ## 删除token计算器时间间隔(单位毫秒)   )--"<<std::endl;
		ofs<<"dumpMsg="<<dumpMsg()<<std::endl;
	int nendPointLen = 0;
	auto endPoint = this->endPoint();
	if (endPoint) nendPointLen = strlen(endPoint);
	std::string strTendPoint =R"("")";
	if (nendPointLen) 
		strTendPoint = endPoint;
		ofs<<"endPoint="<<strTendPoint<<std::endl;
	int nframeConfigFileLen = 0;
	auto frameConfigFile = this->frameConfigFile();
	if (frameConfigFile) nframeConfigFileLen = strlen(frameConfigFile);
	std::string strTframeConfigFile =R"("")";
	if (nframeConfigFileLen) 
		strTframeConfigFile = frameConfigFile;
		ofs<<"frameConfigFile="<<strTframeConfigFile<<R"--(  ## 框架配置文件   )--"<<std::endl;
	int nframeHomeLen = 0;
	auto frameHome = this->frameHome();
	if (frameHome) nframeHomeLen = strlen(frameHome);
	std::string strTframeHome =R"("")";
	if (nframeHomeLen) 
		strTframeHome = frameHome;
		ofs<<"frameHome="<<strTframeHome<<R"--(  ## 框架的安装目录   )--"<<std::endl;
		ofs<<"gateAppGroupId="<<gateAppGroupId()<<std::endl;
		ofs<<"gateIndex="<<gateIndex()<<std::endl;
	int ngateInfoLen = 0;
	auto gateInfo = this->gateInfo();
	if (gateInfo) ngateInfoLen = strlen(gateInfo);
	std::string strTgateInfo =R"("")";
	if (ngateInfoLen) 
		strTgateInfo = gateInfo;
		ofs<<"gateInfo="<<strTgateInfo<<R"--(  ## gate IP 等   )--"<<std::endl;
		ofs<<"gateRouteServerGroupId="<<gateRouteServerGroupId()<<std::endl;
		ofs<<"heartbeatSetp="<<heartbeatSetp()<<R"--(  ## 删除token计算器时间间隔(单位毫秒)   )--"<<std::endl;
	int nhomeDirLen = 0;
	auto homeDir = this->homeDir();
	if (homeDir) nhomeDirLen = strlen(homeDir);
	std::string strThomeDir =R"("")";
	if (nhomeDirLen) 
		strThomeDir = homeDir;
		ofs<<"homeDir="<<strThomeDir<<std::endl;
	int nipLen = 0;
	auto ip = this->ip();
	if (ip) nipLen = strlen(ip);
	std::string strTip =R"("")";
	if (nipLen) 
		strTip = ip;
		ofs<<"ip="<<strTip<<std::endl;
	int nlevel0Len = 0;
	auto level0 = this->level0();
	if (level0) nlevel0Len = strlen(level0);
	std::string strTlevel0 =R"("")";
	if (nlevel0Len) 
		strTlevel0 = level0;
		ofs<<"level0="<<strTlevel0<<std::endl;
		ofs<<"logCon="<<logCon()<<std::endl;
	int nlogFileLen = 0;
	auto logFile = this->logFile();
	if (logFile) nlogFileLen = strlen(logFile);
	std::string strTlogFile =R"("")";
	if (nlogFileLen) 
		strTlogFile = logFile;
		ofs<<"logFile="<<strTlogFile<<std::endl;
		ofs<<"logLevel="<<logLevel()<<std::endl;
	int nlogicModelLen = 0;
	auto logicModel = this->logicModel();
	if (logicModel) nlogicModelLen = strlen(logicModel);
	std::string strTlogicModel =R"("")";
	if (nlogicModelLen) 
		strTlogicModel = logicModel;
		ofs<<"logicModel="<<strTlogicModel<<std::endl;
	int nmodelNameLen = 0;
	auto modelName = this->modelName();
	if (modelName) nmodelNameLen = strlen(modelName);
	std::string strTmodelName =R"("")";
	if (nmodelNameLen) 
		strTmodelName = modelName;
		ofs<<"modelName="<<strTmodelName<<std::endl;
	int nmodelSLen = 0;
	auto modelS = this->modelS();
	if (modelS) nmodelSLen = strlen(modelS);
	std::string strTmodelS =R"("")";
	if (nmodelSLen) 
		strTmodelS = modelS;
		ofs<<"modelS="<<strTmodelS<<std::endl;
	int nnetLibLen = 0;
	auto netLib = this->netLib();
	if (netLib) nnetLibLen = strlen(netLib);
	std::string strTnetLib =R"("")";
	if (nnetLibLen) 
		strTnetLib = netLib;
		ofs<<"netLib="<<strTnetLib<<std::endl;
		ofs<<"netNum="<<netNum()<<std::endl;
	int nprojectInstallDirLen = 0;
	auto projectInstallDir = this->projectInstallDir();
	if (projectInstallDir) nprojectInstallDirLen = strlen(projectInstallDir);
	std::string strTprojectInstallDir =R"("")";
	if (nprojectInstallDirLen) 
		strTprojectInstallDir = projectInstallDir;
		ofs<<"projectInstallDir="<<strTprojectInstallDir<<R"--(  ## 安装目录   )--"<<std::endl;
	int nrunWorkNumLen = 0;
	auto runWorkNum = this->runWorkNum();
	if (runWorkNum) nrunWorkNumLen = strlen(runWorkNum);
	std::string strTrunWorkNum =R"("")";
	if (nrunWorkNumLen) 
		strTrunWorkNum = runWorkNum;
		ofs<<"runWorkNum="<<strTrunWorkNum<<R"--(  ## client进程及线程启动的数量   )--"<<std::endl;
		ofs<<"savePackTag="<<savePackTag()<<std::endl;
	int nserializePackLibLen = 0;
	auto serializePackLib = this->serializePackLib();
	if (serializePackLib) nserializePackLibLen = strlen(serializePackLib);
	std::string strTserializePackLib =R"("")";
	if (nserializePackLibLen) 
		strTserializePackLib = serializePackLib;
		ofs<<"serializePackLib="<<strTserializePackLib<<std::endl;
		ofs<<"srand="<<srand()<<std::endl;
		ofs<<"startPort="<<startPort()<<std::endl;
		ofs<<"testTag="<<testTag()<<std::endl;

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
	m_allocDebug = (strVal == "true") || (atoi(strVal.c_str()));
				continue;
			}
				if (strKey == "appGroupId") {
				ssV>>m_appGroupId;
				continue;
			}
				if (strKey == "appNetType") {
				ssV>>m_appNetType;
				continue;
			}
				if (strKey == "clearTag") {
				ssV>>strVal;
	m_clearTag = (strVal == "true") || (atoi(strVal.c_str()));
				continue;
			}
				if (strKey == "dbgSleep") {
				ssV>>m_dbgSleep;
				continue;
			}
				if (strKey == "delSaveTokenTime") {
				ssV>>m_delSaveTokenTime;
				continue;
			}
				if (strKey == "dumpMsg") {
				ssV>>strVal;
	m_dumpMsg = (strVal == "true") || (atoi(strVal.c_str()));
				continue;
			}
				if (strKey == "endPoint") {
				ssV>>strVal;
				if('"'==strVal.c_str()[0] && '"'==strVal.c_str()[strVal.length()-1]) {
					strVal = strVal.substr(1,strVal.length()-2);
				}
	strCpy(strVal.c_str(), m_endPoint);
				continue;
			}
				if (strKey == "frameConfigFile") {
				ssV>>strVal;
				if('"'==strVal.c_str()[0] && '"'==strVal.c_str()[strVal.length()-1]) {
					strVal = strVal.substr(1,strVal.length()-2);
				}
	strCpy(strVal.c_str(), m_frameConfigFile);
				continue;
			}
				if (strKey == "frameHome") {
				ssV>>strVal;
				if('"'==strVal.c_str()[0] && '"'==strVal.c_str()[strVal.length()-1]) {
					strVal = strVal.substr(1,strVal.length()-2);
				}
	strCpy(strVal.c_str(), m_frameHome);
				continue;
			}
				if (strKey == "gateAppGroupId") {
				ssV>>m_gateAppGroupId;
				continue;
			}
				if (strKey == "gateIndex") {
				ssV>>m_gateIndex;
				continue;
			}
				if (strKey == "gateInfo") {
				ssV>>strVal;
				if('"'==strVal.c_str()[0] && '"'==strVal.c_str()[strVal.length()-1]) {
					strVal = strVal.substr(1,strVal.length()-2);
				}
	strCpy(strVal.c_str(), m_gateInfo);
				continue;
			}
				if (strKey == "gateRouteServerGroupId") {
				ssV>>m_gateRouteServerGroupId;
				continue;
			}
				if (strKey == "heartbeatSetp") {
				ssV>>m_heartbeatSetp;
				continue;
			}
				if (strKey == "homeDir") {
				ssV>>strVal;
				if('"'==strVal.c_str()[0] && '"'==strVal.c_str()[strVal.length()-1]) {
					strVal = strVal.substr(1,strVal.length()-2);
				}
	strCpy(strVal.c_str(), m_homeDir);
				continue;
			}
				if (strKey == "ip") {
				ssV>>strVal;
				if('"'==strVal.c_str()[0] && '"'==strVal.c_str()[strVal.length()-1]) {
					strVal = strVal.substr(1,strVal.length()-2);
				}
	strCpy(strVal.c_str(), m_ip);
				continue;
			}
				if (strKey == "level0") {
				ssV>>strVal;
				if('"'==strVal.c_str()[0] && '"'==strVal.c_str()[strVal.length()-1]) {
					strVal = strVal.substr(1,strVal.length()-2);
				}
	strCpy(strVal.c_str(), m_level0);
				continue;
			}
				if (strKey == "logCon") {
				ssV>>strVal;
	m_logCon = (strVal == "true") || (atoi(strVal.c_str()));
				continue;
			}
				if (strKey == "logFile") {
				ssV>>strVal;
				if('"'==strVal.c_str()[0] && '"'==strVal.c_str()[strVal.length()-1]) {
					strVal = strVal.substr(1,strVal.length()-2);
				}
	strCpy(strVal.c_str(), m_logFile);
				continue;
			}
				if (strKey == "logLevel") {
				ssV>>m_logLevel;
				continue;
			}
				if (strKey == "logicModel") {
				ssV>>strVal;
				if('"'==strVal.c_str()[0] && '"'==strVal.c_str()[strVal.length()-1]) {
					strVal = strVal.substr(1,strVal.length()-2);
				}
	strCpy(strVal.c_str(), m_logicModel);
				continue;
			}
				if (strKey == "modelName") {
				ssV>>strVal;
				if('"'==strVal.c_str()[0] && '"'==strVal.c_str()[strVal.length()-1]) {
					strVal = strVal.substr(1,strVal.length()-2);
				}
	strCpy(strVal.c_str(), m_modelName);
				continue;
			}
				if (strKey == "modelS") {
				ssV>>strVal;
				if('"'==strVal.c_str()[0] && '"'==strVal.c_str()[strVal.length()-1]) {
					strVal = strVal.substr(1,strVal.length()-2);
				}
	strCpy(strVal.c_str(), m_modelS);
				continue;
			}
				if (strKey == "netLib") {
				ssV>>strVal;
				if('"'==strVal.c_str()[0] && '"'==strVal.c_str()[strVal.length()-1]) {
					strVal = strVal.substr(1,strVal.length()-2);
				}
	strCpy(strVal.c_str(), m_netLib);
				continue;
			}
				if (strKey == "netNum") {
				ssV>>m_netNum;
				continue;
			}
				if (strKey == "projectInstallDir") {
				ssV>>strVal;
				if('"'==strVal.c_str()[0] && '"'==strVal.c_str()[strVal.length()-1]) {
					strVal = strVal.substr(1,strVal.length()-2);
				}
	strCpy(strVal.c_str(), m_projectInstallDir);
				continue;
			}
				if (strKey == "runWorkNum") {
				ssV>>strVal;
				if('"'==strVal.c_str()[0] && '"'==strVal.c_str()[strVal.length()-1]) {
					strVal = strVal.substr(1,strVal.length()-2);
				}
	strCpy(strVal.c_str(), m_runWorkNum);
				continue;
			}
				if (strKey == "savePackTag") {
				ssV>>m_savePackTag;
				continue;
			}
				if (strKey == "serializePackLib") {
				ssV>>strVal;
				if('"'==strVal.c_str()[0] && '"'==strVal.c_str()[strVal.length()-1]) {
					strVal = strVal.substr(1,strVal.length()-2);
				}
	strCpy(strVal.c_str(), m_serializePackLib);
				continue;
			}
				if (strKey == "srand") {
				ssV>>strVal;
	m_srand = (strVal == "true") || (atoi(strVal.c_str()));
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
		
		}
	} while (0);
	return nRet;
}
