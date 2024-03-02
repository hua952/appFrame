#ifndef _xmlCommon_h__
#define _xmlCommon_h__
#include <memory>

#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_utils.hpp>
#include <rapidxml/rapidxml_print.hpp>

class xmlCommon
{
public:
    xmlCommon ();
    ~xmlCommon ();
	int  getBoolA(rapidxml::xml_node<char>* pStruct, const char* szName, bool& bRet);
	int  getIntA(rapidxml::xml_node<char>* pStruct, const char* szName, int& bRet);
	const char* getStringA(rapidxml::xml_node<char>* pStruct, const char* szName);
private:
};
#endif
