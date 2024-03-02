#ifndef _moduleWinDefGen_h__
#define _moduleWinDefGen_h__
#include <memory>

class moduleGen;
class moduleWinDefGen
{
public:
    moduleWinDefGen ();
    ~moduleWinDefGen ();
	int  startGen (moduleGen&  rMod);
private:
};
#endif
