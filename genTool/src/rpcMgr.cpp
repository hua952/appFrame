#include<fstream>
#include<sstream>
#include<iostream>
#include "rpcMgr.h"
#include"strFun.h"
#include"tSingleton.h"
#include "msgTool.h"
#include "rLog.h"

rpcMgr::nameBase::nameBase()
{
	m_name[0] = 0;
}

const char* rpcMgr::nameBase::name()const
{
	return m_name;
}
rpcMgr::structInfo::structInfo()
{
	m_bRwPack = false;
}

rpcMgr::structInfo::~structInfo()
{
}

rpcMgr::msgInfo::msgInfo()
{	
	m_bAutoWriteMsg = true;
	m_packType = rpcMgr:: packetType::ePack;
	m_msgID = 0;
}

rpcMgr::msgInfo::~msgInfo()
{
}

void rpcMgr::msgInfo::setName(const char* szName)
{
	strNCpy(m_name, c_nameSize, szName);
	m_packName = m_name;
	m_packName += "Pack";
}

void	rpcMgr::rpcInfo:: setName(const char* szName)
{
	strNCpy(m_name, c_nameSize, szName);
	std::string strAsk = m_name;
	strAsk += "Ask";
	m_ask.setName(strAsk.c_str());

	std::string strRet = m_name;
	strRet += "Ret";
	m_ret.setName(strRet.c_str());
}

bool rpcMgr::pRpcInfoCmp:: operator()(const pRpcInfo& pA, const pRpcInfo& pB)const
{
	if(pA->m_ask.m_msgID < pB->m_ask.m_msgID)
	{
		return true;
	}
	if(pB->m_ask.m_msgID < pA->m_ask.m_msgID)
	{
		return false;
	}
	return strcmp(pA->m_name, pB->m_name) < 0;
}

rpcMgr::rpcArryInfo::rpcArryInfo()
{
	m_nextMsgID = 0;
}
/*
static bool s_vistL(void* pU, rpcMgr::pRpcInfo& p)
{
	rpcMgr::rpcInfo* pB = p;// dynamic_cast<rpcMgr::rpcInfo*>(p);
	if(pB)
	{
		pB->~rpcInfo();
		SD(pB)
	}
	return true;
}
static bool s_vistStr(void* pU, pIStringKey& p)
{
	rpcMgr::structInfo* pB = dynamic_cast<rpcMgr::structInfo*>(p);
	if(pB)
	{
		pB->~structInfo();
		SD(pB)
	}
	return true;
}
*/

rpcMgr::rpcArryInfo::~rpcArryInfo()
{
	//m_pRpcInfoSet.visit(NULL, s_vistL);
	//m_pStructInfoSet.visit(NULL, s_vistStr);
}

rpcMgr::rpcMgr()
{
	//strNCpy(m_cppLibDir, c_dirSize, "../libMsg/src");
	strNCpy(m_jsLibDir, c_dirSize, "../CCAsianGame/src");
	strNCpy(m_tsLibDir, c_dirSize, "../msg2ts/src");
    //m_OrderS = 0;
}

int	rpcMgr::mkDirS ()
{
	int nRet = 0;
	//myMkdir (projectHome());
	//myMkdir (m_cppLibDir);
	//myMkdir (m_jsLibDir);
	//myMkdir (m_tsLibDir);
	return nRet;
}

rpcMgr::~rpcMgr()
{
	/*
	arrayMapIndexType nS = m_pRpcArryS.size();
	for(arrayMapIndexType i = 0; i < nS; i++)
	{
		pIStringKey *ppR = m_pRpcArryS.getNodeByIndex(i);
		if(NULL == ppR)
		{
			continue;
		}
		rpcArryInfo* pa = dynamic_cast<rpcArryInfo*>(*ppR);
		if(NULL == pa)
		{
			continue;
		}
		pa->~rpcArryInfo();
		SD(pa);
	}
	*/
}

bool rpcMgr::insert(std::shared_ptr<rpcMgr::rpcArryInfo>/*rpcArryInfo**/ pInfo)
{
	//m_rpcV.push(pInfo);
	m_rpcV.push_back(pInfo);
	//return m_pRpcArryS.insert(pInfo);
	m_pRpcArryS.push_back(pInfo);
	return true;
}

 rpcMgr::rpcArryInfo*  rpcMgr::getRpcArry(const char* arryName)
{
	/*
nameBase temp;
	strNCpy(temp.m_name, c_nameSize, arryName);
	pIStringKey* ppR = m_pRpcArryS.GetNode(&temp);
	if(NULL == ppR)
	{
		return NULL;
	}
	pRpcArryInfo pA = dynamic_cast<pRpcArryInfo>(*ppR); 
	assert(pA);
	return pA;
	*/
	auto& rA = m_pRpcArryS;
	for (auto it = rA.begin (); rA.end () != it; ++it) {
		auto p = it->get();
		if (0 == strcmp (arryName, p->name())) {
			return it->get ();
		}
	}
	return nullptr;
}

