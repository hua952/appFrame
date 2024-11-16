#ifndef _moduleCMakeListsGen_h__
#define _moduleCMakeListsGen_h__
#include <memory>

class appFile;
class moduleCMakeListsGen
{
public:
    moduleCMakeListsGen ();
    ~moduleCMakeListsGen ();
	int  startGen (appFile& rAppData);
private:
};
#endif
