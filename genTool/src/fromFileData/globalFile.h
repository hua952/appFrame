#ifndef _globalFile_h__
#define _globalFile_h__
#include <memory>
#include <vector>
#include <string>

class globalFile
{
public:
    globalFile ();
    ~globalFile ();
	using msgFileV = std::vector<std::string>;
	const char*  frameBinPath ();
	void  setFrameBinPath (const char* v);
	const char*  depLibHome ();
	void  setDepLibHome (const char* v);
	const char*  depIncludeHome ();
	void  setDepIncludeHome (const char* v);
	const char*  frameHome ();
	void  setFrameHome (const char* v);
	const char*  projectHome ();
	void  setProjectHome (const char* v);
    const char*  frameLibPath ();
    void  setFrameLibPath (const char* v);
    const char*  outPutPath ();
    void  setOutPutPath (const char* v);
	msgFileV&  msgFileS ();
	const char*  projectName ();
	void  setProjectName (const char* v);
    const char*  installPath ();
    void  setInstallPath (const char* v);
    const char*  frameInstallPath ();
    void  setFrameInstallPath (const char* v);
    void  getRealInstallPath (std::string& strPath);
private:
    std::unique_ptr <char[]>  m_installPath;
    std::unique_ptr <char[]>  m_frameInstallPath;
	std::unique_ptr <char[]>  m_projectName;
	msgFileV  m_msgFileS;
    std::unique_ptr <char[]>  m_outPutPath;
    std::unique_ptr <char[]>  m_frameLibPath;
	std::unique_ptr <char[]>  m_projectHome;
	std::unique_ptr <char[]>  m_frameHome;
	std::unique_ptr <char[]>  m_depIncludeHome;
	std::unique_ptr <char[]>  m_depLibHome;
	std::unique_ptr <char[]>  m_frameBinPath;


};
#endif
