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
	int writeUserLogic ();
	const char*  thisRoot ();
	void  setThisRoot (const char* v);
	const char*  srcDir ();
	void  setSrcDir (const char* v);
	const char*  userLogicDir ();
private:
	std::unique_ptr <char[]>  m_userLogicDir;
	std::unique_ptr <char[]>  m_srcDir;
	std::unique_ptr <char[]>  m_thisRoot;
};
#endif
