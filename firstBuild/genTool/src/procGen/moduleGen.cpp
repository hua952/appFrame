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

moduleGen:: moduleGen (appFile& rAppData):m_appData(rAppData)
{
}

moduleGen:: ~moduleGen ()
{
}

int   moduleGen:: startGen ()
{
    int   nRet = 0;
    do {
		// auto& rMod = moduleData ();
		// auto moduleName = rMod.moduleName ();
		std::string moduleName = m_appData.appName(); // rMod.moduleName ();
		moduleName += "M";
		// rInfo ("start "<<moduleName<<" module proc");
		auto& rGlobalFile = tSingleton<globalFile>::single ();
		auto  projectHome = rGlobalFile.projectHome ();
		std::string strDir = projectHome;
		auto appName = m_appData.appName(); // rMod.appName ();
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
		// setSrcPath (src.c_str ());
		m_appData.setSrcPath (src.c_str ());
		nR = myMkdir (src.c_str ());

		auto gen = src;
		gen += "/gen";
		m_appData.setGenPath (gen.c_str ());
		nR = myMkdir (gen.c_str ());
		auto win= gen;
		win += "/win";
		nR = myMkdir (win.c_str ());
		auto userLogic = src;
		userLogic += "/userLogic";
		myMkdir (userLogic.c_str ());
		
		moduleCMakeListsGen cmakeGen;
		nR = cmakeGen.startGen (m_appData);
		if (nR) {
			rError ("moduleCMakeListsGen error nR = "<<nR);
			nRet = 2;
			break;
		}
		moduleWinDefGen  defGen;
		nR = defGen.startGen (m_appData);
		if (nR) {
			rError ("defGen.startGen error nR = "<<nR);
			nRet = 3;
			break;
		}
		moduleExportFunGen mefGen;
		nR = mefGen.startGen (m_appData);
		if (nR) {
			rError ("moduleExportFunGen startGen error nR = "<<nR);
			nRet = 4;
			break;
		}
		moduleLogicServerGen  logicGen;
		nR = logicGen.startGen (m_appData);
		if (nR) {
			rError ("moduleLogicServerGen startGen error nR = "<<nR);
			nRet = 5;
			break;
		}
    } while (0);
    return nRet;
}

/*
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

moduleFile& moduleGen:: moduleData ()
{
    return m_moduleData;
}
*/
const char*  moduleGen:: thisRoot ()
{
    return m_thisRoot.get ();
}

void  moduleGen:: setThisRoot (const char* v)
{
    strCpy (v, m_thisRoot);
}

appFile&  moduleGen:: appData ()
{
    return m_appData;
}

