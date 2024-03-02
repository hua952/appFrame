#ifndef _moduleExportFunGen_h__
#define _moduleExportFunGen_h__
#include <memory>

class moduleGen;
class moduleExportFunGen
{
public:
    moduleExportFunGen ();
    ~moduleExportFunGen ();
	int  startGen (moduleGen& rMod);
private:
	int  genH (moduleGen& rMod);
	int  genCpp (moduleGen& rMod);
};
#endif
