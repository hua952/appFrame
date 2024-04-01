#ifndef _appFileMgr_h__
#define _appFileMgr_h__
#include <memory>
#include <map>
#include <string>

class appFile;
class appFileMgr
{
public:
	using appMap = std::map<std::string, std::shared_ptr<appFile>>;
    appFileMgr ();
    ~appFileMgr ();
	appMap&  appS ();
	appFile* findApp (const char* szAppName);
private:
	appMap  m_appS;
};
#endif
