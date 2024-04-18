#include "xmlLoad.h"
#include "strFun.h"
#include "item.h"
#include "itemMgr.h"
#include <string>
#include "myAssert.h"

xmlLoad:: xmlLoad ()
{
}

xmlLoad:: ~xmlLoad ()
{
}

int  xmlLoad:: load (const char* szFileName)
{
	int  nRet = 0;
	do {
		rapidxml::file<> fdoc(szFileName);
		rapidxml::xml_document<> doc;
		doc.parse<0>(fdoc.data());
		auto& rMgr = itemMgr::mgr ();
		auto& rMap = rMgr.itemS ();
		rapidxml::xml_node<> *root = doc.first_node();
		auto pClassName = root->first_attribute("className");
		if (pClassName) {
			rMgr.setClassName (pClassName->value ());
		}
		auto pProjectDir = root->first_attribute("projectDir");
		if (pProjectDir ) {
			rMgr.setProjectDir(pProjectDir->value ());
		}
		auto pProjectName = root->first_attribute("projectName");
		if (pProjectName) {
			rMgr.setProjectName(pProjectName->value ());
		}
		for (auto pC = root->first_node (); pC; pC = pC->next_sibling ()) {
			int nType = BigDataType_int;
			std::string strType = "int";
			auto pType = pC->first_attribute("type");
			auto pI = std::make_shared <item> ();

			if (pType) {
				strType = pType->value ();
				if (strType == "string") {
					nType = BigDataType_string;
				} else if (strType == "bool") {
					nType = BigDataType_bool;
					std::string strV;
					std::stringstream ss(pC->value ());
					ss>>strV;
					myAssert(strV == "true" || strV == "false");
				}
			}
			pI->setItemName (pC->name ());
			pI->setItemType (strType.c_str());
			pI->setItemValue (pC->value ());
			pI->setDataType (nType);
			auto ret = rMap.insert (std::make_pair(pC->name(), pI));
			if (!ret.second) {
				continue;
			}
			auto pCommit = pC->first_attribute("commit");
			if (pCommit) {
				pI->setCommit(pCommit->value ());
			}
		}
	} while (0);
	return nRet;
}

