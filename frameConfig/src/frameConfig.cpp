#include "frameConfig.h"
#include <memory>
#include <sstream>
#include <fstream>
#include <vector>
#include "strFun.h"

frameConfig::frameConfig ()
{
	strCpy("libeventSession", m_addLogic);
	m_clearTag = false;
	m_detachServerS = 1;
	strCpy("", m_endPoint);
	strCpy("cppLevel0L", m_level0);
	strCpy("", m_logFile);
	m_logLevel = 0;
	strCpy("", m_logicModel);
	strCpy("libeventSession", m_netLib);
	m_procId = 0;
	strCpy("C:/work/appFrameProject/project/chatTestInstall", m_workDir);
	
}
const char*  frameConfig::addLogic ()
{
    return m_addLogic.get();
}

void  frameConfig::setAddLogic (const char* v)
{
	strCpy(v, m_addLogic);
}

bool  frameConfig::clearTag ()
{
    return m_clearTag;
}

void  frameConfig::setClearTag (bool v)
{
	m_clearTag = v;
}

ubyte  frameConfig::detachServerS ()
{
    return m_detachServerS;
}

void  frameConfig::setDetachServerS (ubyte v)
{
	m_detachServerS = v;
}

const char*  frameConfig::endPoint ()
{
    return m_endPoint.get();
}

void  frameConfig::setEndPoint (const char* v)
{
	strCpy(v, m_endPoint);
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

const char*  frameConfig::netLib ()
{
    return m_netLib.get();
}

void  frameConfig::setNetLib (const char* v)
{
	strCpy(v, m_netLib);
}

uword  frameConfig::procId ()
{
    return m_procId;
}

void  frameConfig::setProcId (uword v)
{
	m_procId = v;
}

const char*  frameConfig::workDir ()
{
    return m_workDir.get();
}

void  frameConfig::setWorkDir (const char* v)
{
	strCpy(v, m_workDir);
}




int  frameConfig:: loadConfig (const char* szFile)
{
	int nRet = 0;
	do {
		std::ifstream ifs (szFile);
		if (!ifs) {
			nRet = 1;
			break;
		}
		std::stringstream ss;
		std::string strLine;
		std::vector <std::string> vecT;
		while(getline(ifs, strLine)) {
			auto nf = strLine.find ("//");
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
			ssV>>strVal;
			if (strKey == "addLogic") {
				strCpy(strVal.c_str(), m_addLogic);
				continue;
			}
					if (strKey == "clearTag") {
				m_clearTag = strVal == "true";
				continue;
			}
					if (strKey == "detachServerS") {
				m_detachServerS = (ubyte)(atoi(retS[1]));
				continue;
			}
					if (strKey == "endPoint") {
				strCpy(strVal.c_str(), m_endPoint);
				continue;
			}
					if (strKey == "level0") {
				strCpy(strVal.c_str(), m_level0);
				continue;
			}
					if (strKey == "logFile") {
				strCpy(strVal.c_str(), m_logFile);
				continue;
			}
					if (strKey == "logLevel") {
				ssV>>m_logLevel;
				continue;
			}
					if (strKey == "logicModel") {
				strCpy(strVal.c_str(), m_logicModel);
				continue;
			}
					if (strKey == "netLib") {
				strCpy(strVal.c_str(), m_netLib);
				continue;
			}
					if (strKey == "procId") {
				ssV>>m_procId;
				continue;
			}
					if (strKey == "workDir") {
				strCpy(strVal.c_str(), m_workDir);
				continue;
			}
		
		}
	} while (0);
	return nRet;
}
