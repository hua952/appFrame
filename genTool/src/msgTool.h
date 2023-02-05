#ifndef _msgTool_h_
#define _msgTool_h_
#include"tSingleton.h"
#include <rapidxml/rapidxml.hpp>  
#include <rapidxml/rapidxml_utils.hpp>  
#include <rapidxml/rapidxml_print.hpp>  
#include "arrayMap.h"
#include "comFun.h"
#include "rpcMgr.h"
#include <memory>
//#include "tVector.h"
#include <vector>
#include "appMgr.h"
#include <string>

using namespace rapidxml;  
class msgTool//:public tSingleton<msgTool>
{
public:
	
	msgTool();
	~msgTool();

	int init(int nArgC, char* argS[]);
	int start(const char* szFile);
	int startProcMsg(const char* szFile);
	bool isBaseDataType(const char* szType);
	const char* findDataType(const char* szT);
	const char* projectHome ();
	const char* frameHome ();
	const char* frameBinPath ();
	const char* frameLibPath ();
	const char* outPutPath ();
	const char* depIncludeHome ();
	void setDepIncludeHome (const char* szPath);
	const char* depLibHome ();
	void setDepLibHome (const char* szPath);
	void        setFrameHome (const char* szPath);
	void        getDefMsgPath (std::unique_ptr<char[]>& path);
	void  setProjectHome (const char* pHome);
	void  initPath();
protected:
	typedef arraySet<cpchar, cpcharCmp> dataTypeSet;
	dataTypeSet m_dataTypeSet;
	int procRpc1(rapidxml::xml_node<char> * pRpc);
	int procRpc2(rapidxml::xml_node<char> * pRpc);
	int procStruct(rapidxml::xml_node<char> * pRpc);
	int procMsg(rapidxml::xml_node<char> * pMsg, rpcMgr::msgInfo& rMsg);
	int procStructData(rapidxml::xml_node<char> * pMsg, rpcMgr::structInfo& rMsg);
	int procData(rapidxml::xml_node<char> * pData, rpcMgr::dataInfo& rData);

	int procAppS(rapidxml::xml_node<char> * pRpc);
	int procOnceApp(rapidxml::xml_node<char> * pApp, realServer* pReal);
	int appProcOnceRpcArry(app* pCurApp, rapidxml::xml_node<char> * pApp, const char* szPmpName = NULL, const char* szPmpClass = NULL, const char* szNextPmp = NULL);

	int procCmd1(char* szText);
	//pIStringKeyVector    m_vStruct;
	//pIStringKeyArraySet  m_vRpc;
	//rpcMgr::rpcArryInfo* m_pCurRpcArry;
	std::shared_ptr<rpcMgr::rpcArryInfo>  m_pCurRpcArry;
	char	  m_defFile[rpcMgr::c_dirSize];
	std::unique_ptr <char[]> m_projectHome;
	std::unique_ptr <char[]> m_frameHome;
	std::unique_ptr <char[]> m_frameBinPath;
	std::unique_ptr <char[]> m_frameLibPath;
	std::unique_ptr <char[]> m_depIncludeHome;
	std::unique_ptr <char[]> m_depLibHome;
	std::unique_ptr <char[]> m_outPutPath;
};
#endif
