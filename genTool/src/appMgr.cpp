#include <stdlib.h>
#include <fstream>
#include <sstream>
//#include <unistd.h>
//#include <dirent.h>
#include"appMgr.h"
#include"strFun.h"
#include "makeFileCreator.h"
#include "appFileCreator.h"
#include <memory>
#include "rLog.h"
#include "comFun.h"
#include "CMakeListsCreator.h"
#include "defFileCrector.h"
#include "exportFunCrector.h"

app::app()
{
	strNCpy(m_projectDir, DirSize, "../");
	m_projectDir[0] = 0;
	m_appType = appType::eApp_cpp;
}

app::~app()
{
	otherPmpMap &rM = m_otherPmpMap;
	for(otherPmpMap::iterator iter = rM.begin(); rM.end() != iter; ++iter)
	{
		delete iter->second.pList;
	}
}

appType  app::getAppType()
{
	return m_appType;
}

void	 app::setAppType(appType at)
{
	m_appType = at;
}

bool app::operator < (const app& other)const
{
	return strcmp(m_name, other.name()) < 0;
}

bool app::procRpcInfo::operator < (const procRpcInfo& other)const
{
	/*
	int n = strcmp(m_rpcArryName, other.m_rpcArryName);
	if(n < 0)
	{
		return true;
	}
	if(n > 0)
	{
		return false;
	}
	*/
	int n = strcmp(m_rpcName, other.m_rpcName);
	if(0 == n)
	{
		return m_bAsk;
	}
	else
	{
		return n < 0;
	}
}

const char* app::projectDir()
{
	return m_projectDir;
}
bool app::addProcRpcInfo(const char* szName, bool bAsk, bool bExit, const char* szPmpName, const char* szClassName, 
		const char* szNextPmp, const char* conName, int sendType, bool bPass, bool bNeetPlayer)
{
	procRpcInfoList* pList = &m_procRpcInfoList;
	if(szPmpName)
	{
		otherPmpMap &rM = m_otherPmpMap;
		otherPmpMap::iterator iter = rM.find(szPmpName);
		if(rM.end() == iter)
		{
			pList = new  procRpcInfoList;
			//std::pair<std::string, procRpcInfoList*> p;
			stPmpInfo p;
			if(szClassName)
			{
				p.strClass = szClassName;
			}
			if(szNextPmp)
			{
				p.strNext = szNextPmp;
			}
			p.pList = pList;
			rM[szPmpName] = p;
		}
		else
		{
			//pList = iter->second.second;
			pList = iter->second.pList;
		}
	}
	procRpcInfo info;
	strNCpy(info.m_rpcName, NameSize, szName);
	info.m_bAsk = bAsk;
	info.m_bExit = bExit;
	info.m_bPass = bPass;
	info.m_bNeetPlayer = bNeetPlayer;
	if(conName)
	{
		strNCpy(info.conName, NameSize, conName);
	}
	else
	{
		info.conName[0] = 0;
	}
	info.sendType = (ubyte)sendType;
    bool bRet = pList->insert(info);
	return bRet;
}

app::procRpcInfoList& app::procRpcList()
{
	return m_procRpcInfoList;
}

appMgr::appMgr()
{
}

appMgr::~appMgr()
{
	/*
	m_appList.visit(NULL, [](void* pU, pApp& p){
			p->~app();
			SD(p);
			return true;
			});
			*/
}

bool appMgr::insert(std::shared_ptr<app> pA)
//bool appMgr::insert(app* pA)
{
	m_appList.push_back(pA);
	return 	true;
}


