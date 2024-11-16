#ifndef _moduleExportFunGen_h__
#define _moduleExportFunGen_h__
#include <memory>

class appFile;
class moduleExportFunGen
{
public:
    moduleExportFunGen ();
    ~moduleExportFunGen ();
	int  startGen (appFile& rApp);
private:
	int  genH (appFile& rApp);
	int  genCpp (appFile& rApp);
};
#endif
