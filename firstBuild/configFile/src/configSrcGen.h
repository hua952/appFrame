#ifndef _configSrcGen_h__
#define _configSrcGen_h__
#include <memory>

class configSrcGen
{
public:
    configSrcGen ();
    ~configSrcGen ();
	int startGen ();
	static  configSrcGen& mgr();
	const char*  projectHome ();
	void  setProjectHome (const char* v);
	const char*  srcDir ();
	void  setSrcDir (const char* v);
	const char*  configFileName ();
	void  setConfigFileName (const char* v);
private:
	std::unique_ptr <char[]>  m_configFileName;
	std::unique_ptr <char[]>  m_srcDir;
	std::unique_ptr <char[]>  m_projectHome;
	int writeCMakeList ();
	int writeClassH ();
	int writeClassCpp ();
};
#endif
