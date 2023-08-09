#include "globalFile.h"
#include "strFun.h"

globalFile:: globalFile ()
{
}

globalFile:: ~globalFile ()
{
}

const char*  globalFile::frameBinPath ()
{
    return m_frameBinPath.get ();
}

void  globalFile::setFrameBinPath (const char* v)
{
    strCpy (v, m_frameBinPath);
}

const char* globalFile:: depLibHome ()
{
    return m_depLibHome.get ();
}

void  globalFile::setDepLibHome (const char* v)
{
    strCpy (v, m_depLibHome);
}

const char*  globalFile::depIncludeHome ()
{
    return m_depIncludeHome.get ();
}

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

const char* globalFile:: projectHome ()
{
    return m_projectHome.get ();
}

void  globalFile::setProjectHome (const char* v)
{
    strCpy (v, m_projectHome);
}

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

const char*  globalFile:: installPath ()
{
    return m_installPath.get ();
}

void  globalFile:: setInstallPath (const char* v)
{
    strCpy (v, m_installPath);
}

const char*  globalFile:: frameInstallPath ()
{
    return m_frameInstallPath.get ();
}

void  globalFile:: setFrameInstallPath (const char* v)
{
    strCpy (v, m_frameInstallPath);
}

void   globalFile:: getRealInstallPath (std::string& strPath)
{
    do {
		auto p = installPath ();
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