int  appMgr::procApp()
{
	int nRet = 0;
	auto& rList = m_appList;
	for (auto it = rList.begin (); rList.end () != it; ++it) {
		auto p = it->get();
		std::string strBase = p->projectDir();
		strBase += p->name();
		std::string strProject = strBase;
		strProject += "/";
		strBase += "/src/";
		std::string strSrcDir = strBase;
		std::string strGen = strBase + "gen/";
		//rTrace(__FUNCTION__<<" 000");
		if (!isPathExit (strGen.c_str())) {
			std::string sys = "mkdir -p ";
			sys += strGen;
			rDebug (__FUNCTION__<<" will create Path = "<<strGen.c_str());
			auto nR = myMkdir (strGen.c_str()); //system(sys.c_str());
			if (0 != nR) {
				auto nErr = errno;
				rError (__FUNCTION__<<" create path error path = "<<strGen.c_str()<<"nErr = "<<nErr);
			}
		}
		//rTrace(__FUNCTION__<<" 111");
		std::string strProcMsg = strBase + "procMsg/";
		if (!isPathExit (strProcMsg.c_str()) ){
			std::string sys = "mkdir -p ";
			sys += strProcMsg;
			rDebug (__FUNCTION__<<" will create Path = "<<strProcMsg.c_str());
			auto nR = myMkdir (strProcMsg.c_str()); //system(sys.c_str());
			if (0 != nR) {
				auto nErr = errno;
				rError (__FUNCTION__<<" create path error path = "<<strGen.c_str()<<"nErr = "<<nErr);
			}
		}
		//rTrace(__FUNCTION__<<" 222");
		/*
		std::string strMakeFile = strProject + "Makefile";
		if (!isPathExit (strMakeFile.c_str())) {
			makeFileCreator file(strMakeFile.c_str(), p);
		}
		*/
		std::string strCMakeFile = strProject + "CMakeLists.txt";
		if (!isPathExit (strCMakeFile.c_str())) {
			CMakeListsCreator file(strCMakeFile.c_str(), p);
		}
		//rTrace(__FUNCTION__<<" 333");
		appFileCreator appF(strSrcDir.c_str(), p->name());	
		appF.writeH();
		//rTrace(__FUNCTION__<<" 444");
		appF.writeCpp();

		std::string exportFunFileH = strBase;
		exportFunFileH += "exportFun.h";
		writeExportFunH (exportFunFileH.c_str ());
		std::string exportFunFileCpp = strBase;
		exportFunFileCpp += "exportFun.cpp";
		writeExportFunCpp (exportFunFileCpp.c_str ());

		std::string defFunFile = strBase;
		defFunFile += "defFun.def";
		writeDefFile (defFunFile.c_str ());
		rTrace(__FUNCTION__<<" 555");
		int  nR = procOnceApp(p);
		assert(0 == nR);
	}
/*
	m_appList.visit(this, [](void* pU, pApp& p){
			appMgr* pThis = (appMgr*)pU;
				std::string strBase = p->projectDir();
				strBase += p->name();
				std::string strProject = strBase;
				strProject += "/";
				strBase += "/src/";
				std::string strSrcDir = strBase;
				std::string strGen = strBase + "gen/";
				//if (access(strGen.c_str(),R_OK) !=0) {
				if (!isPathExit (strGen.c_str()) {
					std::string sys = "mkdir -p ";
					sys += strGen;
					system(sys.c_str());
				}
				std::string strProcMsg = strBase + "procMsg/";
				if (!isPathExit (strProcMsg.c_str()) ){
				//if (access(strProcMsg.c_str(),R_OK) !=0) {
					std::string sys = "mkdir -p ";
					sys += strProcMsg;
					system(sys.c_str());
				}
				std::string strMakeFile = strProject + "Makefile";
				if (!isPathExit (strMakeFile.c_str())) {
				//if (access(strMakeFile.c_str(),R_OK) !=0) {
					makeFileCreator file(strMakeFile.c_str(), p);
				}
				appFileCreator appF(strSrcDir.c_str(), p->name());	
				appF.writeH();
				appF.writeCpp();
				int  nR = pThis->procOnceApp(p);
				assert(0 == nR);
				return true;
			});
			*/
	return 0;
}

