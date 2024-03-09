#include "frameConfig.h"
#include <memory>
#include "strFun.h"
frameConfig::frameConfig ()
{
	m_clearTag = false;
				strCpy("d:/pzh/include", m_depInclude);
				m_sendCount = 10000;
				m_sendMember = 10000;
				m_sendNum = 10000;
				m_sendSetp = 10000;
				
}
bool  frameConfig::clearTag ()
{
    return m_clearTag;
}

void  frameConfig::setClearTag (bool v)
{
	m_clearTag = v
;
}

const char*  frameConfig::depInclude ()
{
    return m_depInclude.get();
}

void  frameConfig::setDepInclude (const char* v)
{
strCpy(v, m_depInclude)
;
}

word  frameConfig::sendCount ()
{
    return m_sendCount;
}

void  frameConfig::setSendCount (word v)
{
	m_sendCount = v
;
}

ubyte  frameConfig::sendMember ()
{
    return m_sendMember;
}

void  frameConfig::setSendMember (ubyte v)
{
	m_sendMember = v
;
}

int  frameConfig::sendNum ()
{
    return m_sendNum;
}

void  frameConfig::setSendNum (int v)
{
	m_sendNum = v
;
}

udword  frameConfig::sendSetp ()
{
    return m_sendSetp;
}

void  frameConfig::setSendSetp (udword v)
{
	m_sendSetp = v
;
}



int  frameConfig:: procCmdArgS (int nArg, char** argS)
{
	int nRet = 0;
	do {
		for (decltype (nArg) i = 1; i < nArg; i++) {
			std::unique_ptr<char[]> pArg;
			strCpy (argS[i], pArg);
			char* retS[3];
			auto pBuf = pArg.get ();
			auto nR = strR (pBuf, '=', retS, 3);
			if (2 != nR) {
				nRet = 1;
				break;
			}
			std::string strKey;
			std::string strVal;
			std::stringstream ssK (retS[0]);
			ssK>>strKey;
			std::stringstream ssV (retS[1]);
			ssV>>strVal;
			if (strKey == "clearTag") {
				m_clearTag = strVal == "true";
				continue;
			}
					if (strKey == "depInclude") {
				strCpy(strVal.c_str(), m_depInclude);
				continue;
			}
					if (strKey == "sendCount") {
				ssV>>m_sendCount;
				continue;
			}
					if (strKey == "sendMember") {
				m_sendMember = (ubyte)(atoi(retS[1]));
				continue;
			}
					if (strKey == "sendNum") {
				ssV>>m_sendNum;
				continue;
			}
					if (strKey == "sendSetp") {
				ssV>>m_sendSetp;
				continue;
			}
		
		}
	} while (0);
	return nRet;
}
