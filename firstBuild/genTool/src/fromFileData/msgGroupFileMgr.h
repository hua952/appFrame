#ifndef _msgGroupFileMgr_h__
#define _msgGroupFileMgr_h__
#include <memory>
#include <string>
#include <map>

class msgGroupFile;
class msgGroupFileMgr
{
public:
	using msgGroupMap = std::map <std::string, std::shared_ptr <msgGroupFile>>;
    msgGroupFileMgr ();
    ~msgGroupFileMgr ();
	msgGroupFile* findGroup (const char* pGName);
	msgGroupMap&  msgGroupS ();
private:
	msgGroupMap  m_msgGroupS;
};
#endif
