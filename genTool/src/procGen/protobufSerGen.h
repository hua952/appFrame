#ifndef _protobufSerGen_h__
#define _protobufSerGen_h__
#include <memory>

class protobufSerGen
{
public:
    protobufSerGen ();
    ~protobufSerGen ();
	int  startGen ();
	int  CMakeListGen ();
	int  defFileGen ();
	const char*  rootDir ();
	void  setRootDir (const char* v);
	const char*  srcDir ();
	void  setSrcDir (const char* v);
private:
	std::unique_ptr <char[]>  m_srcDir;
	std::unique_ptr <char[]>  m_rootDir;
	int  mkDir ();
};
#endif
