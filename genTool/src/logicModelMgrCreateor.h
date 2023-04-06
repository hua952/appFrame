#ifndef logicModelMgrCreateor_h__
#define logicModelMgrCreateor_h__
#include <string>
class realServer;
class logicModelMgrCreateor
{
public:
	logicModelMgrCreateor (realServer& rApp);
	realServer&  getApp ();
	int  writeCMakeFile ();
	int  writeExportFunH ();
	int  writeExportFunCpp();
	int  writeDef ();
	int  writeLogicServerH();
	int  writeLogicServerCpp();
	int  start ();

	int  getLogicMgrPath (std::string& strPath);
private:
	realServer&   m_rApp;
};
#endif
