#ifndef _structFileMgr_h__
#define _structFileMgr_h__
#include <memory>
#include <string>
#include <map>

class structFile;
class structFileMgr
{
public:
	using structMap = std::map<std::string, std::shared_ptr <structFile>>;
    structFileMgr ();
    ~structFileMgr ();
	structMap&  structS ();
private:
	structMap  m_structS;
};
#endif
