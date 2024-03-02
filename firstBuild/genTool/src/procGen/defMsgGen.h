#ifndef _defMsgGen_h__
#define _defMsgGen_h__
#include <memory>
#include <string>

class dataFile;
class structFile;
class msgGroupFile;
class msgFile;
class defMsgGen
{
public:
    defMsgGen ();
    ~defMsgGen ();
	int startGen ();
	int loopHandleSGen ();
	int mkDir ();
	const char*  srcDir ();
	void  setSrcDir (const char* v);
private:
	std::unique_ptr <char[]>  m_srcDir;
};

#endif
