#ifndef _item_h__
#define _item_h__
#include <memory>
#include <sstream>

enum BigDataType
{
	BigDataType_int = 0,
	BigDataType_string = 1,
	BigDataType_bool = 2,
};

class item
{
public:
    item ();
    ~item ();
	const char*  itemName ();
	void  setItemName (const char* v);
	const char*  itemValue ();
	void  setItemValue (const char* v);
	int  dataType ();
	void  setDataType (int v);
	const char*  itemType ();
	void  setItemType (const char* v);
	const char*  wordItemName ();
	const char*  memberItemName ();
	void  setMemberItemName (const char* v);
	const char*  commit ();
	void  setCommit (const char* v);
private:
	std::unique_ptr <char[]>  m_commit;
	std::unique_ptr <char[]>  m_memberItemName;
	std::unique_ptr <char[]>  m_wordItemName;
	std::unique_ptr <char[]>  m_itemType;
	std::unique_ptr <char[]>  m_itemValue;
	std::unique_ptr <char[]>  m_itemName;
	int  m_dataType;
};
#endif
