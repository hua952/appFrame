#include "globalFile.h"
#include "strFun.h"
#include "appFileMgr.h"
#include "tSingleton.h"
#include "appFile.h"

globalFile:: globalFile ()
{
}

globalFile:: ~globalFile ()
{
}
/*
const char*  globalFile::frameBinPath ()
{
    return m_frameBinPath.get ();
}

void  globalFile::setFrameBinPath (const char* v)
{
    strCpy (v, m_frameBinPath);
}
*/
const char* globalFile:: depLibHome ()
{
    return m_depLibHome.get ();
}
/*
void  globalFile::setDepLibHome (const char* v)
{
    strCpy (v, m_depLibHome);
}
*/
const char*  globalFile::depIncludeHome ()
{
    return m_depIncludeHome.get ();
}

/*
void  globalFile::setDepIncludeHome (const char* v)
{
    strCpy (v, m_depIncludeHome);
}
const char*  globalFile::frameHome ()
{
    return m_frameHome.get ();
}

void  globalFile::setFrameHome (const char* v)
{
    strCpy (v, m_frameHome);
}
*/
const char* globalFile:: projectHome ()
{
    return m_projectHome.get ();
}

void  globalFile::setProjectHome (const char* v)
{
    strCpy (v, m_projectHome);
}

const char*   globalFile:: frameLibPath ()
{
    return m_frameLibPath.get();
}

/*
const char*  globalFile::frameLibPath ()
{
    return m_frameLibPath.get ();
}

void  globalFile::setFrameLibPath (const char* v)
{
    strCpy (v, m_frameLibPath);
}
const char* globalFile:: outPutPath ()
{
    return m_outPutPath.get ();
}

void  globalFile::setOutPutPath (const char* v)
{
    strCpy (v, m_outPutPath);
}
*/

bool  globalFile:: haveServer ()
{
    bool   nRet = false;
    do {
		auto &rAppS = tSingleton<appFileMgr>::single ().appS ();
		for (auto it = rAppS.begin (); it != rAppS.end (); it++) {
			nRet = it->second->haveServer ();
			if (nRet) {
				break;
			}
		}
    } while (0);
    return nRet;
}

globalFile::msgFileV& globalFile:: msgFileS ()
{
    return m_msgFileS;
}

const char*  globalFile:: projectName ()
{
    return m_projectName.get ();
}

void  globalFile:: setProjectName (const char* v)
{
    strCpy (v, m_projectName);
}
/*
const char*  globalFile:: installPath ()
{
    return m_installPath.get ();
}

void  globalFile:: setInstallPath (const char* v)
{
    strCpy (v, m_installPath);
}
*/
const char*  globalFile:: frameInstallPath ()
{
    return m_frameInstallPath.get ();
}
void  globalFile:: setFrameInstallPath (const char* v)
{
    strCpy (v, m_frameInstallPath);
	std::string strT = v;
	strT += "lib";
	strCpy (strT.c_str (), m_frameLibPath);
}

const char*  globalFile:: thirdPartyDir ()
{
    return m_thirdPartyDir.get ();
}

void  globalFile:: setThirdPartyDir (const char* v)
{
    strCpy (v, m_thirdPartyDir);
	std::string strI = v;
	strI += "include/";
	strCpy (strI.c_str(), m_depIncludeHome);

	std::string strL = v;
	strL += "lib/";
	strCpy (strL.c_str(), m_depLibHome);
}

void   globalFile:: getRealInstallPath (std::string& strPath)
{
    do {
		auto p = frameInstallPath ();
		if (p) {
			strPath = p;
		} else {
			strPath = projectHome ();
			strPath += "/../";
			auto project = projectName ();
			strPath += project;
			strPath += "Install";
		}
    } while (0);
}

msgPmpFile*  globalFile:: findMsgPmp (const char* szPmpName)
{
    msgPmpFile*  nRet = nullptr;
    do {
		auto& rMap =  msgFileS ();
		auto it = rMap.find (szPmpName);
		if (rMap.end () != it) {
			nRet = it->second.get();
		}
    } while (0);
    return nRet;
}

std::vector<std::string>&  globalFile:: rootServerS ()
{
    return m_rootServerS;
}


