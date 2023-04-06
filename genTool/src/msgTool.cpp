#include<assert.h>
#include<iostream>
#include<string.h>
#include<string>
#include<stdlib.h>
#include "cLog.h"
#include"rpcMgr.h"
#include"appMgr.h"
#include"constVar.h"
#include"msgTool.h"
#include"strFun.h"
#include"tSingleton.h"
#include "strFun.h"
#include "rLog.h"

msgTool::msgTool(): m_dataTypeSet(8)
{
  m_dataTypeSet.insert(g_sbyte);
  m_dataTypeSet.insert(g_ubyte);
  m_dataTypeSet.insert(g_word);
  m_dataTypeSet.insert(g_uword);
  m_dataTypeSet.insert(g_dword);
  m_dataTypeSet.insert(g_udword);
  //m_dataTypeSet.insert(g_qword);
  //m_dataTypeSet.insert(g_uqword);
  m_dataTypeSet.insert(g_char);
  m_dataTypeSet.insert(g_float32);
  m_dataTypeSet.insert(g_float64);
  //m_pCurRpcArry.reset ();// m_pCurRpcArry = NULL;
  m_defFile[0] = 0;
  strCpy (R"(${PROJECT_SOURCE_DIR}/bin)", m_outPutPath);
}

const char* msgTool::findDataType(const char* szT)
{
	cpchar* cp = m_dataTypeSet.GetNode(szT);
	return NULL == cp ? NULL : *cp;
}

msgTool::~msgTool()
{
	//s_delRpcArry(m_pCurRpcArry);
}

bool  msgTool::isBaseDataType(const char* szType)
{
	/*cpchar* pF =*/return  m_dataTypeSet.find(szType);
//	return NULL != pF;
}

int msgTool::procRpc1(rapidxml::xml_node<char> * pRpc, msgFileInfo& rInfo)
{
	//rTrace (__FUNCTION__<<" 000");
	int nRet = 0;
	assert(!m_pCurRpcArry);
	//rTrace (__FUNCTION__<<" 000 aaa");
	m_pCurRpcArry = std::make_shared<rpcMgr::rpcArryInfo> ();//s_newRpcArry();
	strNCpy(m_pCurRpcArry->m_name, rpcMgr::c_nameSize, pRpc->name());
	auto pMsgG = std::make_shared <msgGroup>();
	strCpy (pRpc->name (), pMsgG->name);
	//rTrace (__FUNCTION__<<" 000 bbb");
	rapidxml::xml_attribute<char> * p2JS = pRpc->first_attribute(g_2js);
	if(p2JS) {
		m_pCurRpcArry->m_b2js = atoi(p2JS->value()) != 0;
	}
	for(rapidxml::xml_node<>* pCall = pRpc->first_node(); NULL != pCall; pCall = pCall->next_sibling()) {
		nRet =  procStruct(pCall);
		assert(nRet < 2);
		if(nRet >= 2) {
			rError ("procStruct != 0");
			//s_delRpcArry(m_pCurRpcArry);
			return 1;
		}
	}
	//rTrace (__FUNCTION__<<" 111");
  for(rapidxml::xml_node<>* pCall = pRpc->first_node(); NULL != pCall; pCall = pCall->next_sibling()) {
	nRet =  procRpc2(pCall);
	assert(0 == nRet);
	if(0 != nRet) {
		//s_delRpcArry(m_pCurRpcArry);
		return 1;
	}
  }
  //rTrace (__FUNCTION__<<" 222");
  //rpcMgr::single().insert(m_pCurRpcArry.get());
  tSingleton<rpcMgr>::single().insert(m_pCurRpcArry);
  //rTrace (__FUNCTION__<<" 222 aaa");
  m_pCurRpcArry.reset ();
  rInfo.msgGroupS.push_back(pMsgG);
  return 0;
}

