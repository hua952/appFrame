#ifndef logicModelCreator_h__
#define logicModelCreator_h__
#include <string>

class logicModel;
class logicModelCreator
{
public:
    logicModelCreator ();
    ~logicModelCreator ();

	int  writeCMakeFile ();
	int  writeExportFunH ();
	int  writeExportFunCpp();
	int  writeDef ();
	int  writeLogicServerH();
	int  writeLogicServerCpp();
	int  start (logicModel* pModel);

	int  getLogicMgrPath (std::string& strPath);
private:
	logicModel* m_pModel;
};

#endif
