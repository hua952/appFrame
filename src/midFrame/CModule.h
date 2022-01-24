#ifndef Module_h_
#define Module_h_
#include <memory>
#include "mainLoop.h"

class CModule
{
public:
	typedef void (*afterLoadFunT)();
	typedef void (*beforeUnloadFunT)(ForLogicFun* pForLogic);
	CModule();
	~CModule();
	int init(const char* szName);
	const char* name();
	int load();
private:
	int unload();
	beforeUnloadFunT	m_onUnloadFun;
	std::unique_ptr<char[]>	 m_name;
	void* m_handle;
};
#endif
