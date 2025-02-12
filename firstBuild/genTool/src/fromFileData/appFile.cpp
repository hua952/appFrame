#include "appFile.h"
#include "strFun.h"
#include "mainLoop.h"
#include "tSingleton.h"
#include "moduleFileMgr.h"
#include "moduleFile.h"
#include "serverFile.h"

appFile:: appFile ()
{
	m_procId = 0x7fffffff;
	m_detachServerS = true;
	m_netType = appNetType_client;
}

appFile:: ~appFile ()
{
}

const char*  appFile::appName ()
{
    return m_appName.get ();
}

void  appFile::setAppName (const char* v)
{
    strCpy (v, m_appName);
	std::string str = "appTmpId_";
	str += v;
	setAppGroupId (str.c_str());
}
/*
appFile::moduleFileNameSet&  appFile:: moduleFileNameS ()
{
    return m_moduleFileNameS;
}
*/
int  appFile:: procId ()
{
    return m_procId;
}

void  appFile:: setProcId (int v)
{
    m_procId = v;
}

appFile::argV&  appFile:: argS ()
{
    return m_argS;
}

bool  appFile:: detachServerS ()
{
    return m_detachServerS;
}

void  appFile:: setDetachServerS (bool v)
{
    m_detachServerS = v;
}

appFile::argV&  appFile:: mainArgS ()
{
    return m_mainArgS;
}

const char*  appFile:: mainLoopServer ()
{
    return m_mainLoopServer.get ();
}

void appFile:: setMainLoopServer (const char* v)
{
    strCpy (v, m_mainLoopServer);
}

bool   appFile:: haveServer ()
{
	return !m_serverS.empty ();
	/*
    bool   nRet = 0;
    do {
		auto& rMs =  moduleFileNameS ();
		nRet = !rMs.empty ();
    } while (0);
    return nRet;
	*/
}

bool   appFile:: haveNetServer ()
{
	bool   nRet = false;
	auto& rSS =  orderS ();
	for (auto ite = rSS.begin (); ite != rSS.end (); ite++) {
		auto pS = ite->get();
		if (pS->route()) {
			nRet = true;
			break;
		}
	}
	/*
    do {
		auto& rMs =  moduleFileNameS ();
		auto& rModMgr = tSingleton <moduleFileMgr>::single ();
		for (auto it = rMs.begin (); rMs.end () != it; ++it) {
			auto pMod = rModMgr.findModule (it->c_str ());
			if (pMod) {
				auto& rSS =  pMod->orderS ();
				for (auto ite = rSS.begin (); ite != rSS.end (); ite++) {
					auto pS = ite->get();
					if (pS->route()) {
						nRet = true;
						break;
					}
				}
			}
			if (nRet) {
				break;
			}
		}
    } while (0);
	*/
    return nRet;
}

ubyte  appFile:: netType ()
{
    return m_netType;
}

void  appFile:: setNetType (ubyte v)
{
    m_netType = v;
}

const char*  appFile:: runWorkNum ()
{
    return m_runWorkNum.get ();
}

void  appFile:: setRunWorkNum (const char* v)
{
    strCpy (v, m_runWorkNum);
}

const char*  appFile:: appGroupId ()
{
    return m_appGroupId.get ();
}

void  appFile:: setAppGroupId (const char* v)
{
    strCpy (v, m_appGroupId);
}

const char*  appFile:: mainLoopGroupId ()
{
    return m_mainLoopGroupId.get ();
}

void  appFile:: setMainLoopGroupId (const char* v)
{
    strCpy (v, m_mainLoopGroupId);
}

uword  appFile:: appGroupIdInt ()
{
    return m_appGroupIdInt;
}

void  appFile:: setAppGroupIdInt (uword v)
{
    m_appGroupIdInt = v;
}

serverFile* appFile:: mainServer ()
{
	std::string strMain = 	mainLoopGroupId();
	serverOrder&ors =  orderS ();
	
	for(auto it = ors.begin(); ors.end() != it; ++it){
		auto ps = it->get();
		if(strMain == ps->strServerGroupId ()) {
			return ps;
			break;
		}
	}
	return NULL;
}

serverFile*   appFile:: findServer (const char* szName)
{
	serverFile*   nRet = 0;
	do {
		auto& rSS = serverS ();
		auto it = rSS.find (szName);
		if (rSS.end () != it) {
			nRet = it->second.get ();
		}
	} while (0);
    return nRet;
}

appFile::serverMap&   appFile:: serverS ()
{
	return m_serverS;
}

appFile::serverOrder&   appFile:: orderS ()
{
	return m_orderS;
}

const char*  appFile:: genPath ()
{
    return m_genPath.get ();
}

void  appFile:: setGenPath (const char* v)
{
    strCpy (v, m_genPath);
}

const char*  appFile:: srcPath ()
{
    return m_srcPath.get ();
}

void  appFile:: setSrcPath (const char* v)
{
    strCpy (v, m_srcPath);
}