rpcMgr::rpcInfo* rpcMgr::find(const char* arryName, const char* name)
{
	/*
	nameBase temp;
	strNCpy(temp.m_name, c_nameSize, arryName);
	pIStringKey* ppR = m_pRpcArryS.GetNode(&temp);
	if(NULL == ppR)
	{
		return NULL;
	}
	pRpcArryInfo pA = dynamic_cast<pRpcArryInfo>(*ppR); 
	*/
	auto pA = getRpcArry (arryName);
	assert(pA);
	if(NULL == pA)
	{
		return NULL;
	}

	rpcInfo*    pRet = nullptr;
	auto& rRpcS = pA->m_pRpcInfoSet;
	/*
	rTrace(__FUNCTION__<<" pA->m_pRpcInfoSet.size () = "<<pA->m_pRpcInfoSet.size ()
			<<" find name = "<<name<<" arryName = "<<arryName);
			*/
	for (auto it = rRpcS.begin (); rRpcS.end () != it; ++it) {
		auto& pR = *it;
		//rTrace (__FUNCTION__<<" it name = "<<pR->m_name);
		if(strcmp (name, pR->m_name) == 0) {
			pRet = pR.get();
			break;
		}
	}
	/*
	struct strVST
	{
		std::string strName;
		rpcInfo*    pRet;
	};
	strVST vst;
	vst.strName = name;
	vst.pRet = NULL;
	
	pA->m_pRpcInfoSet.visit(&vst, [](void* pU, pRpcInfo& pR){
				strVST* pV = (strVST*)pU;
				if(!pV->pRet)
				{
					if(pV->strName == pR->m_name)
					{
						pV->pRet = pR;
					}
				}
				return true;
			});
	return vst.pRet;
	*/
	return pRet;
}

int	rpcMgr::chickDataTypeS()
{
	arrayMapIndexType nS = m_pRpcArryS.size();
	for(arrayMapIndexType i = 0; i < nS; i++)
	{
		/*
		pIStringKey *ppR = m_pRpcArryS.getNodeByIndex(i);
		if(NULL == ppR)
		{
			continue;
		}
		rpcArryInfo* pa = dynamic_cast<rpcArryInfo*>(*ppR);
		if(NULL == pa)
		{
			continue;
		}
		*/
		auto pa = m_pRpcArryS[i].get();
		std::vector<structInfo*> vSt; //tVector<structInfo*> vSt;
		auto& rStructInfoS = pa->m_pStructInfoSet;
		for (auto it = rStructInfoS.begin (); rStructInfoS.end () != it; ++it) {
			vSt.push_back(it->second.get());
		}
		/*
		pa->m_pStructInfoSet.visit(&vSt, [](void* pU, pIStringKey& p){
					tVector<structInfo*>& vSt = *((tVector<structInfo*>*)(pU));
					structInfo* pS = dynamic_cast<structInfo*>(p); 
					vSt.push(pS);
					return true;
				});
				*/
		udword stNum = vSt.size();
		auto& pStA = vSt;//structInfo** pStA = vSt.arry();
		for(udword i = 0; i < stNum; i++)
		{

			structInfo* pS = pStA[i];
			udword nSize =  pS->m_vData.size();
			//dataInfo* pArr = pS->m_vData.arry();
			auto& pArr = pS->m_vData;
			for(udword j = 0; j < nSize; j++)
			{
				auto &rD = pArr[j];
				//bool bF = msgTool::single().isBaseDataType(rD.m_type);
				bool bF = tSingleton<msgTool>::single().isBaseDataType(rD.m_type);
				if(bF) {
					continue;
				}
									
				//nameBase temp;
				//strNCpy(temp.m_name, rD.m_type, c_nameSize);
				auto it = pa->m_pStructInfoSet.find (rD.m_type);//pIStringKey* ppS = pa->m_pStructInfoSet.find(&temp);
				if (pa->m_pStructInfoSet.end () == it)//if(NULL == ppS)
				{
					std::cout<<"Type "<<rD.m_type<<" Not Find"<<std::endl;
					exit(1);
				}
			}
		}
	}
	return 0;
}

