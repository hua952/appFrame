#ifndef _moduleGen_h__
#define _moduleGen_h__
#include <memory>

class appFile;
class moduleGen
{
public:
    moduleGen (appFile& rAppData);
    ~moduleGen ();
	int  startGen ();
	/*
    const char*  genPath ();
    void  setGenPath (const char* v);
    const char*  srcPath ();
    void  setSrcPath (const char* v);
	*/
	
	// moduleFile&               moduleData ();
	appFile&                  appData ();
	const char*  thisRoot ();
	void  setThisRoot (const char* v);
	const char*  genSrcDir ();
	void  setGenSrcDir (const char* v);
	
private:
	std::unique_ptr <char[]>  m_frameFunDir;
	std::unique_ptr <char[]>  m_thisRoot;
    std::unique_ptr <char[]>  m_procMsgPath;
    // std::unique_ptr <char[]>  m_srcPath;
    std::unique_ptr <char[]>  m_genPath;
	// moduleFile&               m_moduleData;
	appFile&                  m_appData;
};
#endif