int msgTool::procStruct(rapidxml::xml_node<char> * pRpc)
{
	//rTrace (__FUNCTION__);
	rapidxml::xml_node<>* pAsk = pRpc->first_node(g_Ask);
	if(pAsk) {
		return 1;//It's msg  Not Struct
	}
	rapidxml::xml_node<>* pRet = pRpc->first_node(g_Ret);
	assert(!pRet);
	if(pRet) {
		rError ("is not a msg and not a struct");
		return 2;// It is not a msg and not a struct
	}
	//rpcMgr::structInfo* pSt = s_newStruct();
	auto pSt = std::make_shared<rpcMgr::structInfo>();
	rapidxml::xml_attribute<char> * pCommit = pRpc->first_attribute(g_commit);
	if(pCommit) {
		strNCpy(pSt->m_strCommit, NameSize, pCommit->value());
	} else {
		pSt->m_strCommit[0] = 0;
	}

	int nR = procStructData(pRpc,  *pSt);
	assert(0 == nR);
	if(0 != nR) {
		return 3;
	}
	
	strNCpy(pSt->m_name, NameSize, pRpc->name());
	m_pCurRpcArry->m_pStructInfoSet [pSt->m_name] = pSt;//m_pCurRpcArry->m_pStructInfoSet.insert(pSt);
	m_pCurRpcArry->m_pVst.push_back(pSt);
	return 0;
}

int msgTool::procRpc2(rapidxml::xml_node<char> * pRpc)
{
	rapidxml::xml_node<>* pAsk = pRpc->first_node(g_Ask);
	if(NULL == pAsk)
	{
		//pAsk = pRpc;
		return 0;//It's Struct Not msg  
	}
	//rpcMgr::rpcInfo* pRpcInfo = s_newRpc();
	auto pRpcInfo = std::make_shared<rpcMgr::rpcInfo>();
	pRpcInfo->setName(pRpc->name());
	std::string strMsg = m_pCurRpcArry->name();
	strMsg += "MsgID_";
	pRpcInfo->m_ask.m_msgIDName  = strMsg;
	pRpcInfo->m_ask.m_msgIDName+=pRpcInfo->m_ask.name();

	pRpcInfo->m_ret.m_msgIDName  = strMsg;
	pRpcInfo->m_ret.m_msgIDName+= pRpcInfo->m_ret.name();

	int nRet = procMsg(pAsk, pRpcInfo->m_ask);	
	if(0 != nRet) {
		//s_delRpc(pRpcInfo);
		return 2;
	}
	rapidxml::xml_attribute<char> * pCommit = pRpc->first_attribute(g_commit);
	if(pCommit)
	{
		strNCpy(pRpcInfo->m_strCommit, NameSize, pCommit->value());
		std::string strT = pRpcInfo->m_strCommit;
		strT += " ask";
		strNCpy(pRpcInfo->m_ask.m_strCommit, NameSize, strT.c_str());
		std::string strR = pRpcInfo->m_strCommit;
		strR += " ask";
		strNCpy(pRpcInfo->m_ret.m_strCommit, NameSize, strR.c_str());
	}
	else
	{
		pRpcInfo->m_strCommit[0] = 0;
		pRpcInfo->m_ask.m_strCommit[0] = 0;
		pRpcInfo->m_ret.m_strCommit[0] = 0;
	}
	//snprintf(pRpcInfo->m_ask.m_name, rpcMgr::c_nameSize, "%sAsk", pRpcInfo->name());	
	rapidxml::xml_node<>* pRet = pRpc->first_node(g_Ret);
	if(NULL == pRet)
	{
		pRpcInfo->m_bRet = false;
	}
	else
	{
		nRet = procMsg(pRet, pRpcInfo->m_ret);
		if(0 != nRet) {
			//s_delRpc(pRpcInfo);
			return 4;
		}
		pRpcInfo->m_bRet = true;
		//snprintf(pRpcInfo->m_ret.m_name, rpcMgr::c_nameSize, "%sRet", pRpcInfo->name());	
	}
	
	//m_pCurRpcArry->m_pRpcInfoSet.insert(pRpcInfo);	
	m_pCurRpcArry->m_pRpcInfoSet.push_back(pRpcInfo);
	return 0;
}

