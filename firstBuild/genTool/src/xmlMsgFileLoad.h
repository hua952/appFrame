#ifndef _xmlMsgFileLoad_h__
#define _xmlMsgFileLoad_h__
#include <memory>

#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_utils.hpp>
#include <rapidxml/rapidxml_print.hpp>

class structFile;
class msgPmpFile;
class xmlMsgFileLoad
{
public:
    xmlMsgFileLoad ();
    ~xmlMsgFileLoad ();
	int  xmlLoad (const char* szPmpName, const char* szFile, msgPmpFile& rPmp);
	int  xmlLoadFromStr (const char* szPmpName, char* szCon, msgPmpFile& rPmp);
private:
	int  structLoadBody (structFile& rSt, rapidxml::xml_node<char>* pStruct);
	int  structLoad (rapidxml::xml_node<char>* pStructS, msgPmpFile& rPmp, bool powerCom);
	int  onceStructLoad (rapidxml::xml_node<char>* pStruct, msgPmpFile& rPmp, bool powerCom);
	int  rpcLoad (rapidxml::xml_node<char>* pRpcS, msgPmpFile& rPmp, bool powerCom);
	int  onceRpcGroupLoad (rapidxml::xml_node<char>* pGroup, msgPmpFile& rPmp, bool powerCom);
};
#endif