void rpcMgr:: writeMsgPmpID()
{
	const int c_BuffSize = 128;
	char szFilename[c_BuffSize]={0};
	std::unique_ptr<char[]> path;
	tSingleton<msgTool>::single().getDefMsgPath(path);
	snprintf(szFilename, c_BuffSize, "%s/msgPmpID.h", path.get());
	std::ofstream os(szFilename);

	os<<"#ifndef _msgPmpID_h__"<<std::endl
		<<"#define _msgPmpID_h__"<<std::endl
		<<std::endl
		<<"enum msgPmpID"<<std::endl
		<<"{";
	for (auto it = m_rpcV.begin (); m_rpcV.end () != it; ++it) {
		auto& pA = *it;
		os<<"	msgPmpID_"<<pA->name()<<","<<std::endl;
	}
	/*
	m_rpcV.visit(&os, [](void* pU, pRpcArryInfo& pA){
			std::ofstream& os = *((std::ofstream*)(pU));
				os<<"	msgPmpID_"<<pA->name()<<","<<std::endl;
				return true;
			});
	*/	
	os<<"};"<<std::endl
		<<std::endl;
	/*
	for(arrayMapIndexType i = 0; i <  m_pRpcArryS.size(); i++)
	for (auto it = m_pRpcArryS.begin(); m_pRpcArryS.end() != it; ++it)
	{
		auto pA = it->get();
		os<<"#define "<<pA->name()<<"2FullMsg(p) ((uword)((msgPmpID_"<<pA->name()<<" * 100)+p))"<<std::endl;
	}
	*/
	for (auto it = m_rpcV.begin (); m_rpcV.end () != it; ++it) {
		auto& pA = *it;
		os<<"#define "<<pA->name()<<"2FullMsg(p) ((uword)((msgPmpID_"<<pA->name()<<" * 100)+p))"<<std::endl;
	}
	os<<std::endl
		<<"#endif";
}


void rpcMgr:: writeMsgID()
{
	//arrayMapIndexType nS = m_pRpcArryS.size();
	//for(arrayMapIndexType i = 0; i < nS; i++)
	for (auto it = m_pRpcArryS.begin (); m_pRpcArryS.end () != it; ++it)
	{
		/*
		pIStringKey* pp = m_pRpcArryS.getNodeByIndex(i);
		if(NULL == pp)
		{
			continue;
		}
		pRpcArryInfo p = dynamic_cast<pRpcArryInfo>(*pp);
		*/
		auto p = it->get();
		if(p)
		{
			writeOnceMsgID(p, false);
			writeOnceMsgID(p, true);
		}
	}
}

namespace temp
{
	struct visitData
	{
		rpcMgr* pMgr;
		rpcMgr::rpcArryInfo* pA;
		std::ofstream* ps;
		std::stringstream stS;
		bool bAuto;
	};
}
static void s_writeMsgID(rpcMgr::rpcArryInfo* pA, rpcMgr::msgInfo& rMsg, bool bAuto, std::ofstream& os)
{

	assert(rMsg.m_msgID < 100);
	os<<"	"<<rMsg.m_msgIDName<<" = "<<rMsg.m_msgID<<","<<std::endl;
}
/*
static bool s_visit(void* pU, rpcMgr::pRpcInfo& pS)
{
	rpcMgr::pRpcInfo pRpc =  pS;//dynamic_cast<rpcMgr::pRpcInfo>(pS);
	assert(pRpc);
	temp::visitData * pD = (temp::visitData*)pU;
	std::ofstream& os = *pD->ps;
	 s_writeMsgID(pD->pA, pRpc->m_ask, pD->bAuto, os);
	if(!pRpc->m_bRet)
	{
		return true;
	}
	 s_writeMsgID(pD->pA, pRpc->m_ret, pD->bAuto, os);
	 return true;
}
*/
void rpcMgr:: writeOnceMsgID(rpcArryInfo* pA, bool bAuto)
{
	const int c_BuffSize = 128;
	char szFilename[c_BuffSize]={0};
	std::unique_ptr<char[]> path;
	tSingleton<msgTool>::single().getDefMsgPath(path);
	snprintf(szFilename, c_BuffSize, "%s/%sMsgID.h", path.get(), pA->name());
	std::ofstream os(szFilename);

	os<<"#ifndef _"<<pA->name()<<"MsgID_h__"<<std::endl
		<<"#define _"<<pA->name()<<"MsgID_h__"<<std::endl 
		<<"enum "<<pA->name()<<"MsgID"<<std::endl
		<<"{"<<std::endl;
	auto& rRpcS = pA->m_pRpcInfoSet;
	for (auto it = rRpcS.begin (); rRpcS.end () != it; ++it) {
		s_writeMsgID(pA, it->get()->m_ask, bAuto, os);
		if(it->get()->m_bRet) {
			s_writeMsgID(pA, it->get()->m_ret, bAuto, os);
		}
	}
	/*
	temp::visitData data;
	data.pMgr = this;
	data.ps = &os;
	data.pA = pA;
	data.bAuto = bAuto;
	pA->m_pRpcInfoSet.visit(&data, s_visit);
	*/
	os<<"};"<<std::endl;
	os<<"#endif";
}

void rpcMgr::writeRpcArryH()
{
	//arrayMapIndexType nS = m_pRpcArryS.size();
	//for(arrayMapIndexType i = 0; i < nS; i++)
	for (auto it = m_pRpcArryS.begin (); m_pRpcArryS.end() != it; ++it)
	{
		/*
		pIStringKey* pp = m_pRpcArryS.getNodeByIndex(i);
		if(NULL == pp)
		{
			continue;
		}
		pRpcArryInfo p = dynamic_cast<pRpcArryInfo>(*pp);
		*/
		auto p = it->get();
		if(p)
		{
			writeOnceRpcArryH(p);
		}
	}
}

