#ifndef _appGen_h__
#define _appGen_h__
#include <memory>

class appFile;
class appGen
{
public:
    appGen ();
    ~appGen ();
	int startGen (appFile& rApp);
private:
};
#endif