int msgTool::procStructData(rapidxml::xml_node<char> * pMsg, rpcMgr::structInfo& rMsg)
{
	//rTrace (__FUNCTION__);
	rapidxml::xml_attribute<char> * pRWP = pMsg->first_attribute(g_RWPack);
	if(pRWP)
	{
		rMsg.m_bRwPack = atoi(pRWP->value()) != 0;
	}
	rpcMgr::dataVector temp;
	for(rapidxml::xml_node<>* pData = pMsg->first_node(); NULL != pData; pData = pData->next_sibling())
	{
		rpcMgr::dataInfo  tInfo;
		rMsg.m_vData.push_back(tInfo);
		rpcMgr::dataInfo& rData = *(rMsg.m_vData.rbegin());//rMsg.m_vData.push();
		rData.m_length = 1;
		rData.m_haveSize = false;
		procData(pData, rData);
		if(rData.m_length > 1 && rData.m_haveSize)
		{
			temp.push_back(rData);
			rMsg.m_vData.pop_back();
		}
	}
	udword udwS =  temp.size();
	//rpcMgr::dataInfo* pArry = temp.arry();
	auto& pArry = temp;
	for(udword i = 0; i < udwS; i++) {
		rMsg.m_vData.push_back(pArry[i]);
	}
	return 0;
}

int msgTool::procMsg(rapidxml::xml_node<char> * pMsg, rpcMgr::msgInfo& rMsg)
{
	rapidxml::xml_attribute<char> * pPackType= pMsg->first_attribute(g_packetType);
	if(pPackType)
	{
		if(0 == strcmp(pPackType->value(), g_pack))
		{ 
			rMsg.m_packType = rpcMgr:: packetType::ePack;
		}
	}
	
	int nR = procStructData(pMsg, rMsg);
	if(0 != nR)
	{
		return 1;
	}
	rapidxml::xml_attribute<char> * pMsgID = pMsg->first_attribute(g_msgID);
	if(pMsgID)
	{
		rMsg.m_msgID = (int)(atoi(pMsgID->value()));
		rMsg.m_bAutoWriteMsg = false;
		m_pCurRpcArry->m_nextMsgID = rMsg.m_msgID;
	}
	else
	{
		rMsg.m_msgID = m_pCurRpcArry->m_nextMsgID;
	}
	m_pCurRpcArry->m_nextMsgID++;
	
	if(rMsg.m_vData.size())
	{
		rMsg.m_strBodyMaxSize = "sizeof(";
		rMsg.m_strBodyMaxSize += rMsg.m_name;
		rMsg.m_strBodyMaxSize += "Pack)";
	}
	else
	{
		rMsg.m_strBodyMaxSize = "0";
	}

	rapidxml::xml_attribute<char> * pMaxPackSize = pMsg->first_attribute(g_bodyMaxSize);
	if(pMaxPackSize)
	{
		rMsg.m_strBodyMaxSize = pMaxPackSize->value();
	}
	return 0;
}

