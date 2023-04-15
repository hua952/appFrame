#ifndef _xmlMsgFileLoad_h__
#define _xmlMsgFileLoad_h__
#include <memory>

#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_utils.hpp>
#include <rapidxml/rapidxml_print.hpp>

class structFile;
class xmlMsgFileLoad
{
public:
    xmlMsgFileLoad ();
    ~xmlMsgFileLoad ();
	int  xmlLoad (const char* szFile);
private:
	int  structLoad (rapidxml::xml_node<char>* pStructS);
	int  onceStructLoad (rapidxml::xml_node<char>* pStruct);
	int  rpcLoad (rapidxml::xml_node<char>* pRpcS);
	int  onceRpcGroupLoad (rapidxml::xml_node<char>* pGroup);
};
#endif