class stProcArrayInfo
{
public:
	stProcArrayInfo(app* pApp, const char* szBase, const char* szArryName,
			const char* szPmpName, const char* szPmpClass)
	{
		m_bHaveNeetPlayer = false;
		m_pApp = pApp;
		m_pmpName = szPmpName;
		m_pmpClass = szPmpName;
		//IMemMgr* pI = GetMemMgr();
		m_strBase = szBase;
		std::string strGen = szBase;
		strGen += "gen/";
		strGen +=  szPmpName;
		strGen +=  szArryName;
		strGen += "Gen.cpp";
		//m_of.open(strGen.c_str());
		//assert(m_of.is_open());
		m_arryName = szArryName;

		rpcMgr::rpcArryInfo* pArr =  tSingleton<rpcMgr>::single().getRpcArry(szArryName);
		assert(pArr);

		m_of<<"#include\"msgPmpID.h\""<<std::endl
			<<"#include\""<<szArryName<<"MsgID.h\""<<std::endl
			<<"#include\""<<szArryName<<"Rpc.h\""<<std::endl
			<<"#include\"arrMsgPmp.h\""<<std::endl
			<<"#include\"msgPmpMgr.h\""<<std::endl
			<<"#include\"rlog.h\""<<std::endl
			<<"#include<assert.h>"<<std::endl
			<<"#include\"funLog.h\""<<std::endl
			<<std::endl;

		std::string strPro = szBase;
		strPro += "procMsg/";
		strPro +=  szPmpName;
		strPro +=  szArryName;
		strPro += "Proc.cpp";
		std::ifstream ps(strPro.c_str());
		if(ps)
		{
			ps.seekg(0, std::ios::end);    // go to the end  
			udword length = ps.tellg();           // report location (this is the length)  
			ps.seekg(0, std::ios::beg); 
			auto pBuff = std::make_unique<char[]>(length + 1);
			char* szText = pBuff.get();// (char*)(pI->Malloc(length + 1));
			ps.read(szText, length);
			szText[length] = 0;
			m_proc<<szText;
			//SD(szText);
		}
		else
		{
			m_proc<<"#include\"msgPmpID.h\""<<std::endl
			<<"#include\""<<szArryName<<"MsgID.h\""<<std::endl
			<<"#include\"arrMsgPmp.h\""<<std::endl
			<<"#include\"msgPmpMgr.h\""<<std::endl
			<<"#include\""<<szArryName<<"Rpc.h\""<<std::endl
			<<"#include\"rlog.h\""<<std::endl
			<<"#include<assert.h>"<<std::endl
			<<"#include\"funLog.h\""<<std::endl
			<<std::endl;
		}

		if(0 == strlen(szPmpName))
		{
			m_arryRegFun<<"int reg"<<szArryName<<"MsgPmp()"<<std::endl;
		}
		else
		{
			m_arryRegFun<<"int reg"<<szPmpName<<"_"<<szArryName<<"MsgPmp()"<<std::endl;
		}
		m_arryRegFun<<"{"<<std::endl
			<<"LOG_FUN_CALL"<<std::endl;
		if(0 == strlen(szPmpName))
		{
			if(!m_pApp->m_procRpcInfoList.empty())
			{
				m_arryRegFun<<"	MsgPmp* pPmg =  &(arrMsgPmp::single());"<<std::endl;
			}
		}	
		else
		{
			app::otherPmpMap &rMap = m_pApp->m_otherPmpMap;
			for(app::otherPmpMap::iterator iter = rMap.begin(); rMap.end() != iter; ++iter)
			{
				app::procRpcInfoList* pL = iter->second.pList;
				if(pL->empty())
				{
					continue;
				}
				if(iter->first != szPmpName)
				{
					continue;
				}
				m_arryRegFun<<"	MsgPmp* p"<<iter->first<<" =  msgPmpMgr::single().getPmp(\""<<iter->first<<"\");"<<std::endl;
				//m_arryRegFun<<"	MsgPmp* pPmg =  msgPmpMgr::single().getPmp(\""<<iter->first<<"\");"<<std::endl;
			}
		}
		
	}
	~stProcArrayInfo()
	{
		if(m_proc.str().size())
		{
			if(m_pmpName.length() && m_pmpClass == "onceProcMsgPmp")
			{
				std::stringstream sFun;
				sFun<<m_pmpName<<"_onceProcMsgPmp";
				if(-1 == m_proc.str().find(sFun.str()))
				{
					m_proc<<std::endl<<"procMsgRetType	"<<sFun.str()<<"(packetHead* pPack, pPacketHead& pRet)"<<std::endl
						<<"{"<<std::endl
						<<"LOG_FUN_CALL"<<std::endl
						<<std::endl
						<<"	return eProcPass;"<<std::endl
						<<"}"<<std::endl;
				}
			}
			std::string strPro = m_strBase;
			strPro += "procMsg/";
			strPro +=	m_pmpName;
			strPro +=  m_arryName;
			strPro += "Proc.cpp";

			if(m_pmpClass != "onceProcMsgPmp")
			{
			std::ofstream os(strPro.c_str());
			assert(os.is_open());

			os<<m_proc.str();	
			}
		}
		if(m_of.str().size())
		{
			std::string strGen = m_strBase;
			strGen += "gen/";
			strGen +=	m_pmpName;
			strGen +=  m_arryName;
			strGen += "Gen.cpp";

			std::ofstream gs(strGen.c_str());
			assert(gs.is_open());
			if(m_bHaveNeetPlayer)
			{
				gs<<"#include\"../playerMgr.h\""<<std::endl;
			}
			gs<<m_of.str();	
		}
	}
	std::string m_arryName;
	std::string m_pmpName;
	std::string m_pmpClass;
//	std::stringstream *m_pGen;
	std::stringstream m_proc;
//	std::ofstream m_of;
	std::stringstream m_of;
	std::string  m_strBase;
	//std::stringstream  m_regOs;
	std::stringstream  m_arryRegFun;
	app* m_pApp;
	bool m_bHaveNeetPlayer;
	//std::ofstream m_pf;
};

