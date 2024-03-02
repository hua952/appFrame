#include "moduleGen.h"
#include "strFun.h"
#include "comFun.h"
#include "fromFileData/globalFile.h"
#include "fromFileData/moduleFile.h"
#include "fromFileData/appFile.h"
#include "tSingleton.h"
#include "moduleCMakeListsGen.h"
#include "moduleWinDefGen.h"
#include "moduleExportFunGen.h"
#include "moduleLogicServerGen.h"
#include <string>
#include "rLog.h"

moduleGen:: moduleGen (moduleFile& rModuleData):m_moduleData (rModuleData)
{
}

moduleGen:: ~moduleGen ()
{
}

int   moduleGen:: startGen ()
{
    int   nRet = 0;
    do {
		auto& rMod = moduleData ();
		auto moduleName = rMod.moduleName ();
		// rInfo ("start "<<moduleName<<" module proc");
		auto& rGlobalFile = tSingleton<globalFile>::single ();
		auto  projectHome = rGlobalFile.projectHome ();
		std::string strDir = projectHome;
		auto appName = rMod.appName ();
		strDir += "/";
		strDir += appName;
		strDir += "/";
		strDir += moduleName;
		setThisRoot (strDir.c_str ());
		auto moduleDir = strDir;
		int nR = 0;
		nR = myMkdir (strDir.c_str ());
		auto src = moduleDir;
		src += "/src";
		setSrcPath (src.c_str ());
		nR = myMkdir (src.c_str ());

		auto gen = src;
		gen += "/gen";
		setGenPath (gen.c_str ());
		nR = myMkdir (gen.c_str ());
		auto win= gen;
		win += "/win";
		nR = myMkdir (win.c_str ());
		auto userLogic = src;
		userLogic += "/userLogic";
		myMkdir (userLogic.c_str ());
		/*
		auto procMsg = src;
		procMsg += "/procMsg";
		setProcMsgPath (procMsg.c_str ());
		nR = myMkdir (procMsg.c_str ());
		auto frameDir = procMsg;
		frameDir += "/frameFun";
		setFrameFunDir (frameDir.c_str ());
		myMkdir (frameDir.c_str());
		*/
		moduleCMakeListsGen cmakeGen;
		nR = cmakeGen.startGen (*this);
		if (nR) {
			rError ("moduleCMakeListsGen error nR = "<<nR);
			nRet = 2;
			break;
		}
		moduleWinDefGen  defGen;
		nR = defGen.startGen (*this);
		if (nR) {
			rError ("defGen.startGen error nR = "<<nR);
			nRet = 3;
			break;
		}
		moduleExportFunGen mefGen;
		nR = mefGen.startGen (*this);
		if (nR) {
			rError ("moduleExportFunGen startGen error nR = "<<nR);
			nRet = 4;
			break;
		}
		// rInfo ("before "<<moduleName<<" moduleLogicServerGen ");
		moduleLogicServerGen  logicGen;
		nR = logicGen.startGen (*this);
		if (nR) {
			rError ("moduleLogicServerGen startGen error nR = "<<nR);
			nRet = 5;
			break;
		}
		// rInfo ("after "<<moduleName<<" moduleLogicServerGen nR = "<<nR);
    } while (0);
    return nRet;
}

const char*  moduleGen:: genPath ()
{
    return m_genPath.get ();
}

void  moduleGen:: setGenPath (const char* v)
{
    strCpy (v, m_genPath);
}

const char*  moduleGen:: srcPath ()
{
    return m_srcPath.get ();
}

void  moduleGen:: setSrcPath (const char* v)
{
    strCpy (v, m_srcPath);
}
/*
const char*  moduleGen:: procMsgPath ()
{
    return m_procMsgPath.get ();
}

void  moduleGen:: setProcMsgPath (const char* v)
{
    strCpy (v, m_procMsgPath);
}
*/
moduleFile& moduleGen:: moduleData ()
{
    return m_moduleData;
}

const char*  moduleGen:: thisRoot ()
{
    return m_thisRoot.get ();
}

void  moduleGen:: setThisRoot (const char* v)
{
    strCpy (v, m_thisRoot);
}
/*
const char*  moduleGen:: frameFunDir ()
{
    return m_frameFunDir.get ();
}

void  moduleGen:: setFrameFunDir (const char* v)
{
    strCpy (v, m_frameFunDir);
}
*/

