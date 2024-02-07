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