static bool s_visitWriteRpcH(void* pU, rpcMgr::pRpcInfo& pS)
{
	rpcMgr::pRpcInfo pRpc = pS;// dynamic_cast<rpcMgr::pRpcInfo>(pS);
	assert(pRpc);
	temp::visitData * pD = (temp::visitData*)pU;
	std::ofstream& os = *pD->ps;

	pD->pMgr->writeMsgH(os, pRpc->m_ask);	
	if(pRpc->m_bRet)
	{
		pD->pMgr->writeMsgH(os, pRpc->m_ret);
	}
	return true;
}


void	rpcMgr::writeMsgH(std::ofstream& os, msgInfo& rMsg)
{
	if(rMsg.m_vData.size() > 0)
	{
			os<<"struct "<<rMsg.name()<<"Pack"<<std::endl;
		writeVDataH("", os, rMsg);
	}
	std::string strBaseClass = "CMsgBase";
	
	os<<"class "<<rMsg.name()<<" :public "<<strBaseClass<<std::endl
		<<"{"<<std::endl
		<<"public:"<<std::endl;
	
	os<<"	"<<rMsg.name()<<"();"<<std::endl
		<<"	"<<rMsg.name()<<"(packetHead*p);"<<std::endl;
	dataVector& vData = rMsg.m_vData;
	
	udword udwS = vData.size();
	if(udwS > 0)
	{
		//dataInfo* pArr = vData.arry();
		dataVector& pArr = vData;
		dataInfo& rD = pArr[udwS -1];
		if(rMsg.m_bRwPack)
		{
			os<<"	virtual packetHead*   toPack();"<<std::endl
				<<"	virtual bool fromPack(packetHead* p);"<<std::endl;
		}
		else if(rD.m_length > 1 && rD.m_haveSize){
			os<<"	virtual packetHead*   toPack();"<<std::endl;
		}
		os<<"	"<<rMsg.name()<<"Pack*	pack();"<<std::endl;
	}
	os<<"};"<<std::endl<<std::endl;
}

void rpcMgr::writeVDataH(const char* szPer, std::ofstream& os, rpcMgr::structInfo& rSt)
{
	dataVector& vData = rSt.m_vData;
	os<<szPer<<"{";
	if(rSt.m_strCommit[0])
	{
		os<<"//"<<rSt.m_strCommit;
	}
	os<<std::endl;
	if(rSt.m_bRwPack)
	{
		os<<szPer<<"	void read(char* pBuff, udword& udwIndex);"<<std::endl
			<<szPer<<"	void write(char* pBuff, udword& udwIndex);"<<std::endl;
	}
	udword udwS = vData.size();
	//dataInfo* pArr = vData.arry();
	dataVector& pArr = vData;
	for(udword i = 0; i < udwS; i++)
	{
		dataInfo& rD = pArr[i];
		if(rD.m_length > 1 && rD.m_haveSize)
		{
			std::string strType = "udword";
			if(rD.m_wordSize)
			{
				strType = "uword";
			}
			os<<szPer<<"	"<<strType<<"	m_"<<rD.m_name<<"Num;"<<std::endl;
		}
		os<<szPer<<"	"<<rD.m_type<<"		m_"<<rD.m_name;
		if(rD.m_length > 1)
		{
			os<<"["<<rD.m_length<<"]";
		}
		os<<";";
		if(rD.m_strCommit[0])
		{
			os<<"//"<<rD.m_strCommit;
		}
		os<<std::endl;
	}
	os<<szPer<<"};"<<std::endl;
}

void rpcMgr::writeOnceRpcArryH(rpcArryInfo* pA)
{
	const int c_BuffSize = 128;
	char szFilename[c_BuffSize]={0};
	std::unique_ptr<char[]> path;
	tSingleton<msgTool>::single().getDefMsgPath(path);
	snprintf(szFilename, c_BuffSize, "%s/%sRpc.h", path.get(), pA->name());
	std::ofstream os(szFilename);
	
	os<<"#ifndef _"<<pA->name()<<"_rpc_h__"<<std::endl
		<<"#define _"<<pA->name()<<"_rpc_h__"<<std::endl
		<<"#include\"msg.h\""<<std::endl;
	udword udwS = pA->m_pVst.size();

	
	temp::visitData data;
	data.pMgr = this;
	data.ps = &os;
	data.pA = pA;
	//structInfo** ppA = pA->m_pVst.arry();
	auto& ppA = pA->m_pVst;
	for(udword i = 0; i < udwS; i++)
	{
		structInfo* pSt = ppA[i].get();// dynamic_cast<structInfo*>(pI);	
		if(pSt->m_vData.size()==  0)
		{
			return;
		}
		os<<"struct "<<pSt->name()<<std::endl;
		writeVDataH("", os, *pSt);
	}
	
	os<<std::endl;
	//pA->m_pRpcInfoSet.visit(&data,s_visitWriteRpcH);
	for (auto it = pA->m_pRpcInfoSet.begin (); pA->m_pRpcInfoSet.end () != it; ++it) {
		auto p = it->get();
		s_visitWriteRpcH (&data, p);
	}
	os<<"#endif";
}

