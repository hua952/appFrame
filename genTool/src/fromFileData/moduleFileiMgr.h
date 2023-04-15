#ifndef _moduleFileiMgr_h__
#define _moduleFileiMgr_h__
#include <memory>
#include <map>
#include <string>

class moduleFile;
class moduleFileiMgr
{
public:
	using moduleMap = std::map <std::string, std::shared_ptr<moduleFile>>;
    moduleFileiMgr ();
    ~moduleFileiMgr ();
	moduleMap&  moduleS ();
private:
	moduleMap  m_moduleS;
};
#endif
