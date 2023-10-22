#ifndef _argMgr_h__
#define _argMgr_h__
#include <memory>
#include "typeDef.h"

class argMgr
{
public:
    argMgr ();
    ~argMgr ();
	int procArgS(int nArgC, char** argS);
	virtual void onCmpKey (char* argv[]);
protected:
	bool    procStrA(const char* cKey, char* argv[], std::unique_ptr<char[]>& var);
	void    procBoolA(const char* cKey, char* argv[], bool& var);
	void    procUdwordA(const char* cKey, char* argv[], udword& var);
	void    procDwordA(const char* cKey, char* argv[], dword& var);
	void	procUwordA(const char* cKey, char* argv[], uword& var);
};
#endif
