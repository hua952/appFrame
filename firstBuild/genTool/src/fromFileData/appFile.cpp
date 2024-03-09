#include "appFile.h"
#include "strFun.h"
#include "mainLoop.h"

appFile:: appFile ()
{
	m_procId = 0x7fffffff;
	m_detachServerS = false;
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
}

appFile::moduleFileNameSet&  appFile:: moduleFileNameS ()
{
    return m_moduleFileNameS;
}

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
    bool   nRet = 0;
    do {
		auto& rMs =  moduleFileNameS ();
		nRet = !rMs.empty ();
    } while (0);
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