void  rpcMgr:: writeInitMsgCpp()
{
	const int c_BuffSize = 128;
	char szFilename[c_BuffSize]={0};
	std::unique_ptr<char[]> path;
	tSingleton<msgTool>::single().getDefMsgPath(path);
	snprintf(szFilename, c_BuffSize, "%s/libMsg.cpp", path.get());
	std::ofstream os(szFilename);

	os<<"#include<stdlib.h>"<<std::endl
		<<"#include\"funLog.h\""<<std::endl
		<<"int initMsg()"<<std::endl
		<<"{"<<std::endl
		<<"LOG_FUN_CALL"<<std::endl
		<<"	int nRet = 0;"<<std::endl;

	//arrayMapIndexType nS = m_pRpcArryS.size();
	//for(arrayMapIndexType i = 0; i < nS; i++)
	int i = 0;
	for (auto it = m_pRpcArryS.begin (); m_pRpcArryS.end () != it; ++it)
	{
		/*
		pIStringKey* pp = m_pRpcArryS.getNodeByIndex(i);
		if(NULL == pp)
		{
			continue;
		}
		pRpcArryInfo p = dynamic_cast<pRpcArryInfo>(*pp);
		*/
		auto p = it->get();
		if(p)
		{
			os<<"	int "<<p->name()<<"MsgInfoReg();"<<std::endl
				<<"	nRet = "<<p->name()<<"MsgInfoReg();"<<std::endl
				<<"	if(0 != nRet)"<<std::endl
				<<"	{"<<std::endl
				<<"		return	"<<i+1<<";"<<std::endl
				<<"	}"<<std::endl;
		}
		i++;
	}
	os<<"	return 0;"<<std::endl
		<<"}"<<std::endl;
}

void  rpcMgr:: writeRpcArryCpp()
{
	//arrayMapIndexType nS = m_pRpcArryS.size();
	//for(arrayMapIndexType i = 0; i < nS; i++)
	for (auto it = m_pRpcArryS.begin(); m_pRpcArryS.end() != it; ++it)
	{
		/*
		pIStringKey* pp = m_pRpcArryS.getNodeByIndex(i);
		if(NULL == pp)
		{
			continue;
		}
		pRpcArryInfo p = dynamic_cast<pRpcArryInfo>(*pp);
		*/
		auto p = it->get();
		if(p)
		{
			writeOnceRpcArryCpp(p);
		}
	}
}