typedef stProcArrayInfo* pStProcArrayInfo;
/*
static stProcArrayInfo* new_stProcArrayInfo(app* pApp, const char* szBase, const char* szArryName, const char* szPmpName , const char* szPmpClass)
{
	IMemMgr* pI = GetMemMgr();
	stProcArrayInfo* p = (stProcArrayInfo*)(pI->Malloc(sizeof(stProcArrayInfo)));
	p = new (p) stProcArrayInfo(pApp, szBase, szArryName, szPmpName);
	p->m_pmpClass =  szPmpClass;
	return p;
}

static void del_stProcArrayInfo(stProcArrayInfo* p)
{
	if(p)
	{
		if(p->m_arryRegFun.str().length() > 0)
		{
			p->m_arryRegFun<<"		return 0;"<<std::endl
				<<"}";
			p->m_of<<std::endl<<p->m_arryRegFun.str();
		}

		p->~stProcArrayInfo();
		SD(p);
	}
}
*/
struct visitThis
{
	std::string strBase;
	appMgr* pU;
	app*    pApp;
	bool    bAllInOnceProcFun;
	bool    bDefPmp;
	//pStProcArrayInfo pInfo; 
	std::unique_ptr <stProcArrayInfo> pInfo;
	std::stringstream* pRegOF;
	std::stringstream ssRegInc;
	std::stringstream ssRegDel;
	std::string		  strPmpName;
	std::string		  m_pmpClass;
};