int msgTool::procData(rapidxml::xml_node<char> * pData, rpcMgr::dataInfo& rData)
{
	strNCpy(rData.m_name, rpcMgr::c_nameSize, pData->name());
	//rTrace (__FUNCTION__<<" name = "<<rData.m_name);
	rapidxml::xml_attribute<char> * pDataType = pData->first_attribute(g_dataType);
	assert(pDataType);
	if(NULL == pDataType)
	{
		return 2;
	}
	strNCpy(rData.m_type, rpcMgr::c_nameSize, pDataType->value());

	rapidxml::xml_attribute<char> * pLen= pData->first_attribute(g_length);
	if(pLen)
	{
		rData.m_length = atoi(pLen->value());
	}

	rapidxml::xml_attribute<char> * pHaveSize = pData->first_attribute(g_haveSize);
	if(pHaveSize)
	{
		rData.m_haveSize= atoi(pHaveSize->value()) != 0;
	}
	rData.m_wordSize = false;
	rapidxml::xml_attribute<char> * pWorldSize = pData->first_attribute(g_worldSize);
	if(pWorldSize)
	{
		rData.m_wordSize = atoi(pWorldSize->value()) != 0;
	}
	rData.m_zeroEnd = false;
	rapidxml::xml_attribute<char> * pZ = pData->first_attribute(g_zeroEnd);
	if(pZ)
	{
		rData.m_zeroEnd = atoi(pZ->value()) != 0;
	}
	rapidxml::xml_attribute<char> * pCommit = pData->first_attribute(g_commit);
	if(pCommit)
	{
		strNCpy(rData.m_strCommit, NameSize, pCommit->value()) ;
	}
	else
	{
		rData.m_strCommit[0] = 0;
	}
	return 0;
}

int msgTool::startProcMsg(const char* szFile)
{
	rDebug (__FUNCTION__<<" szFile = "<<szFile);
	int nRet = 0;
	std::string strT = projectHome ();
	strT += szFile;
    rapidxml::file<> fdoc(strT.c_str());
	rapidxml::xml_document<> doc;
	doc.parse<0>(fdoc.data());
	rapidxml::xml_node<> *root = doc.first_node();

	auto& rNameS = getMsgFileNameS ();
	auto pF = std::make_shared<msgFileInfo>();
	strCpy (szFile, pF->name);

    for(rapidxml::xml_node<char> * pRpc = root->first_node();  NULL != pRpc; pRpc = pRpc->next_sibling()) {
		if(0 != strcmp(g_Rpc, pRpc->name())) {
			continue;
		}
		for(rapidxml::xml_node<char> * pRpcArry = pRpc->first_node();  NULL != pRpcArry;  pRpcArry = pRpcArry->next_sibling()) {
			auto pMsgG = std::make_shared<msgGroup>();
			strCpy (pRpcArry->name (), pMsgG->name);
			nRet =	procRpc1(pRpcArry, *(pF.get()));
			assert(0 == nRet);
			if(0 != nRet) {
				rError ("procRpc1 != 0");
				return 1;
			}
			pF->msgGroupS.push_back (pMsgG);
		}
	}
	rNameS.push_back (pF);
	return 0;
}

const char* msgTool::projectHome ()
{
	return m_projectHome.get ();
}

void  msgTool::getDefMsgPath (std::unique_ptr<char[]>& path)
{
	std::string strT = projectHome ();
	strT += "defMsg/src/";
	strCpy (strT.c_str (), path);
}

void msgTool:: setProjectHome (const char* szPath)
{
	strCpy (szPath, m_projectHome);
}

const char* msgTool::frameHome ()
{
	return m_frameHome.get ();
}

const char* msgTool::depIncludeHome ()
{
	return m_depIncludeHome.get ();
}

const char* msgTool::depLibHome ()
{
	return m_depLibHome.get ();
}

void msgTool::setDepIncludeHome (const char* szPath)
{
	strCpy (szPath, m_depIncludeHome);
}

void msgTool::setDepLibHome (const char* szPath)
{
	strCpy (szPath, m_depLibHome);
}

void msgTool::setFrameHome (const char* szPath)
{
	strCpy (szPath, m_frameHome);
}

const char* msgTool::frameBinPath ()
{
	return m_frameBinPath.get();
}

const char* msgTool::frameLibPath ()
{
	return m_frameLibPath.get();
}

const char* msgTool::outPutPath ()
{
	return m_outPutPath.get();
}

msgTool::msgFileNameV&  msgTool::getMsgFileNameS ()
{
	return m_msgFileNameS;
}

