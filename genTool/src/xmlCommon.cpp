#include "xmlCommon.h"
#include "strFun.h"
#include <cstring>

xmlCommon:: xmlCommon ()
{
}

xmlCommon:: ~xmlCommon ()
{
}

const char*  xmlCommon:: getStringA(rapidxml::xml_node<char>* pStruct, const char* szName)
{
    const char*  nRet = nullptr;
	do {
		auto pZ = pStruct->first_attribute(szName);
		if(!pZ) {
			break;
		}
		nRet = pZ->value ();
	} while (0);
    return nRet;
}

int   xmlCommon:: getBoolA(rapidxml::xml_node<char>* pStruct, const char* szName, bool& bRet)
{
    int   nRet = 0;
	do {
		auto pZ = pStruct->first_attribute(szName);
		if(!pZ) {
			nRet = 1;
			break;
		}
		auto szValue = pZ->value ();
		if (!szValue) {
			nRet = 2;
			break;
		}
		if (0 == strlen (szValue)) {
			nRet = 3;
			break;
		}
		auto nL = atoi (szValue);
		bRet = nL;
	} while (0);
    return nRet;
}

int   xmlCommon:: getIntA(rapidxml::xml_node<char>* pStruct, const char* szName, int& bRet)
{
    int   nRet = 0;
    do {
		auto pS = getStringA (pStruct, szName);
		if (pS) {
			bRet = atoi (pS);
		} else {
			nRet = 1;
		}
    } while (0);
    return nRet;
}

