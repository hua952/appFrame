#ifndef Module_h_
#define Module_h_
#include <memory>
#include "mainLoop.h"

class CModule
{
public:
	typedef void (*afterLoadFunT)(const ForLogicFun* pForLogic);
	typedef void (*beforeUnloadFunT)();
	CModule();
	~CModule();
	int init(const char* szName);
	const char* name();
	int load(const ForLogicFun* pForLogic);
private:
	int unload();
	beforeUnloadFunT	m_onUnloadFun;
	std::unique_ptr<char[]>	 m_name;
	void* m_handle;
};
#endif
