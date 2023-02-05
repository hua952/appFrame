#ifndef Module_h_
#define Module_h_
#include <memory>
#include "mainLoop.h"


class CModule
{
public:
	typedef int (*getServerSFunT)(const char** pBuff, int nBuffNum);
	typedef void (*afterLoadFunT)(const ForLogicFun* pForLogic);
	//typedef void (*RegMsgFunT)(const ForRegMsg* pForLogic);
	typedef void (*beforeUnloadFunT)();
	CModule();
	~CModule();
	int init(const char* szName);
	const char* name();
	int load(const ForLogicFun* pForLogic);
	int load_os (const char* szName, const ForLogicFun* pForLogic);
private:
	int unload();
	beforeUnloadFunT	m_onUnloadFun;
	std::unique_ptr<char[]>	 m_name;
	void* m_handle;
};
#endif