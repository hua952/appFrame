#ifndef _configMgr_h__
#define _configMgr_h__
#include <memory>
#include "comFun.h"
class configMgr
{
public:
    configMgr ();
    ~configMgr ();
	const char*  defFile ();
	void  setDefFile (const char* v);
	int procArgS(int cArg, char* argS[]);

private:
	bool    procStrA(const char* cKey, char* argv[], std::unique_ptr<char[]>& var);
	void    procBoolA(const char* cKey, char* argv[], bool& var);
	void    procUdwordA(const char* cKey, char* argv[], udword& var);
	void	procUwordA(const char* cKey, char* argv[], uword& var);
	std::unique_ptr<char[]>  m_defFile;
};
#endif
