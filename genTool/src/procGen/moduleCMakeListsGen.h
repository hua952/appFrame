#ifndef _moduleCMakeListsGen_h__
#define _moduleCMakeListsGen_h__
#include <memory>

class moduleFile;
class moduleCMakeListsGen
{
public:
    moduleCMakeListsGen ();
    ~moduleCMakeListsGen ();
	int  startGen (moduleFile& rMod);
private:
};
#endif
