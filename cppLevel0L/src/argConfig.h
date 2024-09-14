#ifndef _argConfig_h__
#define _argConfig_h__
#include <memory>
#include "frameConfig.h"

class argConfig:public frameConfig
{
public:
	struct stServer 
	{
		// std::unique_ptr<char[]>  serverName;
		serverIdType	         serverTmpId;
		serverIdType	         openNum;
		bool                     autoRun;
		bool                     route;
	};
	struct stModel
	{
		std::unique_ptr<char[]>  modelName;
		std::unique_ptr<stServer[]>  serverS;
		serverIdType	         serverTemNum;
	};
	struct serverRunInfo
	{
		udword		  sleepSetp;
		serverIdType  beginId;
		serverIdType  runNum;
		bool          autoRun;
	};
    argConfig ();
    ~argConfig ();
	int afterAllArgProc ();
	stModel* allModelS ();
	serverIdType  modelNum ();
	const char*  modelMgrName ();
	void  setModelMgrName (const char* v);
	int  serverGroupNum ();
	void  setServerGroupNum (int v);
	const serverRunInfo*  serverGroups();
private:
    std::unique_ptr<serverRunInfo>	m_serverGroups;
	int  m_serverGroupNum {0};
	std::unique_ptr<char[]>  m_modelMgrName;
	serverIdType  m_modelNum;
	std::unique_ptr<stModel[]>  m_modelS;
};
#endif