int msgTool::start(const char* szFile)
{
	int nRet = 0;
    rapidxml::file<> fdoc(szFile);
	rapidxml::xml_document<> doc;
	doc.parse<0>(fdoc.data());
	rapidxml::xml_node<> *root = doc.first_node();

	for(rapidxml::xml_node<char> * pRpc = root->first_node();  NULL != pRpc; pRpc = pRpc->next_sibling()) {
		if(0 == strcmp(pRpc->name(), "projectHome")) {
			auto szPath = pRpc->value ();
			tSingleton<msgTool>::single().setProjectHome (szPath);
			continue;
		} if(0 == strcmp(pRpc->name(), "frameHome")) {
			auto szPath = pRpc->value ();
			tSingleton<msgTool>::single().setFrameHome(szPath);
			continue;
		} if(0 == strcmp(pRpc->name(), "depIncludeHome")) {
			auto szPath = pRpc->value ();
			tSingleton<msgTool>::single().setDepIncludeHome(szPath);
			continue;
		} if(0 == strcmp(pRpc->name(), "depLibHome")) {
			auto szPath = pRpc->value ();
			tSingleton<msgTool>::single().setDepLibHome(szPath);
			continue;
		} if(0 == strcmp(pRpc->name(), "frameBinPath")) {
			strCpy (pRpc->value (), m_frameBinPath);
			continue;
		} if(0 == strcmp(pRpc->name(), "frameLibPath")) {
			strCpy (pRpc->value (), m_frameLibPath);
			continue;
		} if(0 == strcmp(pRpc->name(), "outPutPath")) {
			strCpy (pRpc->value (), m_outPutPath);
			continue;
		}
	}
    for(rapidxml::xml_node<char> * pRpc = root->first_node();  NULL != pRpc; pRpc = pRpc->next_sibling()) {
		if(0 != strcmp(pRpc->name(),"msgFile")) {
			continue;
		}

		rapidxml::xml_attribute<char> * pAppType = pRpc->first_attribute("file");
		if(!pAppType) {
			rTrace ("not find attr file");
			return 2;
		}
		int nR = startProcMsg(pAppType->value());	
		rTrace (__FUNCTION__<<" after startProcMsg nR = "<<nR);
		if(0 != nR) {
			rWarn (__FUNCTION__<<" 0 != nR");
			return 1;
		}
	}
	auto& rRpc = tSingleton<rpcMgr>::single();
	initPath ();
	//rRpc.mkDirS ();
	rRpc.chickDataTypeS();
	//rTrace (__FUNCTION__<<" 000");
	procAppS(root);
	//rTrace (__FUNCTION__<<" 111");
	rRpc.writeMsgPmpID();
	//rTrace (__FUNCTION__<<" 222");
	rRpc.writeMsgID();
	//rTrace (__FUNCTION__<<" 333");
	rRpc.writeRpcArryH();
	//rTrace (__FUNCTION__<<" 444");
	rRpc.writeRpcArryCpp();
	//rTrace (__FUNCTION__<<" 555");
	rRpc.writeInitMsgCpp();
	//rTrace (__FUNCTION__<<" 666");
	rRpc.writeRpcArry2Ts();
	//rTrace (__FUNCTION__<<" 777");
//	rpcMgr::single().writeRpcArry2js();
	auto& rApp = tSingleton<appMgr>::single();
	rApp.procApp();
	rTrace (__FUNCTION__<<" 888");
	return 0;
}

void msgTool:: initPath()
{
	myMkdir (projectHome ());
	std::unique_ptr<char[]> path;
	getDefMsgPath (path);
	myMkdir (path.get());
}

