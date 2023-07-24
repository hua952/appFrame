#ifndef _moduleMgrGen_h__
#define _moduleMgrGen_h__
#include <memory>

class appFile;
class moduleMgrGen
{
public:
    moduleMgrGen ();
    ~moduleMgrGen ();
	int  startGen (appFile& rApp);
	const char*  thisRoot ();
	void  setThisRoot (const char* v);
	const char*  genSrcDir ();
	void  setGenSrcDir (const char* v);
private:
	std::unique_ptr <char[]>  m_genSrcDir;
	std::unique_ptr <char[]>  m_thisRoot;
	int  writeCMakeLists (appFile& rApp);
	int  writeDefFile ();
	int  writeExportFunH ();
	int  writeExportFunCpp ();
	int  writeLogicServerH ();
	int  writeLogicServerCpp ();
};
#endif
