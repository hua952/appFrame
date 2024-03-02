#ifndef _itemMgr_h__
#define _itemMgr_h__
#include <memory>
#include <map>
#include <string>

class item;
class itemMgr
{
public:
	using itemMap = std::map<std::string, std::shared_ptr<item>>;
    itemMgr ();
    ~itemMgr ();
	itemMap&  itemS ();
	static itemMgr&  mgr ();
	const char*  className ();
	void  setClassName (const char* v);
	const char*  projectName ();
	void  setProjectName (const char* v);
	const char*  projectDir ();
	void  setProjectDir (const char* v);
	const char*  includeDir ();
	void  setIncludeDir (const char* v);
	const char*  libDir ();
	void  setLibDir (const char* v);
private:
	std::unique_ptr <char[]>  m_libDir;
	std::unique_ptr <char[]>  m_includeDir;
	std::unique_ptr <char[]>  m_projectDir;
	std::unique_ptr <char[]>  m_projectName;
	std::unique_ptr <char[]>  m_className;
	itemMap  m_itemS;
};
#endif