int msgTool::procCmd1(char* szText)
{
	const int c_BuffSize = 128;
	char szBuff[c_BuffSize];
	int nL = strlen(szText);
	assert(nL < c_BuffSize);
	strNCpy(szBuff, c_BuffSize, szText);
	const int c_RetBuffSize = 3;
	char* retA[c_RetBuffSize];
	int nR = strR(szBuff, '=', retA, c_RetBuffSize);
	assert(2==nR);
	if(2 != nR)
	{
		return 1;
	}
		
	if(0 == strcmp(retA[0], "defFile"))
	{
		strNCpy(m_defFile, rpcMgr::c_dirSize, retA[1]);	
	}
	
	return 0;
}

int msgTool::init(int nArgC, char* argS[])
{
	auto nInitLog = initLog ("genTool", "genTool.log", 0);
	if (0 != nInitLog) {
		std::cout<<"initLog error nInitLog = "<<nInitLog<<std::endl;
		return 3;
	}

	rDebug (__FUNCTION__<<" nArgC = "<<nArgC);
	for(int i = 1; i < nArgC; i++)
	{
		int nR = procCmd1(argS[i]);
		if(0 != nR)
		{
			return 1;
		}
	}
	int nRet = start(m_defFile);
	if(0 != nRet)
	{
		return 2;
	}
	return 0;
}

int msgTool::procAppS(rapidxml::xml_node<char> * pRoot)
{
	/*
	auto nInitLog = initLog ("genTool", "genTool.log", 0);
	if (0 != nInitLog) {
		std::cout<<"initLog error nInitLog = "<<nInitLog<<std::endl;
		return 1;
	}
	rInfo ("Loger init OK");
	*/
	auto& rRealS = tSingleton<appMgr>::single ().realServerList ();
	for(rapidxml::xml_node<char> * pApp = pRoot->first_node();  NULL != pApp; pApp= pApp->next_sibling()) {
		if(0 != strcmp(g_app, pApp->name())) {
			continue;
		}
		for(rapidxml::xml_node<char> * pC = pApp->first_node();  NULL != pC; pC = pC->next_sibling()) {
			auto pReal = std::make_shared<realServer>();
			pReal->setName (pC->name ());
			for(auto pCC = pC->first_node();  NULL != pCC; pCC = pCC->next_sibling()) {
				if (0 == strcmp ("module", pCC->name ())) {
					for(auto pCCC = pCC->first_node();  NULL != pCCC; pCCC = pCCC->next_sibling()) {
						int nR = procOnceApp(pCCC, pReal.get());
						assert(0==nR);
						if(0 != nR) {
							return 1;
						}
					}
				}
			}
			rRealS.push_back (pReal);
		}
	}
	return 0;
}
/*
static app* newApp()
{
	IMemMgr* pIM = GetMemMgr();
	app* pApp = (app*)(pIM->Malloc(sizeof(app)));
	pApp = new(pApp) app;
	return pApp;
}

static void delApp(app* p)
{
	if(p)
	{
		p->~app();
		SD(p);
	}
}
*/
int msgTool::procOnceApp(rapidxml::xml_node<char> * pAppNode, realServer* pReal)
{
	auto pCurApp = std::make_shared<app>();//m_pCurApp = newApp();	
	auto pCurModel = std::make_shared<logicModel> ();
	pCurModel->setName (pAppNode->name ());
	rapidxml::xml_attribute<char> * pProjectDir = pAppNode->first_attribute(g_projectDir );
	if(pProjectDir)
	{
		strNCpy(pCurApp->m_projectDir,DirSize, pProjectDir->value());
	}
	rapidxml::xml_attribute<char> * pAppType = pAppNode->first_attribute("appType");
	if(pAppType)
	{
		if(0 == strcmp(pAppType->value(), "ts"))
		{
			return 0;
		}
	}
	strNCpy(pCurApp->m_name, NameSize, pAppNode->name());
	for(rapidxml::xml_node<char> * pC = pAppNode->first_node();  NULL != pC;  pC = pC->next_sibling())
	{
		if(0 == strcmp(g_procRpc, pC->name()))
		{
			for(rapidxml::xml_node<char> * pArry = pC->first_node();  NULL != pArry;  pArry = pArry->next_sibling())
			{
				int n = appProcOnceRpcArry(pCurApp.get(), pArry);
				assert(0 == n);
				if(0 != n)
				{
					return 1;
				}
			}
		}

		if(0 == strcmp(g_msgPmp, pC->name()))
		{
			for(rapidxml::xml_node<char> * pPmp = pC->first_node();  NULL != pPmp;  pPmp = pPmp->next_sibling())
			{
				const char* szClassName = NULL;
				rapidxml::xml_attribute<char> * pClass = pPmp->first_attribute("className");
				if(pClass)
				{
					szClassName = pClass->value();
				}

				const char* szNextPmp = NULL;
				rapidxml::xml_attribute<char> * pNextPmp = pPmp->first_attribute("nextpmp");
				if(pNextPmp)
				{
					szNextPmp = pNextPmp->value();
				}

				for(rapidxml::xml_node<char> * pP= pPmp->first_node();  NULL != pP;  pP = pP->next_sibling())
				{

					if(0 != strcmp(g_procRpc, pP->name()))
					{
						continue;
					}
					for(rapidxml::xml_node<char> * pArry = pP->first_node();  NULL != pArry;  pArry = pArry->next_sibling())
					{
						int n = appProcOnceRpcArry(pCurApp.get(), pArry, pPmp->name(), szClassName, szNextPmp);
						assert(0 == n);
						if(0 != n)
						{
							return 1;
						}
					}
				}	
			}
		}
	}
	auto& rAppS = pReal->getAppList ();	
	rAppS.push_back (pCurApp);
	auto& rMap = pReal->getModelMap ();
	rMap[pCurModel->name ()] = pCurModel;
	return 0;
}

