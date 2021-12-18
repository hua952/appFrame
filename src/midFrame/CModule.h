#ifndef Module_h_
#define Module_h_
#include <memory>
class CModule
{
public:
	typedef void (*afterLoadFunT)();
	typedef void (*beforeUnloadFunT)();
	CModule();
	~CModule();
	int init(const char* szName);
	const char* name();
private:
	int load();
	int unload();
	beforeUnloadFunT	m_onUnloadFun;
	std::unique_ptr<char[]>	 m_name;
	void* m_handle;
};
#endif
