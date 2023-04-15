#ifndef _newTool_h__
#define _newTool_h__

#include <memory>
#include <vector>
#include <string>

class newTool
{
public:
    newTool ();
    ~newTool ();
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
private:
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