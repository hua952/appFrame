#include "item.h"
#include "strFun.h"

item:: item ()
{
	m_itemType = 0;
	m_dataType = 0;
}

item:: ~item ()
{
}

const char*  item:: itemName ()
{
    return m_itemName.get ();
}

void  item:: setItemName (const char* v)
{
    strCpy (v, m_itemName);
    strCpy (v, m_wordItemName);
	toWord (m_wordItemName.get());
	std::string str = "m_";
	str += v;
    strCpy (str.c_str(), m_memberItemName);
}


const char*  item:: itemValue ()
{
    return m_itemValue.get ();
}

void  item:: setItemValue (const char* v)
{
    strCpy (v, m_itemValue);
}

int  item:: dataType ()
{
    return m_dataType;
}

void  item:: setDataType (int v)
{
    m_dataType = v;
}

const char*  item:: itemType ()
{
    return m_itemType.get ();
}

void  item:: setItemType (const char* v)
{
    strCpy (v, m_itemType);
}

const char*  item:: wordItemName ()
{
    return m_wordItemName.get ();
}

const char*  item:: memberItemName ()
{
    return m_memberItemName.get ();
}

void  item:: setMemberItemName (const char* v)
{
    strCpy (v, m_memberItemName);
}

