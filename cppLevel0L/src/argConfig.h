#ifndef _argConfig_h__
#define _argConfig_h__
#include <memory>
#include "frameConfig.h"

class argConfig:public frameConfig
{
public:
	struct stServer 
	{
		std::unique_ptr<char[]>  serverName;
		serverIdType	         openNum;
	};
	struct stModel
	{
		std::unique_ptr<char[]>  modelName;
		std::unique_ptr<stServer[]>  serverS;
		serverIdType	         serverTemNum;
	};
    argConfig ();
    ~argConfig ();
	int afterAllArgProc ();
	stModel* allModelS ();
	serverIdType  modelNum ();
private:
	serverIdType  m_modelNum;
	std::unique_ptr<stModel[]>  m_modelS;
};
#endif