void  rpcMgr:: writeMsgCpp(rpcArryInfo* pA, std::ofstream& os, msgInfo& rMsg, bool bRet)
{
	dataVector& vData = rMsg.m_vData; 
	udword udwS = vData.size(); 
	os<<rMsg.name()<<"::"<<rMsg.name()<<"()"<<std::endl
		<<"{"<<std::endl;
		//<<"LOG_FUN_CALL"<<std::endl;
	if(udwS > 0)
	{
		os<<"	m_pPacket = (packetHead*)allocPacket(sizeof("<<rMsg.name()<<"Pack));"<<std::endl;
	}
	else
	{
		os<<"	m_pPacket = (packetHead*)allocPacket(0);"<<std::endl;
	}
	os<<"	netPacketHead* pN = P2NHead(m_pPacket);"<<std::endl
		<<"	pN->dwMsgID = "<<pA->name()<<"2FullMsg("<<rMsg.m_msgIDName<<");"<<std::endl;
		if (bRet) {
			os<<"    NSetRet(pN);"<<std::endl;
		}
	std::string strMsgPackname = rMsg.name();
	strMsgPackname += "Pack";

	if(udwS > 0)
	{
		//dataInfo* pArr = vData.arry();
		dataVector& pArr = vData;
		dataInfo& rD = pArr[udwS -1];
		if(rD.m_length > 1 && rD.m_haveSize)
		{
			os<<"	"<<rMsg.name()<<"Pack* p = (("<<rMsg.name()<<"Pack*)(N2User(pN)));"<<std::endl
				<<"	p->m_"<<rD.m_name<<"Num = 0;"<<std::endl
				<<"	pN->dwLength = sizeof("<<rMsg.name()<<"Pack) - sizeof(p->m_"<<rD.m_name<<");"<<std::endl;
		}
	}

	std::string strBaseClass = "CMsgBase";
	
	os<<"}"<<std::endl
		<<rMsg.name()<<"::"<<rMsg.name()<<"(packetHead* p):"<<strBaseClass<<"(p)"<<std::endl
		<<"{"<<std::endl
		<<"}"<<std::endl;
	writeVDataCpp("", os, rMsg, strMsgPackname.c_str());
	if(udwS > 0)
	{ 
		//dataInfo* pArr = vData.arry(); 
		dataVector& pArr = vData;
		dataInfo& rD = pArr[udwS -1]; 
		if(rMsg.m_bRwPack)
		{
			os<<"packetHead* "<<rMsg.name()<<"::  toPack()"<<std::endl
				<<"{"<<std::endl
				//<<"	if(m_pRWPacket)return m_pRWPacket;"<<std::endl
				<<"	netPacketHead* pN = P2NHead(m_pPacket);"<<std::endl
				<<"	packetHead* pNew  = (packetHead*)allocPacket(sizeof("<<rMsg.name()<<"Pack));"<<std::endl
				<<"	netPacketHead* pNewN = P2NHead(pNew);"<<std::endl
				<<"	*pNewN = *pN;"<<std::endl
				<<"	"<<rMsg.name()<<"Pack* p = (("<<rMsg.name()<<"Pack*)(N2User(pN)));"<<std::endl
				<<"	char* pC = (char*)(N2User(pNewN));"<<std::endl
				<<"	udword udwIndex = 0;"<<std::endl
				<<"	p->write(pC, udwIndex);"<<std::endl
				<<"	pNewN->dwLength = udwIndex;"<<std::endl	
				<<"	releasePack(m_pPacket);"<<std::endl
				<<"	m_pPacket= pNew;"<<std::endl
				<<"	return m_pPacket;"<<std::endl 
				<<"}"<<std::endl;
			os<<"bool "<<rMsg.name()<<":: fromPack(packetHead* p)"<<std::endl
				<<"{"<<std::endl
				<<"	releasePack(m_pPacket);"<<std::endl
				<<"	netPacketHead* pN = P2NHead(p);"<<std::endl
				<<"	packetHead* pNew  = (packetHead*)allocPacket(sizeof("<<rMsg.name()<<"Pack));"<<std::endl
				<<"	netPacketHead* pNewN = P2NHead(pNew);"<<std::endl
				<<"	*pNewN = *pN;"<<std::endl
				<<"	char* pC = (char*)(N2User(pN));"<<std::endl
				<<"	"<<rMsg.name()<<"Pack* pU = (("<<rMsg.name()<<"Pack*)(N2User(pNewN)));"<<std::endl
				<<"	udword udwIndex = 0;"<<std::endl
				<<"	pU->read(pC, udwIndex);"<<std::endl;

			if(rD.m_length > 1 && rD.m_haveSize)
			{	
				os<<"	pNewN->dwLength = sizeof("<<rMsg.name()<<"Pack) - sizeof(p->m_"<<rD.m_name<<"[0]) * ("<<rD.m_length<<" - p->m_"<<rD.m_name<<"Num);"<<std::endl;
			}
			else
			{
				os<<"	pNewN->dwLength = sizeof("<<rMsg.name()<<"Pack);"<<std::endl;
			}
			os<<"	m_pPacket = pNew;"<<std::endl
				<<"	releasePack(p);"<<std::endl
				<<"}"<<std::endl;
		}
		else if(rD.m_length > 1 && rD.m_haveSize)
		{	
			os<<"packetHead* "<<rMsg.name()<<"::  toPack()"<<std::endl
				<<"{"<<std::endl
				<<"	netPacketHead* pN = P2NHead(m_pPacket);"<<std::endl
				<<"	"<<rMsg.name()<<"Pack* p = (("<<rMsg.name()<<"Pack*)(N2User(pN)));"<<std::endl
				<<"	assert(p->m_"<<rD.m_name<<"Num < "<<rD.m_length<<");"
				<<"	pN->dwLength = sizeof("<<rMsg.name()<<"Pack) - sizeof(p->m_"<<rD.m_name<<"[0]) * ("<<rD.m_length<<" - p->m_"<<rD.m_name<<"Num);"<<std::endl
				<<"	return m_pPacket;"<<std::endl 
				<<"}"<<std::endl;
		}
	}
	if(udwS > 0)
	{
		os<<rMsg.name()<<"Pack* "<<rMsg.name()<<"::	pack()"<<std::endl
			<<"{"<<std::endl
			<<"	return (("<<rMsg.name()<<"Pack*)(P2User(m_pPacket)));"<<std::endl
			<<"}"<<std::endl;
	}
	os<<std::endl;
}

static bool s_visitWriteRpcCpp(void* pU, rpcMgr::pRpcInfo& pS)
{
	rpcMgr::pRpcInfo pRpc =  dynamic_cast<rpcMgr::pRpcInfo>(pS);
	assert(pRpc);
	temp::visitData * pD = (temp::visitData*)pU;
	std::ofstream& os = *pD->ps;

	pD->pMgr->writeMsgCpp(pD->pA, os, pRpc->m_ask, false);	
	if(pRpc->m_bRet)
	{
		pD->pMgr->writeMsgCpp(pD->pA, os, pRpc->m_ret, true);
	}
	return true;
}

