#include "msgTool.h"
#include"rpcMgr.h"
#include<string>
#include<sstream>

namespace temp
{
	struct visitDataTS
	{
		rpcMgr* pMgr;
		std::stringstream *ps;
		rpcMgr::rpcArryInfo* pA;
	};
}

void	rpcMgr::writeRpcArry2Ts()
{
	const int c_BuffSize = 64;
	char szFilename[c_BuffSize]={0};
	snprintf(szFilename, c_BuffSize, "%s/RpcOff.cpp", m_tsLibDir); 
	std::ofstream os(szFilename);
	std::stringstream ss;	
	std::stringstream sIn;
	os<<"#include\"rpcMgr.h\""<<std::endl
		<<"#include\"tSingleton.h\""<<std::endl;

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
		if(!p)
		{
			continue;
		}
		if(!p->m_b2js)
		{
			continue;
		}
		sIn<<"#include\""<<p->name()<<"Rpc.h\""<<std::endl;
		writeOnceRpcArry2TS(ss, p);
	}
	os<<sIn.str()<<std::endl
		<<"void writeMsgOff()"<<std::endl
		<<"{"<<std::endl
		<<"	rpcMgr::structInfo* pSt = NULL;"<<std::endl

		<<"	uqword uqwOff = 0;"<<std::endl
			<<"	uqword uqwNumOff = 0;"<<std::endl
			<<std::endl
		<<ss.str()<<std::endl
		<<"}"<<std::endl;	
}

void  rpcMgr::writeVDataTS(const char* szPer, std::stringstream& os,  dataVector& vData, const char* szStName)
{
	os<<"	pSt = rpcMgr::single().findSt(\""<<szStName<<"\");"<<std::endl;
	udword udwS = vData.size();
	auto& pArr = vData; // dataInfo* pArr = vData.arry();
	for(udword i = 0; i < udwS; i++)
	{
		dataInfo& rD = pArr[i];
		if(rD.m_length > 1 && rD.m_haveSize)
		{
			std::string strName = rD.m_name;
			strName += "Num";
			os<<szPer<<"	uqwNumOff = "<<"(uqword)&(("<<szStName<<"*)0)->m_"<<strName<<";"<<std::endl;
		}
		os<<szPer<<"	uqwOff = "<<"(uqword)&(("<<szStName<<"*)0)->m_"<<rD.m_name<<";"<<std::endl
			<<szPer<<"	pSt->setOff(\""<<rD.m_name<<"\", uqwOff, uqwNumOff, sizeof((("<<szStName<<"*)0)->m_"<<rD.m_name<<"));"<<std::endl;
	}
}

void  rpcMgr:: writeOnceRpcArry2TS(std::stringstream &os, rpcArryInfo* pA)
{
	/*
		temp::visitDataTS data;
	data.pMgr = this;
	data.ps = &os;
	data.pA = pA;
	pA->m_pStructInfoSet.visit(&data,[](void* pU, pIStringKey& pI){
				structInfo* pSt = dynamic_cast<structInfo*>(pI);	
				if(NULL == pSt)
				{
					return true;
				}

				temp::visitDataTS& rdata = *((temp::visitDataTS*)(pU));
				std::stringstream&os = *rdata.ps;
				if(pSt->m_vData.size()==  0)
				{
					return true;
				}

				rdata.pMgr->writeVDataTS("", os,  pSt->m_vData, pSt->name());
				os<<std::endl;
				return true;
			});
			*/
	for (auto it = pA->m_pStructInfoSet.begin (); pA->m_pStructInfoSet.end () != it; ++it) {
		auto pSt = it->second.get();
		writeVDataTS("", os,  pSt->m_vData, pSt->name());
		os<<std::endl;
	}
	/*
	temp::visitDataTS data2;
	data2.pMgr = this;
	data2.ps = &os;
	data2.pA = pA;
	pA->m_pRpcInfoSet.visit(&data2, [](void* pU, pRpcInfo& pI){
			rpcMgr::pRpcInfo pRpc =  dynamic_cast<rpcMgr::pRpcInfo>(pI);
			assert(pRpc);
			temp::visitDataTS* pD = (temp::visitDataTS*)pU;
			std::stringstream& os = *pD->ps;

			pD->pMgr->writeVDataTS("", os,  pRpc->m_ask.m_vData, pRpc->m_ask.m_packName.c_str());	

			os<<std::endl;
			if(pRpc->m_bRet)
			{
				pD->pMgr->writeVDataTS("", os,  pRpc->m_ret.m_vData, pRpc->m_ret.m_packName.c_str());	
				os<<std::endl;
			}
			return true;
		});
		*/
	for (auto it = pA->m_pRpcInfoSet.begin (); pA->m_pRpcInfoSet.end () != it; ++it) {
		auto pRpc = it->get();
		writeVDataTS("", os,  pRpc->m_ask.m_vData, pRpc->m_ask.m_packName.c_str());
		os<<std::endl;
		if(pRpc->m_bRet) {
			writeVDataTS("", os,  pRpc->m_ret.m_vData, pRpc->m_ret.m_packName.c_str());
			os<<std::endl;
		}
	}
}