static  bool s_visitProcRpc(void* pU, app::procRpcInfo& node)
{
	visitThis* pThis = (visitThis*)pU;
	char szBuff[NameSize]={0};
	rTrace (__FUNCTION__<<" rpcName = "<<node.m_rpcName);
	strNCpy(szBuff, NameSize, node.m_rpcName);
	char* pRet[2];
	int nR = strR(szBuff, ':', pRet, 2);
	assert(2 == nR);
	if(pThis->pInfo)
	{
		if(pThis->pInfo->m_arryName != pRet[0])
		{
			if(pThis->bDefPmp)
			{
				pThis->ssRegDel<<"int reg"<<pThis->pInfo->m_arryName<<"MsgPmp();"<<std::endl;
				*pThis->pRegOF<<"	nR = reg"<<pThis->pInfo->m_arryName<<"MsgPmp();"<<std::endl;
			}
			else
			{
				pThis->ssRegDel<<"int reg"<<pThis->strPmpName<<"_"<<pThis->pInfo->m_arryName<<"MsgPmp();"<<std::endl;
				*pThis->pRegOF<<"	nR = reg"<<pThis->strPmpName<<"_"<<pThis->pInfo->m_arryName<<"MsgPmp();"<<std::endl;
			}
			*pThis->pRegOF<<"	assert(0 == nR);"<<std::endl;
			pThis->pInfo.reset ();
			//del_stProcArrayInfo(pThis->pInfo);
			//pThis->pInfo = NULL;
		}
	}
	if(!pThis->pInfo)
	{
		const char* szPmpName = "";
		const char* szClassName = "";
		if(!pThis->bDefPmp)
		{
			szPmpName = pThis->strPmpName.c_str();
			szClassName = pThis->m_pmpClass.c_str();
		}
		//pThis->pInfo = new_stProcArrayInfo(pThis->pApp, pThis->strBase.c_str(), pRet[0], szPmpName, szClassName);
		pThis->pInfo = std::make_unique<stProcArrayInfo>(pThis->pApp, pThis->strBase.c_str(), pRet[0], szPmpName, szClassName);
		pThis->ssRegInc<<"#include\""<<pThis->pInfo->m_arryName<<"MsgID.h\""<<std::endl;
	}
	rpcMgr::rpcInfo* pRpc = tSingleton<rpcMgr>::single().find(pRet[0], pRet[1]);
	assert(pRpc);
	if(!pRpc)
	{
		//del_stProcArrayInfo(pThis->pInfo);
		//pThis->pInfo = NULL;
		exit(1);
	}

	std::stringstream sFun;
	if(!pThis->bAllInOnceProcFun)
	{
		pThis->pInfo->m_of<<"static procMsgRetType on_";
		sFun<<"void proc_";
		if(node.m_bAsk)
		{
			sFun<<pRpc->m_ask.m_name;
			pThis->pInfo->m_of<<pRpc->m_ask.m_name;
		}
		else
		{
			sFun<<pRpc->m_ret.m_name;
			pThis->pInfo->m_of<<pRpc->m_ret.m_name;
		}
		sFun<<"(";
		if(node.m_bNeetPlayer)
		{
			sFun<<"player* pPl, ";
		}
		pThis->pInfo->m_of<<"(packetHead* pAsk, pPacketHead& pRet)"<<std::endl
			<<"{"<<std::endl
			<<"LOG_FUN_CALL"<<std::endl;
	}
	if(node.m_bAsk)
	{
		if(!pThis->bAllInOnceProcFun)
		{
			pThis->pInfo->m_of<<"	"<<pRpc->m_ask.m_name<<"	ask(NULL); ask.fromPack(pAsk);"<<std::endl;
			
			if(pRpc->m_ask.m_vData.size() > 0)
			{
				sFun<<"const "<<pRpc->m_ask.m_packName<<"& rAsk";
			}
			if(pRpc->m_bRet)
			{
				if(pRpc->m_ret.m_vData.size() > 0)
				{
					if(pRpc->m_ask.m_vData.size() > 0)
					{
						sFun<<", ";
					}
					sFun<<pRpc->m_ret.m_packName<<"& rRet";
				}
				pThis->pInfo->m_of<<"	"<<pRpc->m_ret.m_name<<"	ret;"<<std::endl;
			}
		}
	}
	else
	{
		if(!pThis->bAllInOnceProcFun)
		{
			pThis->pInfo->m_of<<"	"<<pRpc->m_ask.m_name<<" ask(NULL); ask.fromPack(pAsk);"<<std::endl;
			if(pRpc->m_ask.m_vData.size() > 0)
			{
				sFun<<"const "<<pRpc->m_ask.m_packName<<"& rAsk";
			}
			assert(pRpc->m_bRet);

			pThis->pInfo->m_of<<"	"<<pRpc->m_ret.m_name<<" ret(NULL); ret.fromPack(pRet);"<<std::endl;
			if(pRpc->m_ret.m_vData.size() > 0)
			{

				if(pRpc->m_ask.m_vData.size() > 0)
				{
					sFun<<", ";
				}
				sFun<<"const "<<pRpc->m_ret.m_packName<<"& rRet";
			}
		}
	}

	if(!pThis->bAllInOnceProcFun)
	{
		sFun<<")";
		//node.m_funName = sFun.str();
		std::stringstream& rProcS = pThis->pInfo->m_proc;
		strNCpy(node.m_FunName, DirSize, sFun.str().c_str());	
		int nF = rProcS.str().find(node.m_FunName);
		if(-1 == nF) {
			rProcS<<std::endl<<node.m_FunName<<std::endl
				<<"{"<<std::endl
				<<"}"<<std::endl;
		}
		if(node.m_bNeetPlayer) {
			if(!pThis->pInfo->m_bHaveNeetPlayer) {
				pThis->pInfo->m_bHaveNeetPlayer = true;
			}
			pThis->pInfo->m_of<<"	netPacketHead* pANH = P2NHead(pAsk);"<<std::endl
				<<"	player* pPl = tSingleton<playerMgr>::single().getPlayer(pANH->dwAccID);"<<std::endl
				<<"	assert(pPl);"<<std::endl;
		}
		/* 以下处理自动生成函数的后半部分 */
		auto pIFO = pThis->pInfo.get();
		auto& tios = pIFO->m_of;
		//std::stringstream& tios = pThis->pInfo->m_of;
		char* pFunName = node.m_FunName;
		tios<<"	"
			<< pFunName
			<<";"<<std::endl
			<<"	proc_";
	}
	if(node.m_bAsk)
	{
		if(pThis->bAllInOnceProcFun) {
			pThis->pInfo->m_arryRegFun<<"	p"<<pThis->strPmpName<<"->regMsgRelay("<<pThis->pInfo->m_arryName<<"2FullMsg("<<pRpc->m_ask.m_msgIDName
				<<"), NULL, \""<<node.conName<<"\", "<<(int)(node.sendType)<<");"<<std::endl;
		}
		else {
			pThis->pInfo->m_of<<pRpc->m_ask.m_name<<"(";
			if(node.m_bNeetPlayer) {
				pThis->pInfo->m_of<<"pPl, ";
			}
			if(pRpc->m_ask.m_vData.size() > 0) {
				pThis->pInfo->m_of<<"*ask.pack()";
			}
			if(pRpc->m_bRet) {
				if(pRpc->m_ret.m_vData.size() > 0) {
					if(pRpc->m_ask.m_vData.size() > 0) {
						pThis->pInfo->m_of<<", ";
					}
					pThis->pInfo->m_of<<"*ret.pack()";
				}
			}
			if(pThis->strPmpName.empty()) {
				pThis->pInfo->m_arryRegFun<<"	pPmg->regMsg("<<pThis->pInfo->m_arryName<<"2FullMsg("<<pRpc->m_ask.m_msgIDName<<"), on_"<<pRpc->m_ask.m_name<<");"<<std::endl;
			}
			else {
				pThis->pInfo->m_arryRegFun<<"	p"<<pThis->strPmpName<<"->regMsg("<<pThis->pInfo->m_arryName<<"2FullMsg("<<pRpc->m_ask.m_msgIDName<<"), on_"<<pRpc->m_ask.m_name<<");"<<std::endl;
;
			}
		}
	}
	else
	{
		if(pThis->bAllInOnceProcFun)
		{
			pThis->pInfo->m_arryRegFun<<"	p"<<pThis->strPmpName<<"->regMsgRelay("<<pThis->pInfo->m_arryName<<"2FullMsg("<<pRpc->m_ret.m_msgIDName
				<<"), NULL, \""<<node.conName<<"\", "<<(int)(node.sendType)<<");"<<std::endl;
		}
		else
		{
			pThis->pInfo->m_of<<pRpc->m_ret.m_name<<"(";
			if(pRpc->m_ask.m_vData.size() > 0)
			{
				pThis->pInfo->m_of<<"*ask.pack()";
			}
			assert(pRpc->m_bRet);
			if(pRpc->m_ret.m_vData.size() > 0)
			{
				if(pRpc->m_ask.m_vData.size() > 0)
				{
					pThis->pInfo->m_of<<", ";
				}
				pThis->pInfo->m_of<<"*ret.pack()";
			}
			if(pThis->strPmpName.empty())
			{
				pThis->pInfo->m_arryRegFun<<"	pPmg->regMsg("
					<<pThis->pInfo->m_arryName<<"2FullMsg("<<pRpc->m_ret.m_msgIDName<<"), on_"<<pRpc->m_ret.m_name<<");"<<std::endl;
			}
			else
			{

				pThis->pInfo->m_arryRegFun<<"	p"<<pThis->strPmpName<<"->regMsg("
					<<pThis->pInfo->m_arryName<<"2FullMsg("<<pRpc->m_ret.m_msgIDName<<"), on_"<<pRpc->m_ret.m_name<<");"<<std::endl;
			}
		}
	}

	if(!pThis->bAllInOnceProcFun)
	{
		pThis->pInfo->m_of<<");"<<std::endl;
		std::string strRetT = "eProcPass"; 
		if(node.m_bAsk)
		{
			if(pRpc->m_bRet)
			{
				pThis->pInfo->m_of<<"	"<<"pRet = ret.pop();"<<std::endl;
				strRetT = "eSendRet";
			}
		}
		if(node.m_bExit)
		{
			strRetT = "eExitLoop";
		}
		pThis->pInfo->m_of<<"	return "<<strRetT<<";"<<std::endl
			<<"}"<<std::endl;
	}
	return true;
}

