#ifndef _moduleWinDefGen_h__
#define _moduleWinDefGen_h__
#include <memory>

class appFile;
class moduleWinDefGen
{
public:
    moduleWinDefGen ();
    ~moduleWinDefGen ();
	int  startGen (appFile& rApp);
private:
};
#endif
