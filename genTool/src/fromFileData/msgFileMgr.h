#ifndef _msgFileMgr_h__
#define _msgFileMgr_h__
#include <memory>
#include <string>
#include <map>

class msgFile;
class msgFileMgr
{
public:
	using msgMap = std::map<std::string, std::shared_ptr <msgFile>>;
    msgFileMgr ();
    ~msgFileMgr ();
	msgFile*  findMsg (const char* szName);
	msgMap&  msgS ();
private:
	msgMap  m_msgS;
};
#endif
