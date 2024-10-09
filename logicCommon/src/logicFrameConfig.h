#ifndef _logicFrameConfig_h__
#define _logicFrameConfig_h__
#include <memory>
#include "frameConfig.h"

class logicFrameConfig: public frameConfig
{
public:
	struct serverRunInfo
	{
		udword		  sleepSetp;
		serverIdType  beginId;
		serverIdType  runNum;
		bool          autoRun;
	};
	using endPointType = std::pair<std::unique_ptr<char[]>, uword>;
	struct gateNode
	{
		endPointType endPoints[2];
		ubyte        endPointNum{0};
	};
    logicFrameConfig ();
    ~logicFrameConfig ();
	int afterAllArgProc ();
	int  serverGroupNum ();
	void  setServerGroupNum (int v);
	const serverRunInfo*  serverGroups();
	ubyte  gateNodeNum ();
	void  setGateNodeNum (ubyte v);
	gateNode* gateNodes ();
private:
	int procGateNode();
	int procWorkers ();
	ubyte  m_gateNodeNum{0};
    std::unique_ptr<gateNode[]>	m_gateNodes;
    std::unique_ptr<serverRunInfo[]>	m_serverGroups;
	int  m_serverGroupNum {0};
};
#endif
