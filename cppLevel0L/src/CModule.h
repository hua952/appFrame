#ifndef Module_h_
#define Module_h_
/*
#include <memory>
#include "mainLoop.h"


class CModule
{
public:
	typedef int (*getServerSFunT)(const char** pBuff, int nBuffNum);
	//typedef void (*RegMsgFunT)(const ForRegMsg* pForLogic);
	typedef void (*beforeUnloadFunT)();
	CModule();
	~CModule();
	int init(const char* szName);
	const char* name();
	int load(int nArgC, char** argS,ForLogicFun* pForLogic);
	// int load_os (const char* szName, ForLogicFun* pForLogic);
	logicOnAcceptFT  fnLogicOnAccept ();
	logicOnConnectFT  fnLogicOnConnect ();
	afterLoadFunT  fnAfterLoad ();
	onLoopBeginFT  fnOnLoopBegin ();
	onLoopEndFT    fnOnLoopEnd ();
	onFrameLagicFT  fnOnFrameLogic ();
private:
	onFrameLagicFT  m_fnOnFrameLogic;
	onLoopEndFT  m_fnOnLoopEnd;
	onLoopBeginFT  m_fnOnLoopBegin;
	afterLoadFunT  m_fnAfterLoad;
	logicOnConnectFT  m_fnLogicOnConnect;
	logicOnAcceptFT  m_fnLogicOnAccept;
	beforeUnloadFunT	m_onUnloadFun;
	int unload();
	std::unique_ptr<char[]>	 m_name;
	void* m_handle;
};
*/
#endif
