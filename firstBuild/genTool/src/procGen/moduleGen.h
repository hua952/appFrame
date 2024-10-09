#ifndef _moduleGen_h__
#define _moduleGen_h__
#include <memory>

class moduleFile;
class moduleGen
{
public:
    moduleGen (moduleFile& rModuleData);
    ~moduleGen ();
	int  startGen ();
    const char*  genPath ();
    void  setGenPath (const char* v);
    const char*  srcPath ();
    void  setSrcPath (const char* v);
	
	moduleFile&               moduleData ();
	const char*  thisRoot ();
	void  setThisRoot (const char* v);
	const char*  genSrcDir ();
	void  setGenSrcDir (const char* v);
	
private:
	std::unique_ptr <char[]>  m_frameFunDir;
	std::unique_ptr <char[]>  m_thisRoot;
    std::unique_ptr <char[]>  m_procMsgPath;
    std::unique_ptr <char[]>  m_srcPath;
    std::unique_ptr <char[]>  m_genPath;
	moduleFile&               m_moduleData;
};
#endif
