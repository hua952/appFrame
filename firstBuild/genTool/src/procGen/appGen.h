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
	int CMakeListGen (appFile& rApp);
	int batFileGen (appFile& rApp);
	const char*  thisRoot ();
	void  setThisRoot (const char* v);
private:
	std::unique_ptr <char[]>  m_thisRoot;
};
#endif