int msgTool::appProcOnceRpcArry(app* pCurApp, rapidxml::xml_node<char> * pArry, const char* szPmpName, const char* szClassName, const char* szNextPmp)
{
	for(rapidxml::xml_node<char> * pC = pArry->first_node();  NULL != pC;  pC = pC->next_sibling())
	{
		std::string str = pArry->name();
		str +=":";
		str +=pC->name();
		bool bAsk = true;
		bool bExit = false;
		rapidxml::xml_attribute<char> *pEx = pC->first_attribute(g_Exit);
		if(pEx)
		{
			bExit = 0 != atoi(pEx->value());	
		}

		rapidxml::xml_attribute<char> * pAT = pC->first_attribute(g_askType);
		if(pAT)
		{
			if(0 == strCaseCmp(pAT->value(), g_Exit))
			{
				continue;
			}

			int n = strCaseCmp(pAT->value(), g_Ask);
			if(0 == n)
			{
				bAsk = true;
			}
			else
			{
				n = strCaseCmp(pAT->value(), g_Ret);
				assert(0 == n);
				if(0 != n)
				{
					return 1;
				}
				bAsk = false;
			}
		}
		char* szConName = NULL;	
		rapidxml::xml_attribute<char> * pConName = pC->first_attribute("conName");
		if(pConName)
		{
			szConName = pConName->value();
		}

		int sendType = 0;
		rapidxml::xml_attribute<char> * pSendType = pC->first_attribute("sendType");
		if(pSendType)
		{
			sendType = atoi(pSendType->value());
		}
		bool bPass = false;
		rapidxml::xml_attribute<char> * pPass = pC->first_attribute("pass");
		if(pPass)
		{
			bPass = atoi(pPass->value());
		}
		bool bNP= false;
		rapidxml::xml_attribute<char> * pNP = pC->first_attribute("neetPlayer");
		if(pNP)
		{
			bNP = atoi(pNP->value());
		}
		pCurApp->addProcRpcInfo(str.c_str(), bAsk, bExit, szPmpName, szClassName, szNextPmp, szConName, sendType, bPass, pNP);
	}
		return 0;
}
