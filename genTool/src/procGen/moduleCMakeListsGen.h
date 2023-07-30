#ifndef _moduleCMakeListsGen_h__
#define _moduleCMakeListsGen_h__
#include <memory>

class moduleGen;
class moduleCMakeListsGen
{
public:
    moduleCMakeListsGen ();
    ~moduleCMakeListsGen ();
	int  startGen (moduleGen& rModel);
private:
};
#endif
