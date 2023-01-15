#ifndef _rpcMgr_h_
#define _rpcMgr_h_
#include <fstream>
#include <string.h>
#include <string>
#include "tSingleton.h"
#include "arrayMap.h"
//#include "tVector.h"
#include <vector>
#include "comFun.h"
#include "tJumpListNS.h"
#include "constVar.h"
#include <memory>
#include <map>

class rpcMgr
{
public:
	static const int c_nameSize = NameSize;
	static const int c_dirSize = DirSize;
	enum class packetType:char 
	{
		ePack
	};
	
	struct nameBase:public iStringKey
	{
		nameBase();
		virtual const char* name()const;
		virtual ~nameBase(){}
		char m_name[c_nameSize];
	};
	struct dataInfo
	{
		int  m_length;
		char m_name[c_nameSize];
		char m_type[c_nameSize];
		bool m_wordSize;
		bool m_haveSize;
		bool m_zeroEnd;
		char m_strCommit[c_nameSize];
	};
	//typedef tVector<dataInfo>  dataVector;
	typedef std::vector<dataInfo>  dataVector;
	class structInfo:public nameBase
	{
	public:
		structInfo();
		~structInfo();
		//int		   m_order;
		bool	   m_bRwPack;
		dataVector   m_vData;
		char m_strCommit[c_nameSize];
	};
	class msgInfo:public structInfo
	{
	public:
		msgInfo();
		~msgInfo();
		void setName(const char* szName);
		std::string m_packName;
		std::string m_msgIDName;
		std::string m_strBodyMaxSize;
		//std::string m_strZeroProc;
		uword      m_msgID;
		packetType m_packType;
		bool       m_bAutoWriteMsg;
	};
	class rpcInfo:public nameBase
	{
	public:
		void	 setName(const char* szName);
		msgInfo  m_ask;
		msgInfo  m_ret;
		bool     m_bRet;
		char m_strCommit[c_nameSize];
	};
	typedef rpcInfo* pRpcInfo;
	class pRpcInfoCmp
	{
	public:
		bool operator()(const pRpcInfo& pA, const pRpcInfo& pB)const;
	};
	//typedef tJumpListNS<pRpcInfo, 4,  pRpcInfoCmp> pRpcInfoListType;
	typedef std::vector<std::shared_ptr<rpcMgr::rpcInfo>> pRpcInfoListType;
	typedef pIStringKeyArraySet nameKeySet;
	//typedef pIStringKeyJumpList nameKeyList;
	typedef std::map<std::string, std::shared_ptr<structInfo>>  structInfoMap;
	class rpcArryInfo:public nameBase 
	{
	public:
		rpcArryInfo();
		~rpcArryInfo();
		//nameKeyList m_pStructInfoSet;
		structInfoMap m_pStructInfoSet;
		//tVector<structInfo*> m_pVst;
		std::vector<std::shared_ptr<structInfo>> m_pVst;
	    pRpcInfoListType m_pRpcInfoSet;
		//nameKeyList m_pRpcInfoSet;
		uword		m_nextMsgID;
		bool		m_b2js;
	};
	typedef rpcArryInfo* pRpcArryInfo;	
	rpcMgr();
	~rpcMgr();
	bool		  insert(std::shared_ptr<rpcMgr::rpcArryInfo>/*rpcArryInfo**/ pInfo);
	rpcInfo*      find(const char* arryName, const char* name);
	rpcArryInfo*  getRpcArry(const char* arryName);
	void          writeMsgPmpID();
	void		  writeMsgID();
	void          writeRpcArryH();
	void          writeRpcArryCpp();
	void          writeMsgH(std::ofstream& os, msgInfo& rMsg);
	void          writeMsgCpp(rpcArryInfo* pA,std::ofstream& os, msgInfo& rMsg);
	void		  writeInitMsgCpp();
	void		  writeRpcArry2js();
	void		  writeRpcArry2Ts();
	int			  chickDataTypeS();	
	char		  m_cppLibDir[c_dirSize];
	char		  m_jsLibDir[c_dirSize];
	char		  m_tsLibDir[c_dirSize];
	int			  m_OrderS;
private:
	//nameKeySet m_pRpcArryS;
	std::vector<std::shared_ptr<rpcMgr::rpcArryInfo>> m_pRpcArryS;
	//tVector<pRpcArryInfo> m_rpcV;
	std::vector<std::shared_ptr<rpcMgr::rpcArryInfo>> m_rpcV;
	void          writeVDataH(const char* szPer, std::ofstream& os, rpcMgr::structInfo& rSt);
	void	      writeVDataCpp(const char* szPer, std::ofstream& os, rpcMgr::structInfo& rSt, const char* szClass);
	void          writeOnceRpcArryH(rpcArryInfo* pA);

	void		  writeVDataTS(const char* szPer, std::stringstream& os,  dataVector& vData, const char* szStName);
	void          writeOnceRpcArry2TS(std::stringstream &os, rpcArryInfo* pA);

	void		  writeVDataJS(const char* szPer, std::ofstream& os,  dataVector& vData);
	void	      writeMsgJS(std::ofstream& os, msgInfo& rMsg);
	void          writeOnceRpc2jsFile(rpcArryInfo* pA);
	void          writeOnceRpcArryCpp(rpcArryInfo* pA);
	void		  writeOnceMsgID(rpcArryInfo* pA, bool bAuto);
};
#endif