static bool s_visitWriteRpcRegID(void* pU, rpcMgr::pRpcInfo& pS)
{
	rpcMgr::pRpcInfo pRpc = pS;// dynamic_cast<rpcMgr::pRpcInfo>(pS);
	assert(pRpc);
	temp::visitData * pD = (temp::visitData*)pU;
	std::ofstream& os = *pD->ps;

	if(pRpc->m_bRet)
	{
		os<<"	s_RegRPC("<<pRpc->m_ask.m_msgIDName<<", "<<pRpc->m_ret.m_msgIDName<<", "<<pRpc->m_ask.m_strBodyMaxSize
			<<", "<<pRpc->m_ret.m_strBodyMaxSize<<");"<<std::endl;
	}
	else
	{
		os<<"	s_RegMsg("<<pRpc->m_ask.m_msgIDName<<", "<<pRpc->m_ask.m_strBodyMaxSize<<");"<<std::endl;
	}
	return true;
}

void rpcMgr::writeVDataCpp(const char* szPer, std::ofstream& os, rpcMgr::structInfo& rSt, const char* szClass)
{
	dataVector& vData = rSt.m_vData;

	if(!rSt.m_bRwPack)
	{
		return;
	}
	
	std::stringstream rs;
	std::stringstream ws;
	rs<<"void "<<szClass<<"::read(char* pBuff, udword& udwIndex)"<<std::endl
		<<"{"<<std::endl;
	ws<<"void "<<szClass<<"::write(char* pBuff, udword& udwIndex)"<<std::endl
		<<"{"<<std::endl;
	udword udwS = vData.size();
	//dataInfo* pArr = vData.arry();
	dataVector& pArr = vData;
	for(udword i = 0; i < udwS; i++)
	{
		dataInfo& rD = pArr[i];
		std::string strName = "m_";
		strName += rD.m_name;
		std::string strNumName = strName;
		strNumName += "Num";
		udword udwMaxNum = rD.m_length;
		if(rD.m_zeroEnd)
		{
			udwMaxNum--;
		}
		//cpchar pFind = msgTool::single().findDataType(rD.m_type);
		cpchar pFind = tSingleton<msgTool>::single().findDataType(rD.m_type);
		if(rD.m_length > 1 && rD.m_haveSize) {
			rs<<"	memcpy(&"<<strNumName<<", pBuff + udwIndex, sizeof("<<strNumName<<"));"<<std::endl
				<<"	assert("<<strNumName<<" < "<<rD.m_length<<");"<<std::endl
				<<"	if("<<strNumName<<" >= rD.m_length)"<<std::endl
				<<"		"<<strNumName<<" = "<<udwMaxNum<<";"<<std::endl
				<<"	udwIndex += sizeof("<<strNumName<<");"<<std::endl;


			ws<<"	memcpy(pBuff + udwIndex, &"<<strNumName<<",  sizeof("<<strNumName<<"));"<<std::endl
				<<"	udwIndex += sizeof("<<strNumName<<");"<<std::endl;
		}
		if(rD.m_length > 1)
		{
			if(pFind)
			{
				std::stringstream sLen;
				sLen<<"sizeof("<<strName<<"[0]) * "<<udwMaxNum;
				rs<<"	memcpy("<<strName<<", pBuff + udwIndex,"<<sLen.str()<<");"<<std::endl
					<<"	udwIndex += ("<<sLen.str()<<");"<<std::endl;

				if(rD.m_zeroEnd && rD.m_haveSize)
				{
					rs<<"	"<<strName<<"["<<strNumName<<"] = 0;"<<std::endl;
				}
				ws<<"	memcpy(pBuff + udwIndex, "<<strName<<", "<<sLen.str()<<");"<<std::endl
					<<"	udwIndex += ("<<sLen.str()<<");"<<std::endl;
			}
			else
			{
				std::stringstream sst;
				sst<<rD.m_length;
				std::string strLen = sst.str();
				if(rD.m_haveSize)
				{
					strLen = strNumName;
				}
				rs<<szPer<<"	for(udword i = 0; i < "<<strLen<<"; i++)"<<std::endl
					<<szPer<<"		"<<strName<<"[i].read(pBuff, udwIndex);"<<std::endl;
				ws<<szPer<<"	for(udword i = 0; i < "<<strLen<<"; i++)"<<std::endl
					<<szPer<<"		"<<strName<<"[i].write(pBuff, udwIndex);"<<std::endl;
			}
		}
		else
		{
			if(pFind)
			{
				rs<<"	memcpy(&"<<strName<<", pBuff + udwIndex, sizeof("<<strName<<"));"<<std::endl
					<<"	udwIndex += sizeof("<<strName<<");"<<std::endl;
				ws<<"	memcpy(pBuff + udwIndex, &"<<strName<<",  sizeof("<<strName<<"));"<<std::endl
					<<"	udwIndex += sizeof("<<strName<<");"<<std::endl;
			}
			else
			{
				rs<<szPer<<"	"<<strName<<".read(pBuff, udwIndex);"<<std::endl;
				ws<<szPer<<"	"<<strName<<".write(pBuff, udwIndex);"<<std::endl;
			}
		}
	}
	rs<<szPer<<"}"<<std::endl;
	ws<<szPer<<"}"<<std::endl;
	os<<rs.str()
		<<ws.str();
}

