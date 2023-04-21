#ifndef _moduleMgrGen_h__
#define _moduleMgrGen_h__
#include <memory>

class appFile;
class moduleMgrGen
{
public:
    moduleMgrGen ();
    ~moduleMgrGen ();
	int  startGen (appFile& rApp);
private:
	int  writeCMakeLists (const char* szDir, appFile& rApp);
};
#endif
