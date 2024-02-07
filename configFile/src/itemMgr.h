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
private:
	std::unique_ptr <char[]>  m_className;
	itemMap  m_itemS;
};
#endif
