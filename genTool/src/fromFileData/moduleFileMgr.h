#ifndef _moduleFileMgr_h__
#define _moduleFileMgr_h__
#include <memory>
#include <map>
#include <string>

class moduleFile;
class moduleFileMgr
{
public:
	using moduleMap = std::map <std::string, std::shared_ptr<moduleFile>>;
    moduleFileMgr ();
    ~moduleFileMgr ();
	moduleMap&  moduleS ();
	moduleFile*  findModule (const char* szName);
private:
	moduleMap  m_moduleS;
};
#endif
