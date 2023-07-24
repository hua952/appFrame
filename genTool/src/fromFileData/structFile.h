#ifndef _structFile_h__
#define _structFile_h__
#include <memory>
#include <vector>
#include <map>
#include <string>

class dataFile;
class structFile
{
public:
	using dataV = std::vector <std::shared_ptr <dataFile>>;
	using dataMap = std::map <std::string, std::shared_ptr <dataFile>>;
    structFile ();
    ~structFile ();
	const char*  structName ();
	void  setStructName (const char* v);
	const char*  commit ();
	void  setCommit (const char* v);
	dataV&  dataOrder ();
	dataMap&  dataS ();
	bool   hasData ();
private:
	dataMap  m_dataS;
	dataV  m_dataOrder;
	std::unique_ptr <char[]>  m_commit;
	std::unique_ptr <char[]>  m_structName;
};
#endif
