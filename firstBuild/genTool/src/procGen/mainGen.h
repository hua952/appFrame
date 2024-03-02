#ifndef _mainGen_h__
#define _mainGen_h__
#include <memory>

class appFile;
class mainGen
{
public:
    mainGen ();
    ~mainGen ();
	int startGen (appFile& rApp);
	int writeCmake (appFile& rApp);
	int writeUnixDep ();
	int writeWinDep ();
	int writeMain (appFile& rApp);
	const char*  thisRoot ();
	void  setThisRoot (const char* v);
	const char*  srcDir ();
	void  setSrcDir (const char* v);
private:
	std::unique_ptr <char[]>  m_srcDir;
	std::unique_ptr <char[]>  m_thisRoot;
};
#endif