int  appMgr::procOnceApp(app* pApp)
{
	std::string strBase = pApp->projectDir();
	strBase += pApp->name();
	strBase += "/src/";
	
	visitThis th;
	th.pU = this;
	th.pInfo = NULL;
	th.strBase = strBase;
	th.pApp = pApp;
	th.bAllInOnceProcFun = false;
	th.bDefPmp = true;
	std::string strReg = strBase;
	strReg += "gen/";
	myMkdir (strReg.c_str ());
	strReg += "regFun.cpp";
	rTrace (__FUNCTION__<<" strReg = "<<strReg.c_str());
	std::ofstream regRealOF(strReg.c_str());
	assert(regRealOF.is_open());
	
	std::stringstream regOF;
			regOF<<"int regMsgPmp()"<<std::endl
			<<"{"<<std::endl
			<<"	LOG_FUN_CALL"<<std::endl
			<<"		udword nR = 0;"<<std::endl;

	th.pRegOF = &regOF; 
	app::procRpcInfoList& rList = pApp->procRpcList();
	rList.visit(&th, s_visitProcRpc);
	if(th.pInfo)
	{
		th.ssRegDel<<"int reg"<<th.pInfo->m_arryName<<"MsgPmp();"<<std::endl;
						regOF<<"	nR = reg"<<th.pInfo->m_arryName<<"MsgPmp();"<<std::endl
						<<"	assert(0 == nR);"<<std::endl;

		//del_stProcArrayInfo(th.pInfo);
		//th.pInfo = NULL;
	}

	th.bAllInOnceProcFun = true;
	th.bDefPmp = false;

	app::otherPmpMap &rMap = pApp->m_otherPmpMap;
	for(app::otherPmpMap::iterator iter = rMap.begin(); rMap.end() != iter; ++iter)
	{
		app::procRpcInfoList* pL = iter->second.pList;//second;
		if(pL->empty())
		{
			continue;
		}
		th.strPmpName = iter->first;	
		th.m_pmpClass = iter->second.strClass;
		th.bAllInOnceProcFun = (iter->second.strClass == "onceProcMsgPmp");
		pL->visit(&th, s_visitProcRpc);
		if(th.pInfo)
		{
			if(iter->second.strClass == "onceProcMsgPmp")
			{
				th.ssRegDel<<"procMsgRetType	"<<iter->first<<"_onceProcMsgPmp(packetHead* pPack, pPacketHead& pRet);"<<std::endl;
				regOF<<"	onceProcMsgPmp* p"<<iter->first<<" =	dynamic_cast<onceProcMsgPmp*>(msgPmpMgr::single().getPmp(\""
					<<iter->first<<"\"));"<<std::endl;
				if(iter->second.strNext.empty())
				{
					regOF<<"	p"<<iter->first<<"->setProcNullFunPmp(&(arrMsgPmp::single()));"<<std::endl;
				}
				else
				{

					regOF<<"	p"<<iter->first<<"->setProcNullFunPmp(msgPmpMgr::single().getPmp(\""<<iter->second.strNext<<"\"));"<<std::endl;
				}
			}
			
			th.ssRegDel<<"int reg"<<iter->first<<"_"<<th.pInfo->m_arryName<<"MsgPmp();"<<std::endl;
			regOF<<"	nR = reg"<<iter->first<<"_"<<th.pInfo->m_arryName<<"MsgPmp();"<<std::endl
				<<"	assert(0 == nR);"<<std::endl;

			//del_stProcArrayInfo(th.pInfo);
			//th.pInfo = NULL;
		}
	}
	regOF<<"	return 0;"<<std::endl
		<<"}";

	regRealOF<<"#include\"msgPmpID.h\""<<std::endl
			<<"#include\"arrMsgPmp.h\""<<std::endl
			<<"#include\"msgPmpMgr.h\""<<std::endl
			<<"#include\"onceProcMsgPmp.h\""<<std::endl
			<<"#include\"funLog.h\""<<std::endl
			<<"#include<assert.h>"<<std::endl
			<<std::endl
		<<th.ssRegInc.str()<<std::endl
		<<th.ssRegDel.str()<<std::endl
		<<regOF.str();

	return 0;
}
