#ifndef _structFileMgr_h__
#define _structFileMgr_h__
#include <memory>
#include <string>
#include <map>
#include <vector>

class structFile;
class structFileMgr
{
public:
	using structMap = std::map<std::string, std::shared_ptr <structFile>>;
	using structVec = std::vector<std::shared_ptr <structFile>>;
    structFileMgr ();
    ~structFileMgr ();
	structMap&  structS ();
	structVec&  structOrder ();
private:
	structVec  m_structOrder;
	structMap  m_structS;
};
#endif
