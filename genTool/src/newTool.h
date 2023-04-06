#ifndef _newTool_h__
#define _newTool_h__

#include <memory>
class newTool
{
public:
    newTool ();
    ~newTool ();
	char*  projectHome ();
	void  setProjectHome (const char* v);
	char*  frameHome ();
	void  setFrameHome (const char* v);
	char*  depIncludeHome ();
	void  setDepIncludeHome (const char* v);
private:
	std::unique_ptr <char[]>  m_depIncludeHome;
	std::unique_ptr <char[]>  m_frameHome;
	std::unique_ptr <char[]>  m_projectHome;
};
#endif
