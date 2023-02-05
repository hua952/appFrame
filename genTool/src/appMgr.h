#ifndef _appMgr_h__
#define _appMgr_h__

#include"constVar.h"
#include"rpcMgr.h"
#include<string>
#include<vector>
#include <memory>

enum class appType:char 
{
		eApp_cpp,
		eApp_ts
};

class app:public rpcMgr::nameBase
{
public:
	app();
	~app();
	bool operator < (const app& other)const;
	struct procRpcInfo
	{
		//char m_rpcArryName[NameSize];
		//std::string m_funName;
		char conName[NameSize];
		char m_FunName[DirSize];
		char m_rpcName[NameSize];
		bool m_bAsk;
		bool m_bExit;
		bool m_bPass;
		bool m_bNeetPlayer;
		ubyte sendType;
		bool operator < (const procRpcInfo& other)const;
	};
	
	typedef tJumpListNS<procRpcInfo, 6> procRpcInfoList;
	struct  stPmpInfo
	{
		std::string strClass;
		std::string strNext;
		procRpcInfoList* pList;
	};
	typedef std::map<std::string,/* std::pair<std::string, procRpcInfoList*>*/stPmpInfo> otherPmpMap;
	bool addProcRpcInfo(const char* szName, bool bAsk, bool bExit, const char* szPmpName, const char* szClassName,
			const char* szNextPmp, const char* conName, int sendType, bool bPass, bool bNeetPlayer);
	const char* projectDir();
	void getGenPath (std::unique_ptr<char[]>& path);
	void getAppPath (std::unique_ptr<char[]>& path);
	void getDefFilePath (std::unique_ptr<char[]>& path);
	void getProcMsgPath (std::unique_ptr<char[]>& path);
	int  initPath ();
	appType  getAppType();
	void	 setAppType(appType at);
	procRpcInfoList&  procRpcList();
	char m_projectDir[DirSize];
	appType		m_appType;
	otherPmpMap m_otherPmpMap;
//private:
	procRpcInfoList  m_procRpcInfoList;
};
typedef app* pApp;

class realServer
{
public:
	typedef std::vector<std::shared_ptr<app>> appList;
	appList&  getAppList ();
	const char* name ();
	void        setName (const char* szName);
private:
	appList		m_appList;
	std::unique_ptr <char[]> m_name;
};

class appMgr
{
public:
	appMgr();
	~appMgr();
	bool insert(std::shared_ptr<app> pA);
	int  procApp();
	int  procOnceApp(app* pApp);
	typedef std::vector<std::shared_ptr<realServer>> vRealServerListT;
	//appList&  getAppList ();
	int writeRootCMakeFile ();
	vRealServerListT& realServerList ();
private:
	vRealServerListT m_realServerList;
	//appList		m_appList;
};
#endif