void   rpcMgr::  writeOnceRpcArryCpp(rpcArryInfo* pA)
{
	const int c_BuffSize = 128;
	char szFilename[c_BuffSize]={0};
	std::unique_ptr<char[]> path;
	tSingleton<msgTool>::single().getDefMsgPath(path);
	snprintf(szFilename, c_BuffSize, "%s/%sRpc.cpp", path.get(), pA->name());
	std::ofstream os(szFilename);
	
	os<<"#include<assert.h>"<<std::endl
		<<"#include<stdlib.h>"<<std::endl
		<<"#include \""<<pA->name()<<"Rpc.h\""<<std::endl
		<<"#include \"memmgr.h\""<<std::endl
		<<"#include \"msgPmpID.h\""<<std::endl
		<<"#include \"MsgMgr.h\""<<std::endl
		<<"#include \""<<pA->name()<<"MsgID.h\""<<std::endl
		<<"#include \"funLog.h\""<<std::endl
		<<std::endl
		<<"packetHead* allocPacket(udword udwS);"<<std::endl;
	temp::visitData data;
	data.pMgr = this;
	data.ps = &os;
	data.pA = pA;
	for (auto it = pA->m_pStructInfoSet.begin (); pA->m_pStructInfoSet.end () != it; ++it) {
		auto pSt = it->second.get();
		if (nullptr == pSt) {
			continue;
		}
		if(pSt->m_vData.size()==  0) {
			continue;
		}
		writeVDataCpp("", os, *pSt, pSt->m_name);
	}
	/*
	pA->m_pStructInfoSet.visit(&data,[](void* pU, pIStringKey& pI){
				temp::visitData* pData = (temp::visitData*)pU;
				structInfo* pSt = dynamic_cast<structInfo*>(pI);	
				if(NULL == pSt)
				{
					return true;
				}

				temp::visitData &rdata = *((temp::visitData*)(pU));
				std::ofstream &os = *rdata.ps;
				if(pSt->m_vData.size()==  0)
				{
					return true;
				}
				pData->pMgr->writeVDataCpp("", os, *pSt, pSt->m_name);
				return true;
			});
			*/
	os<<std::endl;
	for (auto it = pA->m_pRpcInfoSet.begin (); pA->m_pRpcInfoSet.end () != it; ++it) {
		auto p = it->get();
		s_visitWriteRpcCpp (&data, p);
	}
	//pA->m_pRpcInfoSet.visit(&data,s_visitWriteRpcCpp);

	os<<"static void s_RegRPC(uword uwAsk, uword uwRet, udword askMaxSize, udword retMaxSize)"<<std::endl
		<<"{"<<std::endl
		//<<"LOG_FUN_CALL"<<std::endl
		<<"	assert(uwAsk < 256 && uwRet < 256);"<<std::endl
		<<"	CMsgMgr& msgMgr =  CMsgMgr::single();"<<std::endl
		<<"	uwAsk = "<<pA->name()<<"2FullMsg(uwAsk);"<<std::endl
		<<"	uwRet = "<<pA->name()<<"2FullMsg(uwRet);"<<std::endl
		<<"	msgMgr.regRpc((uwAsk), (uwRet), askMaxSize, retMaxSize);"<<std::endl
		<<"}"<<std::endl
		
		<<"static void s_RegMsg(uword uwAsk, udword askMaxSize)"<<std::endl
		<<"{"<<std::endl
		//<<"LOG_FUN_CALL"<<std::endl
		<<"	assert(uwAsk < 256);"<<std::endl
		<<"	CMsgMgr& msgMgr =  CMsgMgr::single();"<<std::endl
		<<"	msgMgr.regMsg("<<pA->name()<<"2FullMsg(uwAsk), askMaxSize);"<<std::endl
		<<"}"<<std::endl

		<<"int "<<pA->name()<<"MsgInfoReg()"<<std::endl
		<<"{"<<std::endl;
		//<<"LOG_FUN_CALL"<<std::endl;

		for (auto it = pA->m_pRpcInfoSet.begin (); pA->m_pRpcInfoSet.end () != it; ++it) {
			auto p = it->get();
			s_visitWriteRpcRegID (&data, p);
		}
		//pA->m_pRpcInfoSet.visit(&data, s_visitWriteRpcRegID);
		os<<"	return 0;"<<std::endl;
	os<<"}"<<std::endl;
}
