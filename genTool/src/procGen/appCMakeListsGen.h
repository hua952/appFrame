#ifndef _appCMakeListsGen_h__
#define _appCMakeListsGen_h__
#include <memory>

class appFile;
class appCMakeListsGen
{
public:
    appCMakeListsGen ();
    ~appCMakeListsGen ();
	int  startGen (appFile& rApp);
private:
};
#endif
