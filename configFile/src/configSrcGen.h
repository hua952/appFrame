#ifndef _configSrcGen_h__
#define _configSrcGen_h__
#include <memory>

class configSrcGen
{
public:
    configSrcGen ();
    ~configSrcGen ();
	int startGen ();
	static  configSrcGen& mgr();
private:
	int writeClassH ();
	int writeClassCpp ();
};
#endif
